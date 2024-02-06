cmake_minimum_required(VERSION 3.5)

project(CUDAAdvanceLibraries2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(_GLIBCXX_USE_CXX11_ABI 0)


if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/models)
set(MODEL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/models)
set(USE_MODELS true)
else()
set(MODEL_ROOT_DIR )
set(USE_MODELS false)
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp.in ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(CUDAAdvanceLibraries2 ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# Set the executable name and suffix
set_target_properties(CUDAAdvanceLibraries2 PROPERTIES OUTPUT_NAME segmentation_model_tester)

# Enforce a specific extension like ".exe"
set_target_properties(CUDAAdvanceLibraries2 PROPERTIES SUFFIX ".exe")

# Set the runtime output directory to be the `bin` directory
set_target_properties(CUDAAdvanceLibraries2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/)




set(LIBTORCH_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libtorch")
set(LIBTORCH_INCLUDE_DIR "${LIBTORCH_ROOT_DIR}/include/")
set(LIBTORCH_LIB_DIR "${LIBTORCH_ROOT_DIR}/lib/")


if(NOT EXISTS ${LIBTORCH_ROOT_DIR}/lib/libnvToolsExt)
    execute_process(
        COMMAND bash -c "cd ${LIBTORCH_ROOT_DIR}/lib && ln -s $(ls libnvToolsExt-*.*) libnvToolsExt"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE finalize
        COMMAND_ERROR_IS_FATAL ANY
        OUTPUT_QUIET
        ERROR_QUIET
        COMMAND_ECHO NONE
    )
endif()


set(CUDAToolkit_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cuda)

# First set cuda.
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.17")
    find_package(CUDAToolkit REQUIRED)
endif()
find_package(CUDA REQUIRED)

# Set a few variable
set(CUDAToolkit_ROOT ${CUDA_TOOLKIT_ROOT_DIR})
set(CUDA_SDK_TOOLKIT_DIR ${CUDA_TOOLKIT_ROOT_DIR})
set(CMAKE_CUDA_COMPILER ${CUDA_NVCC_EXECUTABLE})

# Step 1) Execute nvidia-smi command to get the architectures
execute_process(COMMAND nvidia-smi --query-gpu=compute_cap --format=csv OUTPUT_VARIABLE GPU_INFO)

# Step 2) Extract GPU architectures from the output
string(REGEX MATCHALL "[0-9]+\\.[0-9]+" GPU_ARCHITECTURES "${GPU_INFO}")

# Step 3) Remove the dot in the middle of the architectures
foreach(ARCHITECTURE IN LISTS GPU_ARCHITECTURES)
    string(REPLACE "." "" ARCHITECTURE_NO_DOT ${ARCHITECTURE})
    list(APPEND LOCAL_CUDA_ARCHITECTURES ${ARCHITECTURE_NO_DOT})
endforeach()

set_target_properties(CUDAAdvanceLibraries2 PROPERTIES CUDA_ARCHITECTURES "${LOCAL_CUDA_ARCHITECTURES}")


set(Torch_DIR ${LIBTORCH_ROOT_DIR}/share/cmake/Torch)

set(CAFFE2_USE_CUDNN ON)

find_package(Torch REQUIRED PATHS ${Torch_DIR} ${CUDAToolkit_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

#if(TORCH_FOUND)
#    message("The local torch was found!")
    message("${TORCH_INCLUDE_DIRS}")
    message("TORCH NON CUDA:${TORCH_LIBRARIES}")
    message("TORCH CUDA: ${TORCH_CUDA_LIBRARIES}")
    message("LIBS: ${CUDA_LIBRARIES}")
    message("INCLUDE: ${CUDA_INCLUDE_DIRS}")
#endif()


set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/opencv/lib/cmake/opencv4)
find_package(OpenCV REQUIRED core imgcodecs)

target_include_directories(CUDAAdvanceLibraries2 PRIVATE ${TORCH_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cuda/include)
target_link_directories(CUDAAdvanceLibraries2 PRIVATE  ${LIBTORCH_LIB_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cuda/lib64 ${OpenCV_INCLUDE_DIRS}/../../lib )


target_link_libraries(CUDAAdvanceLibraries2 PRIVATE
    -lbackend_with_compiler
    -lc10_cuda
#    -lc10d_cuda_test
    -lc10
    -ltorch_cpu
    -ltorch_cuda_linalg
    -ltorch_cuda
    -ltorch_global_deps
#    -ltorch_python
    -ltorch
    -lopencv_core
    -lopencv_imgcodecs
    -lopencv_imgproc
)


# UNIT TEST

add_subdirectory(test)



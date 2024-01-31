project(unit_tests)

set(MODEL_ROOT_DIR ${CMAKE_SOURCE_DIR}/models)
set(SOURCE_IMAGES_ROOT_DIR ${CMAKE_SOURCE_DIR}/data/src/images)
set(BINARY_SOURCE_FILENAME ${CMAKE_SOURCE_DIR}/bin/segmentation_model_tester.exe)
set(BINARY_TEST_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tgt)
set(BINARY_GT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gt)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.cpp.in ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)

add_executable(unit_tests test.cpp)

set(OCV_ROOT_DIR "${CMAKE_SOURCE_DIR}/third_party/opencv")
set(OCV_LIB_DIR "${OCV_ROOT_DIR}/lib")
set(OCV_INCLUDE_DIR "${OCV_ROOT_DIR}/include/opencv4")


target_include_directories(unit_tests PRIVATE ${OCV_INCLUDE_DIR})
target_link_directories(unit_tests PRIVATE ${OCV_LIB_DIR} )
target_link_libraries(unit_tests PRIVATE -lopencv_core -lopencv_imgcodecs -lopencv_imgproc)

set(CMAKE_EXECUTABLE_SUFFIX ".exe")

set_target_properties(unit_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests)
install(FILES ${CMAKE_SOURCE_DIR}/bin/test/unit_tests.exe DESTINATION tests)

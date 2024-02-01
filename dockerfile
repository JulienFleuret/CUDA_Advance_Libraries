FROM ubuntu:23.04

WORKDIR /opt


RUN apt update && apt install -yqq gcc g++ git wget bash unzip libxml2-dev cmake
RUN git clone https://github.com/JulienFleuret/CUDA_Advance_Libraries.git .
COPY ./models /opt/models
RUN bash setup.sh
COPY ~/Downloads/cudnn-linux-x86_64-8.9.3.28_cuda12-archive /opt/third_party
RUN cd third_party/cudnn-linux-x86_64-8.9.3.28_cuda12-archive/include && cp -a * ../../cuda/include && cd ../lib && cp -a * ../../cuda/lib64
RUN mkdir build && cd build && cmake .. && make
RUN echo $(ls)
RUN bin/tests/unit_tests.exe


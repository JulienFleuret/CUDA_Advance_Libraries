FROM ubuntu:23.04

WORKDIR /opt


RUN apt update && apt install -yqq gcc g++ git wget bash unzip libxml2-dev cmake
RUN git clone https://github.com/JulienFleuret/CUDA_Advance_Libraries.git .
COPY ./models /opt/models
RUN bash setup.sh
RUN mkdir build && cd build && cmake .. && make
RUN bin/tests/unit_tests.exe


FROM ubuntu:23.04

WORKDIR /opt


RUN apt update && apt install -y gcc g++ git wget bash
RUN git clone https://github.com/JulienFleuret/CUDA_Advance_Libraries.git .
RUN bash setup.sh
RUN mkdir build && cd build && cmake .. && make
RUN bin/tests/unit_tests.exe

COPY ./models /opt/models

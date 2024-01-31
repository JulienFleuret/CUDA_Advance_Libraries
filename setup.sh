#!/bin/bash

if [ ! -d "$PWD/third_party" ]; then
    ./scripts/download_libtorch.sh
    ./scripts/download_cuda.sh
    ./scripts/download_opencv.sh
elif [ ! -d "$PWD/third_party/libtorch" ]; then
    ./scripts/download_libtorch.sh
elif [ ! -d "$PWD/third_party/cuda" ]; then
    ./scripts/download_cuda.sh
elif [ ! -d "$PWD/third_party/opencv" ]; then
    ./scripts/download_opencv.sh
fi


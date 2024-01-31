#!/bin/bash

if [ ! -d "$PWD/models" ]; then
dvc pull models.dvc
fi

if [ ! -d "$PWD/third_party" ]; then
    bash $PWD/scripts/download_libtorch.sh
    bash $PWD/scripts/download_cuda.sh
    bash $PWD/scripts/download_opencv.sh
elif [ ! -d "$PWD/third_party/libtorch" ]; then
    bash $PWD/scripts/download_libtorch.sh
elif [ ! -d "$PWD/third_party/cuda" ]; then
    bash $PWD/scripts/download_cuda.sh
elif [ ! -d "$PWD/third_party/opencv" ]; then
    bash $PWD/scripts/download_opencv.sh
fi


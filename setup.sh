#!/bin/bash

if [ ! -d "$PWD/models" ]; then
dvc pull models.dvc
fi

if [ ! -d "$PWD/third_party" ]; then
    $(PWD)/scripts/download_libtorch.sh
    $(PWD)/scripts/download_cuda.sh
    $(PWD)/scripts/download_opencv.sh
elif [ ! -d "$PWD/third_party/libtorch" ]; then
    $(PWD)/scripts/download_libtorch.sh
elif [ ! -d "$PWD/third_party/cuda" ]; then
    $(PWD)/scripts/download_cuda.sh
elif [ ! -d "$PWD/third_party/opencv" ]; then
    $(PWD)/scripts/download_opencv.sh
fi


#!/bin/bash
mkdir third_party
wget https://download.pytorch.org/libtorch/cu121/libtorch-cxx11-abi-shared-with-deps-2.1.2%2Bcu121.zip -q
unzip libtorch-cxx11-abi-shared-with-deps-2.1.2+cu121.zip -qq
rm libtorch-cxx11-abi-shared-with-deps-2.1.2+cu121.zip
mv libtorch third_party/libtorch


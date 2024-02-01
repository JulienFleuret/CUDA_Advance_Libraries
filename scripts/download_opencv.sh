#!/bin/bash

mkdir ocv_dir
cd ocv_dir 
git clone https://github.com/opencv/opencv.git
cd opencv
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/../../../third_party/opencv -DBUILD_LIST=core,imgcodecs,imgproc -DWITH_PNG=ON -DWITH_JPEG=ON -DWITH_WEBP=ON -DWITH_TIFF=ON -DWITH_JASPER=ON -DWITH_EXR=ON -DWITH_GDCM=ON
make -j -l8
make install
rm -Rf ocv_dir

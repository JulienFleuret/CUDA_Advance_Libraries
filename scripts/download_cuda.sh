#!/bin/bash
wget https://developer.download.nvidia.com/compute/cuda/12.3.2/local_installers/cuda_12.3.2_545.23.08_linux.run && sh ./cuda_12.3.2_545.23.08_linux.run --toolkit --toolkitpath=$PWD/third_party/cuda --silent --override && rm cuda_12.3.2_545.23.08_linux.run


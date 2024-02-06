# Segmentation model tester
I did this project in the context of the module "CUDA Advance Libraries" from Coursera.

## Project Description

This project creates an executable named `segmentation_model_tester.exe`. This executable utilizes `libtorch`, the C++ frontend of `pytorch`, and is designed for evaluating the performance of a segmentation model after serialization between C++ and Python.

### Arguments for `segmentation_model_tester.exe`:
- `-h`, `-help`, `-usage`, or `-?`: Print help.
- `-images`,`-input`, or `-i` : specify the path to the data to use as input, which can be:
  - an image.
  - a path to a folder of images.
- `-output`, or `-o`: specify the path to a directory where to write the results.
- `-model`, or `-m`: path to a custom model or name of a pre-trained model. Available pre-trained models are: <br />
  - `UNet`
  - `UNet++`
  - `DeepLabV3`
  - `DeepLabV3+`
  - `RUNet`: Recurrent UNet.
  - `AUNet`: UNet with Attention layer.
  - `RAUNet`: Recurrent UNet with Attention Layer.
  - `PSPNet`
  - `FPN`
  - `MANet`
  - `LinkNet`
  - `PANet`
- `-batch_size`, or `-bs`: (optional, default: -1) batch size to use for the computations.
- `-width`, or `-w`: (optional, default: 224) width of the image expected by the model.
- `-height`, or `-h`: (optional, default: 224) height of the image expected by the model.
- `-mean`, or `-mu`: (optional, default: 0.485,0.456,0.406) mean to subract from the channels of the image. The values must be separated by a comma.
- `-std`, `-sigma`, or `-dev`: (optional, default: 0.229,0.224,0.225) standard deviation to normalize the channels of the image with. The values must be separated by a comma.

## Code Organization

`bin/`
This folder will only exist once the project is compiled, and will contain: `segmentation_model_tester.exe` and in a sub-directory named `tests`, `unit_tests.exe`.

`data/`
This folder contains 64 images, randomly taken from the dataset of the challenge PASCAL VOC 2012

`docker/`
This folder contains a script to install and run the docker image computed for this project.

`scripts/`
All the bash scripts that are used to install and configure the dependencies, required by this project (CUDA, LibTorch, OpenCV).

`src/`
All the source files that this project depends on are here.

`test/`
All the files related to the build of the unit test. Additionally, it contains the ground truth images used for the unit tests.

`CMAkeLists.txt`
Configuration file.

`run.sh`
Run the executable (arguments must be provided). If the project was not compiled create the project first. If the pre-trained models have not been downloaded, download the models (requires DVC).

`setup.sh`
Install all the dependencies and prepare all the dependencies.

`models.dvc`
DVC file to download the pre-trained models.

`dockerfile`
Dockerfile used to create the docker images that is used by the script in the `docker` folder.




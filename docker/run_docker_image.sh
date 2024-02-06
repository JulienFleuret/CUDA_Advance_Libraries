#!/bin/bash

IMAGE_NAME="jf42673/cuda_advance_lib"
TAG="latest"

# Check if the Docker image exists locally
if docker images "${IMAGE_NAME}:${TAG}" | grep -q "${IMAGE_NAME}"; then
    echo "Docker image '${IMAGE_NAME}:${TAG}' exists locally."
else
    echo "Docker image '${IMAGE_NAME}:${TAG}' does not exist locally. Pulling from Docker Hub..."
    docker login
    docker pull "${IMAGE_NAME}:${TAG}"
fi

docker run --runtime=nvidia --gpus all -it ${IMAGE_NAME}:${TAG} /bin/bash

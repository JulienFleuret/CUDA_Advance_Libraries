#!/bin/bash

# Initialize default values for optional parameters
model="UNet"
batch_size="-1"
width="224"
height="224"
mean="0.485,0.456,0.406"
std="0.229,0.224,0.225"

# Function to display help/usage message
usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, -help, -usage, -?: Show this help message"
    echo "  -images, -input, -i <path>: Specify input images path"
    echo "  -output, or -o <path>: Specify output path"
#    echo "  -model, or -m <model>: Specify model (optional if default models are presents, mandatory otherwise, default: UNet)"
    echo "  -model, or -m <model>: Specify model (optional, default: UNet)"
    echo "  -batch_size, or -bs <size>: Specify batch size (optional, default: -1)"
    echo "  -width, or -w <width>: Specify the image width expected by the model (optional, default: 224)"
    echo "  -height, or -h <height>: Specify the image height expected by the model (optional, default: 224)"
    echo "  -mean, or -mu <mean>: Specify mean (optional, default: 0.485,0.456,0.406)"
    echo "  -std, -sigma, or -dev <std>: Specify standard deviation (optional, default: 0.229,0.224,0.225)"
    exit 1
}

# Check if no arguments were provided
if [ $# -eq 0 ]; then
    usage
    exit 1
fi

# Parse arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|-help|-usage|-\?)
            usage
            ;;
        -images|-input|-i)
            images="$2"
            shift 2
            ;;
        -output|-o)
            output="$2"
            shift 2
            ;;
        -model|-m)
            model="$2"
            shift 2
            ;;
        -batch_size|-bs)
            batch_size="$2"
            shift 2
            ;;
        -width|-w)
            width="$2"
            shift 2
            ;;
        -height|-h)
            height="$2"
            shift 2
            ;;
        -mean|-mu)
            mean="$2"
            shift 2
            ;;
        -std|-sigma|-dev)
            std="$2"
            shift 2
            ;;

        *)
            echo "Unknown option: $1"
            usage
            ;;
    esac
done

# Check for required arguments
if [ -z "$input" ] && [ -z "$output" ]; then
    echo "The following arguments -input, and -output must be provided." >&2
    usage
fi

if [ ! -d models ]; then

if which dvc >/dev/null 2>&1; then
    dvc pull models.dvc
else
    echo "DVC is required in order to download the pre-trained models."
fi

fi

./bin/segmentation_model_tester.exe -i=$images -o=$output -m=$model -bs=$batch_size -w=$width -h=$height -mu="$mean" -sigma="$std"
	

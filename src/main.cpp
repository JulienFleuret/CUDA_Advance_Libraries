#include <iostream>
#include <filesystem>
#include <regex>
#include <cstdlib>

#include <torch/torch.h>
#include <torch/script.h>

#include <opencv2/core.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
namespace fs = std::filesystem;

///
/// \brief split_string : split a string into token and convert them into floating point values.
/// \param str : string to process.
/// \param delim : delimiter character or string.
/// \return a vector of single precision floating points numbers.
///
std::vector<float> split_string(const cv::String& str, const cv::String& delim);

///
/// \brief getPreTrainedModelRoot : provide the path where the pre-trained model are stored.
/// \return path to the directory where the pre-trained models are stored.
///
inline constexpr const char* getPreTrainedModelRoot()
{
    return "/home/smile/prog/CUDAAdvanceLibraries2/models";
}

///
/// \brief usePretrainedModel : true if the pre-trained models are provided, false otherwise.
/// \return true if the pre-trained models are provided, false otherwise.
///
inline constexpr bool usePretrainedModel()
{
    return true;
}

///
/// \brief The generate_output_filename_helper_t class : helper class to generate the output filename
/// given the input filename.
///
struct generate_output_filename_helper_t
{
    /// \brief path to the directory where to save the processed image.
    fs::path output_dir;

    ///
    /// \brief generate_output_filename_helper_t : parametric constructor
    /// \param _output_dir : path to the directory where to save the processed image.
    ///
    inline generate_output_filename_helper_t(const cv::String& _output_dir):
        output_dir(_output_dir)
    {
        if(!cv::utils::fs::exists(_output_dir))
            cv::utils::fs::createDirectories(_output_dir);
    }

    ///
    /// \brief get : generate an output path given the path of an input file.
    /// \param _input_filename : path of an input file.
    /// \return output path.
    ///
    inline cv::String get(const cv::String& _input_filename) const
    {
        return this->output_dir / fs::path(_input_filename).filename();
    }
};

///
/// \brief process_images : apply a segmentation model to one or more images.
/// \param device : device to use for the computation.
/// \param filenames : name of images to process.
/// \param batch_size : how many images to process at a time.
/// \param width : width of the image expected by the model.
/// \param heigth : height of the image expected by the model.
/// \param generate_output_filename : helper functor to find the output filename.
/// \param model : model to use for processing the data.
/// \return true if all the images were properly processed and saved.
///
bool process_images(
        const torch::Device& device,
        const std::vector<cv::String>& filenames,
        const std::size_t& batch_size, const int &width, const int &height,
        const torch::ArrayRef<float>& _mu, const torch::ArrayRef<float>& _dev,
        const generate_output_filename_helper_t& generate_output_filename,
        torch::jit::Module& model);

int main(int argc, char* argv[])
{

try {
        cv::String keys;
        if constexpr(usePretrainedModel())
        {
            keys=
                    "{help usage ? | | print this message}"
                    "{model m |UNet | model to use.\n"
                    "		-Custom serialized model (provide the path to the serialized model).\n"
                    "		Available pretrained models are: \n"
                    "		-UNet: classical UNet model\n"
                    "		-UNet++: improved UNet model\n"
                    "		-DeepLabV3\n"
                    "		-DeepLabV3+\n"
                    "		-RUNet: Recurrent UNet model\n"
                    "		-AUNet: UNet with attention layer\n"
                    "		-RAUNet: Recurrent UNet with attention layer\n"
                    "		-PSPNet\n"
                    "		-FPN\n"
                    "		-MANet\n"
                    "		-LinkNet\n"
                    "		-PANet\n"
                    "}"
                    "{images input i| | path or image to use as input}"
                    "{output o| | path where to write the output, must be a directory}"
                    "{batch_size bs|-1| number of images to process at a time}"
                    "{width w|224| width of the image expected by the model}"
                    "{height h|224| heigth of the image expected by the model}"
                    "{mean mu|0.485,0.456,0.406| mean to subract from the channels of the image. The values must be separated by a comma}"
                    "{std sigma dev|0.229,0.224,0.225| standard deviation to normalize the channels of the image with. The values must be separated by a comma}";
        }
        else
        {
            keys=
                    "{help usage ? | | print this message.}"
                    "{model m |UNet | path to the model to use.}"
                    "{images input i| | path or image to use as input}"
                    "{output o| | path where to write the output, must be a directory}"
                    "{batch_size bs|-1| number of images to process at a time}"
                    "{width w|224| width of the image expected by the model}"
                    "{height h|224| heigth of the image expected by the model}"
                    "{mean mu|0.485,0.456,0.406| mean to subract from the channels of the image. The values must be separated by a comma}"
                    "{std sigma dev|0.229,0.224,0.225| standard deviation to normalize the channels of the image with. The values must be separated by a comma}";
        }


    cv::CommandLineParser parser(argc, argv, keys);

    parser.about("segmentation_model_tester.exe: a simple program to evaluate serialized models.");
    
    if(argc == 1)
    {
        parser.printErrors();
        return EXIT_FAILURE;
    }
    
    if(parser.has("help"))
    {
        parser.printMessage();
        return EXIT_SUCCESS;
    }

    if(!parser.has("images"))
    {
        std::cerr<<"Please specify the path to an image or to a folder of images!"<<std::endl;
        return EXIT_FAILURE;
    }

    if(!parser.has("output"))
    {
        std::cerr<<"Please specify the path to a directory where to write the output!"<<std::endl;
        return EXIT_FAILURE;
    }


    // Step 0) Parse the inputs.
    cv::String model_arg = parser.get<cv::String>("model");
    cv::String input_path = parser.get<cv::String>("input");
    cv::String output_path = parser.get<cv::String>("output");
    int width = parser.get<int>("width");
    int height = parser.get<int>("height");

    std::vector<float> mu = split_string(parser.get<cv::String>("mean"), ",");
    std::vector<float> stddev = split_string(parser.get<cv::String>("std"), ",");

    torch::jit::Module model;
    torch::Device device = torch::cuda::is_available() ? torch::kCUDA : torch::kCPU;

    int min_batch_size = 1;

    int batch_size = parser.get<int>("batch_size");

    if constexpr(usePretrainedModel())
    {
        cv::String pre_trained_models[]  = {        "unet",
                                                    "unetpp",
                                                    "deeplabv3",
                                                    "deeplabv3p",
                                                    "runet",
                                                    "aunet",
                                                    "raunet",
                                                    "pspnet",
                                                    "fpn",
                                                    "manet",
                                                    "linknet",
                                                    "panet"};

        //Step 1) Load the model.
        if(cv::utils::fs::exists(model_arg))
        {
            // If the model is a valid path.
            model = torch::jit::load(model_arg, device);
        }
        else
        {
            // If the model is a pre-trained model.

            // replace the `+` by `p` and makes all the characters lower case.
            std::transform(model_arg.begin(), model_arg.end(), model_arg.begin(),
                           [](const char& c) -> char
            {
                return c=='+' ? 'p' : static_cast<char>(std::tolower(static_cast<int>(c)));
            });

            bool model_found(false);


            for(const auto& pmn : pre_trained_models)
                if(model_arg == pmn)
                {
                    model_found = true;

                    // For some reason some it was not possible to script some model with a batch size of 1.
                    if((model_arg.find("deeplabv3") != std::string::npos) || (model_arg == "panet"))
                        min_batch_size = 2;
                }

            if(!model_found)
            {
                std::cout<<"The specified model argument is not a pre-trained model, nor a path to a pre-trained model!"<<std::endl;
                return EXIT_FAILURE;
            }

            model = torch::jit::load(cv::utils::fs::join(getPreTrainedModelRoot(), model_arg) + ".pt", device);
        }
    }
    else
    {
        //Step 1) Load the model.
        if(cv::utils::fs::exists(model_arg))
        {
            // If the model is a valid path.
            model = torch::jit::load(model_arg, device);
        }
        else
        {
            std::clog<<"The path provided for the model is invalid!"<<std::endl;
            return EXIT_FAILURE;
        }
    }

    // Update the batch size if required.
    batch_size = std::max(min_batch_size, parser.get<int>("batch_size"));

    // Step 2) Load the data.
    if(!cv::utils::fs::exists(input_path))
    {
        std::cerr<<"The specified input does not exists!"<<std::endl;
        return EXIT_FAILURE;
    }

    generate_output_filename_helper_t generate_output_filename(output_path);


    if(cv::utils::fs::isDirectory(input_path))
    {
        // If the input is the path of a directory.
        std::vector<cv::String> filenames;

        // List all the files in the source directory.
        cv::utils::fs::glob(input_path,"*.*", filenames);

        if(filenames.empty())
        {
            std::cout<<"The provided folder does not contains any images"<<std::endl;
            return EXIT_SUCCESS;
        }

        // Keep only the files with extensions supported by both `cv::imread` and `cv::imwrtie`.
        {
            std::vector<cv::String> tmp;

            tmp.reserve(filenames.size());

            for(auto& filename : filenames)
            {
                if(cv::haveImageReader(filename) && cv::haveImageWriter(filename))
                    tmp.push_back(std::move(filename));
            }

            tmp.shrink_to_fit();

            filenames = std::move(tmp);
        }

        // Process all the images.
        if(!process_images(device, filenames, static_cast<std::size_t>(batch_size), width, height, mu, stddev, generate_output_filename, model))
            return EXIT_FAILURE;
    }
    else
    {
        // Process the image.
        if(!process_images(device, {input_path}, static_cast<std::size_t>(batch_size), width, height, mu, stddev, generate_output_filename, model))
            return EXIT_FAILURE;
    }
}
catch(const std::exception& err)
{
std::clog<<"An Unexpected Error Has Occured: "<<err.what()<<std::endl;
return EXIT_FAILURE;
}
    std::cout << "done! :)" << std::endl;
    return EXIT_SUCCESS;
}

///
/// \brief split_string : split a string into token and convert them into floating point values.
/// \param str : string to process.
/// \param delim : delimiter character or string.
/// \return a vector of single precision floating points numbers.
///
std::vector<float> split_string(const cv::String& str, const cv::String& delim)
{
    std::list<float> dst;

    // Regular expression for comma delimiter
    std::regex regexDelimiter(",");

    // Use sregex_token_iterator to split the string
    std::sregex_token_iterator iter(str.begin(), str.end(), regexDelimiter, -1);
    std::sregex_token_iterator end;

    // Print the split parts
    while (iter != end)
    {
        dst.emplace_back(std::atof(iter->str().c_str()));
        ++iter;
    }

    return std::move(std::vector<float>(dst.begin(), dst.end()));
}


///
/// \brief use_torch_for_upsampling : function use to set which upsampling library should be used, between Torch and OpenCV.
/// \return true if the define USE_TORCH_FOR_SAMPLING is set, false otherwise.
///
inline constexpr bool use_torch_for_upsampling()
{
    return
        #ifdef USE_TORCH_FOR_UPSAMPLING
            true
        #else
            false
        #endif
            ;
}

///
/// \brief save_masks : save the result of a instance of a serialized model as an colour image.
/// \param image : masks (dimensions [N x H x W], where N is the number of classes possible.
/// \param filename : name of the file to write.
/// \return true if the file was successfuly written, false otherwise.
///
bool save_masks(const torch::Tensor& _image, const cv::String& filename, const cv::Size& _orignal_size = cv::Size())
{
    std::array<uchar, 66> lookup_table_ =
    {
        0,0,0,
        0,0,0x80,
        0,0x80,0,
        0,0x80,0x80,
        0x80,0,0,
        0x80,0,0x80,
        0x80,0x80,0,
        0x80,0x80,0x80,
        0,0,0x40,
        0,0,0xc0,
        0,0x80,0x40,
        0,0x80,0xc0,
        0x80,0,0x40,
        0x80,0,0xc0,
        0x80,0x80,0x40,
        0x80,0x80,0xc0,
        0,0x40,0,
        0,0x40,0x80,
        0,0xc0,0,
        0,0xc0,0x80,
        0x40,0x80,0,
        0xFF,0xFF,0xFF
    };

    torch::Tensor image = _image.to(torch::kCPU).squeeze();

    cv::Mat dst(image.size(0), image.size(1), CV_8UC1, image.data_ptr<uchar>());


    cv::Mat3b tmp(dst.size());

    std::transform(dst.begin<uchar>(), dst.end<uchar>(),
                   tmp.begin(),
                   [&lookup_table_](const uchar& v)->cv::Vec3b
    {
        int colour_idx = static_cast<int>(v) * 3;


        cv::Vec3b colour;

        std::copy_n(std::addressof(lookup_table_.at(colour_idx)), 3, colour.val);

        return colour;
    });

    if(_orignal_size.empty())
    {
        dst = tmp;
    }
    else
    {
        cv::resize(tmp, dst, _orignal_size);
    }

    return cv::imwrite(filename, dst);

}



///
/// \brief process_images : apply a segmentation model to one or more images.
/// \param device : device to use for the computation.
/// \param filenames : name of images to process.
/// \param batch_size : how many images to process at a time.
/// \param width : width of the image expected by the model.
/// \param heigth : height of the image expected by the model.
/// \param generate_output_filename : helper functor to find the output filename.
/// \param model : model to use for processing the data.
/// \return true if all the images were properly processed and saved.
///
bool process_images(const torch::Device& device,
                    const std::vector<cv::String>& filenames,
                    const std::size_t& batch_size,
                    const int& width, const int& height,
                    const torch::ArrayRef<float>& _mu, const torch::ArrayRef<float>& _dev,
                    const generate_output_filename_helper_t& generate_output_filename,
                    torch::jit::Module& model)
{

    // Image net mean and standard deviation per channels.
    auto mu = torch::tensor(_mu, torch::kFloat32).unsqueeze(1).unsqueeze(2).to(device);
    auto sigma = torch::tensor(_dev, torch::kFloat32).unsqueeze(1).unsqueeze(2).to(device);


    std::vector<torch::Tensor> images(batch_size);
    std::vector<cv::Size> images_original_sizes(batch_size);

    images.shrink_to_fit();
    images_original_sizes.shrink_to_fit();

    auto filenames_it = filenames.begin();

    // for all the files...
    for(std::size_t i=0; i<filenames.size(); i+=batch_size, filenames_it+=batch_size)
    {
        // Step 0) adjust the batch size for the current iteration.
        std::size_t max_batch_size = std::min(batch_size, filenames.size() - i);

        // Step 1) load and pre-process all the images.
        for(std::size_t j=0; j<max_batch_size; ++j)
        {
            cv::Mat tmp = cv::imread(*(filenames_it + j), cv::IMREAD_COLOR);

            torch::Tensor ret = torch::from_blob(tmp.ptr(),{1, tmp.rows, tmp.cols, tmp.channels()}, torch::kByte)
                    .to(device)
                    .permute({0,3,1,2})
                    .to(torch::kFloat32);

            images.at(j) = std::move(torch::resize(ret, {1, 3, height, width}));
            images_original_sizes.at(j) = tmp.size();
        }

        // Step 2) apply the segmentation algorithm.
        torch::Tensor X = std::move((images.size() == 1 ? images.front() : torch::cat(images)).sub(mu).div(sigma));

        torch::Tensor y = std::move(model.forward({X}).toTensor());

        y = y.argmax(1).to(torch::kByte);



        // Step 3) save the images.
        for(std::size_t j=0; j<max_batch_size; ++j)
        {
            auto orignal_size = images_original_sizes.at(j);

            torch::Tensor tmp = y[j];

            // Re-upsample the detection to the size of the original size and apply a colormap to distinguish the classes.
            // Only the colour map is saved.
            if constexpr(use_torch_for_upsampling())
            {
                tmp = at::upsample_bicubic2d(tmp.unsqueeze(0).unsqueeze(0).to(torch::kFloat32), {orignal_size.height, orignal_size.width}, true,1.,1.).to(torch::kByte);

                orignal_size = cv::Size();
            }


            if(!save_masks(tmp, generate_output_filename.get(*(filenames_it + j)), orignal_size))
            {
                return false;
            }
        }
    }

    return true;
}











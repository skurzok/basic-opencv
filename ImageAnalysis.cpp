
#include "ImageAnalysis.h"
#include "utils.h"


class EuclideanDistance
{
public:
    explicit EuclideanDistance(float threshold):
        threshold_(threshold)
    {}
    bool operator()(const cv::Vec3b & base, const cv::Vec3b & candidate)
    {// http://answers.opencv.org/question/68718/distance-between-images-pixel-by-pixel/
        return cv::norm(base, candidate) < threshold_;
    }
private:
    float threshold_;
};


namespace image_analysis_service
{
    

const std::string ImageAnalysis::find_region_str = "find_region";
const std::string ImageAnalysis::find_perimeter_str = "find_perimeter";
const std::string ImageAnalysis::save_str = "save";
const std::string ImageAnalysis::display_str = "display";   

void ImageAnalysis::doFindRegion(std::string command, const Image & image, Selection & output)
{
    if (!output.empty())
    {
        throw std::runtime_error("find_region must be first command (expected display)");
    }
    command = command.substr(find_region_str.size()); 
    removeCharsFromString(command, "() ");
    auto args = split(command, ',');
    if (args.size() == 2)
    {
        int x = std::stoi(args[0]);
        int y = std::stoi(args[1]);
        output = findRegion(image, x, y, EuclideanDistance(30));
    }
    else if (args.size() == 3)
    {
        int x = std::stoi(args[0]);
        int y = std::stoi(args[1]);
        int d = std::stoi(args[2]);
        output = findRegion(image, x, y, EuclideanDistance(d));
    }
    else
    {
        throw std::runtime_error("Incorrect number of parameters in find_region");
    }   
}


void ImageAnalysis::doFindPerimeter(Selection & output)
{ 
    if (output.empty())
    {
        throw std::runtime_error("find region before finding perimeter");
    }
    output = findPerimeter(output);
}

void ImageAnalysis::doSave(std::string command, Selection & output)
{ 
    command = command.substr(save_str.size());
    removeCharsFromString(command, "() ");
    auto args = split(command, ',');
    if (args.size() == 0)
    {
        throw std::runtime_error("No file name given in save");
    }
    auto path = args[0];
    if (args.size() == 1)
    {
        output.Save(path);
    }
    else if (args.size() == 2)
    {
        if (args[1] == "all")
        {
            output.SaveAsText(path, Selection::ALL_PIXELS);
        }
        else if (args[1] == "selected")
        {
            output.SaveAsText(path, Selection::SELECTED_PIXELS);
        }
        else
        {
            throw std::runtime_error("Unknown type of save as text");
        }
    }
    else
    {
        throw std::runtime_error("Incorrect number of parameters in save");
    }
}

void ImageAnalysis::doFindPerimeter(const Image & image, Selection & output, int cmd_num)
{ 
    if (cmd_num == 0)
    {
        image.Show("image");
    }
    else
    {
        output.Show("image");
    }
}

void ImageAnalysis::execute(const Image & image, const std::string & input_commands)
{
    //TODO: handle illy formated commands
    // input_image operations_definition [[otputformat:]output_file]
    // operations_definition = command(args)|command(args)|command(args)
    // comand = find_region | find_perimeter | display | save
    // find_region = find_region
    
    auto commands = split(input_commands, '|');
    Selection output;
    int cmd_num = 0;
    for (auto & command : commands)
    {
        if (command.substr(0, find_region_str.size()) == find_region_str)
        {
            doFindRegion(command, image, output);
        }
        else if (command.substr(0, find_perimeter_str.size()) == find_perimeter_str)
        {
            doFindPerimeter(output); 
        }
        else if (command.substr(0, save_str.size()) == save_str)
        {
            doSave(command, output);
        }
        else if (command.substr(0, display_str.size()) == display_str)
        {
            doFindPerimeter(image, output, cmd_num);
        }
        else
        {
            throw std::runtime_error("Unknown command: " + command);
        }
        ++cmd_num;
    }
}

Selection ImageAnalysis::findPerimeter(const Selection & region)
{
    Selection perimeter(region.rows(), region.cols());
    
    int i, j;
    for (i = 1; i < region.rows() - 1; ++i) {
        for (j = 1; j < region.cols() - 1; ++j) {
            
            if (region.at(i, j) == Selection::FILLED &&
                (region.at(i + 1, j) == Selection::UNCHECKED ||
                region.at(i - 1, j) == Selection::UNCHECKED ||
                region.at(i, j + 1) == Selection::UNCHECKED ||
                region.at(i, j - 1) == Selection::UNCHECKED))
            {
                perimeter.at(i, j) = Selection::FILLED;
            }
        }
    }
    
    return perimeter;
    //TODO: What if whole input is single region?
}
}


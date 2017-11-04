#include <vector>
#include <algorithm>
#include <string>

#include "Selection.h"


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
    
template<typename Out>
void split(const std::string &s, char delim, Out result) { // https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) { 
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

void removeCharsFromString( std::string &str, const char* charsToRemove ) {  // https://stackoverflow.com/questions/5891610/how-to-remove-certain-characters-from-a-string-in-c
   for ( unsigned int i = 0; i < strlen(charsToRemove); ++i ) { 
      str.erase( remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
   }
}
    
class ImageAnalysis
{
public:
    static std::string execute(const Image & image, const std::string & input_commands)
    {
        //TODO: handle illy formated commands
        // input_image operations_definition [[otputformat:]output_file]
        // operations_definition = command(args)|command(args)|command(args)
        // comand = find_region | find_perimeter | display | save
        // find_region = find_region
        const std::string find_region_str = "find_region";
        const std::string find_perimeter_str = "find_perimeter";
        const std::string save = "save";
        const std::string display = "display";
        
        auto commands = split(input_commands, '|');
        Selection output;
        int cmd_num = 0;
        for (auto & command : commands)
        {
            if (command.substr(0, find_region_str.size()) == find_region_str)
            {
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
                    return "Incorrect number of parameters in find_region";
                }
            }
            else if (command.substr(0, find_perimeter_str.size()) == find_perimeter_str)
            {
                //command = command.substr(find_perimeter.size());
                //r//emoveCharsFromString(command, "() ");
                //args = split(command, '|');
                output = findPerimeter(output);
            }
            else if (command.substr(0, save.size()) == save)
            {
                command = command.substr(save.size());
                removeCharsFromString(command, "() ");
                auto args = split(command, ',');
                if (args.size() == 0)
                {
                    return "No file name given in save";
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
                        return "Incorrect number of parameters in save as text";
                    }
                }
                else
                {
                    return "Incorrect number of parameters in save";
                }
            }
            else if (command.substr(0, display.size()) == display)
            {
                command = command.substr(display.size());
                if (cmd_num == 0)
                {
                    image.Show("image");
                }
                else
                {
                    output.Show("image");
                }
            }
            ++cmd_num;
        }
        return "";
    }
    

public:
    template<class T>
    static Selection findRegion(const Image & image, int x, int y, T metric)
    {
        struct Cord{int x, y;};
        std::vector<Cord> to_check(1, Cord{x, y});
        
        Selection status(image.rows(), image.cols());
        
        auto pattern = image.at(x, y);
        
        while (to_check.size())
        {
            auto pos = to_check.back();
            to_check.pop_back();
            
            if (status.at(pos.x, pos.y) == Selection::UNCHECKED &&            
                metric(pattern, image.at(pos.x, pos.y)))
            {
                status.at(pos.x, pos.y) = Selection::FILLED;
                if (pos.x - 1 >= 0 && // pos.x - 1 < image.rows &&
                    status.at(pos.x - 1, pos.y) == Selection::UNCHECKED)
                {
                    to_check.push_back(Cord{pos.x - 1, pos.y});
                }
                if (pos.x + 1 < image.rows() &&
                    status.at(pos.x + 1, pos.y) == Selection::UNCHECKED)
                {
                    to_check.push_back(Cord{pos.x + 1, pos.y});
                }
                if (pos.y - 1 >= 0 && // pos.x - 1 < image.rows &&
                    status.at(pos.x, pos.y - 1) == Selection::UNCHECKED)
                {
                    to_check.push_back(Cord{pos.x, pos.y - 1});
                }
                if (pos.y + 1 < image.cols() &&
                    status.at(pos.x, pos.y + 1) == Selection::UNCHECKED)
                {
                    to_check.push_back(Cord{pos.x, pos.y + 1});
                }
                //TODO: diagonal ?
            }
        }
        //std::cout << "filed " << dbg_filed << std::endl;
        return status;
    }
    

    static Selection findPerimeter(const Selection & region)
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


};
    
}

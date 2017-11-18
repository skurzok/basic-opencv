
#include "ImageAnalysis.h"

#include <algorithm>
#include <memory>

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
const std::string ImageAnalysis::find_smooth_perimeter_str = "find_smooth";
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


void ImageAnalysis::doFindSmoothPerimeter(Selection & output)
{
    if (output.empty())
    {
        throw std::runtime_error("find region before finding perimeter");
    }
    output = findSmoothPerimeter(output);
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
        else if (command.substr(0, find_smooth_perimeter_str.size()) == find_smooth_perimeter_str)
        {
            doFindSmoothPerimeter(output); 
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
            
            //if (region.at(i, j) == Selection::FILLED &&
            //    (region.at(i + 1, j) == Selection::UNCHECKED ||
            //    region.at(i - 1, j) == Selection::UNCHECKED ||
            //    region.at(i, j + 1) == Selection::UNCHECKED ||
            //    region.at(i, j - 1) == Selection::UNCHECKED))
            //{
            //    perimeter.at(i, j) = Selection::FILLED;
            //}
            if (region.at(i, j) == Selection::FILLED)
            {
                if (region.at(i + 1, j) == Selection::UNCHECKED)
                {
                    perimeter.at(i + 1, j) = Selection::FILLED;
                }
                if (region.at(i - 1, j) == Selection::UNCHECKED)
                {
                    perimeter.at(i - 1, j) = Selection::FILLED;
                }
                if (region.at(i, j + 1) == Selection::UNCHECKED)
                {
                    perimeter.at(i, j + 1) = Selection::FILLED;
                }
                if (region.at(i, j - 1) == Selection::UNCHECKED)
                {
                    perimeter.at(i, j - 1) = Selection::FILLED;
                }
            }
        }
    }
    
    return perimeter;
    //TODO: What if whole input is single region?
}

struct Point{
    int x, y;
};

enum{
    DEADEND,
};

//Selection debug_view;
//int max_points = 500;

bool select(std::vector<Point> & points, int i, int j, cv::Mat & paths, int path_id)
{
    points.push_back(Point{i, j});
    paths.at<int>(i, j) = 0;
    
    //debug_view.at(i, j) = 127;
    //debug_view.Show("debug");
    
    auto check = [&points, &paths, path_id](int x, int y) -> bool
    {
//std::cout << "checking " << x << "," << y << std::endl;
        if (x >= 0 && y >= 0 && x < paths.rows && y < paths.cols)
        {
            if (points.size() > 0 && points[0].x == x && points[0].y == y)
            {
                return true;    //TODO: add conditions for border or mark border as perimeter
            }
            if (paths.at<int>(x, y) == path_id)
            {
                auto f = select(points, x, y, paths, path_id);
                if (f)
                {
                    return true;
                }
                //std::cout << "back" << std::endl;
                paths.at<int>(x, y) = path_id;
                //debug_view.at(x, y) = 0;
                points.pop_back();
            }
        }
        return false;
    };
    if (check(i + 1, j)) return true;
    if (check(i + 1, j + 1)) return true;
    if (check(i,     j + 1)) return true;
    if (check(i - 1, j + 1)) return true;
    if (check(i - 1, j)) return true;
    if (check(i - 1, j - 1)) return true;
    if (check(i,     j - 1)) return true;
    if (check(i + 1, j - 1)) return true;

    return false;
}


std::vector<Point> makeSequence(cv::Mat & paths, int path_id, const Point & start)
{

    //debug_view = Selection(paths.rows, paths.cols);
    //std::cout << "start is at" << start.x << "," << start.y << std::endl;
    std::vector<Point> points;
    int td = select(points, start.x, start.y, paths, path_id);
    std::cout << "points" << std::endl;
    for (const auto & p : points)
    {
        std::cout << p.x << ", " << p.y << "   ";
    }
    return points;
}

int mark(int i, int j, const Selection & perimeter, cv::Mat & paths, int path_id)
{
    int total = 1;
    paths.at<int>(i, j) = path_id;
    
    auto check = [&total, &paths, &perimeter, path_id](int x, int y)
    {
            
        if (x >= 0 && y >= 0 && x < perimeter.rows() && y < perimeter.cols() &&
            perimeter.at(x, y) == Selection::FILLED &&
            paths.at<int>(x, y) == 0)
        {
            total += mark(x, y, perimeter, paths, path_id);
        }
    };
    check(i + 1, j);
    check(i + 1, j + 1);
    check(i,     j + 1);
    check(i - 1, j + 1);
    check(i - 1, j);
    check(i - 1, j - 1);
    check(i,     j - 1);
    check(i + 1, j - 1);
    
    return total;
}


std::vector<Point> getBiggestPath(const Selection & perimeter) //TODO: retun all paths bigger then some limit
{
    // check selection and find all (biggest) path
    
    cv::Mat paths(perimeter.rows(), perimeter.cols(), CV_32S);
    
    std::vector<int> sizes(1, 0);
    std::vector<Point> points(1, Point{-1, -1});
    for (int i = 1; i < perimeter.rows() - 1; ++i) {
        for (int j = 1; j < perimeter.cols() - 1; ++j) {
            
            if (perimeter.at(i, j) == Selection::FILLED && paths.at<int>(i,j) == 0)
            {
                int s = mark(i, j, perimeter, paths, sizes.size()); //TODO: consider different implementetion, current may result in stack overflow
                sizes.push_back(s);
                points.push_back(Point{i, j});
            }
        }
    }
    int biggest_path_id = std::distance(sizes.begin(), std::max_element(sizes.begin(), sizes.end()));
    

//paths.setTo(1000000, paths == biggest_path_id); 

       // namedWindow("paths", cv::WINDOW_AUTOSIZE);
      ///  imshow("paths", paths);
      //  cv::waitKey(0);
    
    auto points = makeSequence(paths, biggest_path_id, points[biggest_path_id]);
    
    
}

Selection ImageAnalysis::findSmoothPerimeter(const Selection & region)
{
    Selection perimeter(region.rows(), region.cols());
    
    int i, j;
    for (i = 1; i < region.rows() - 1; ++i) {
        for (j = 1; j < region.cols() - 1; ++j) {

            if (region.at(i, j) == Selection::FILLED)
            {
                if (region.at(i + 1, j) == Selection::UNCHECKED)
                {
                    perimeter.at(i + 1, j) = Selection::FILLED;
                }
                if (region.at(i - 1, j) == Selection::UNCHECKED)
                {
                    perimeter.at(i - 1, j) = Selection::FILLED;
                }
                if (region.at(i, j + 1) == Selection::UNCHECKED)
                {
                    perimeter.at(i, j + 1) = Selection::FILLED;
                }
                if (region.at(i, j - 1) == Selection::UNCHECKED)
                {
                    perimeter.at(i, j - 1) = Selection::FILLED;
                }
            }
        }
    }
    getBiggestPath(perimeter);
    return perimeter;
    //TODO: What if whole input is single region?
}
}


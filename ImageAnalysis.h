#include <vector>
#include <algorithm>
#include <string>

#include "Selection.h"



namespace image_analysis_service
{
    


class ImageAnalysis
{
public:
    static void execute(const Image & image, const std::string & input_commands);
private:
    
    static const std::string find_region_str;
    static const std::string find_perimeter_str;
    static const std::string find_smooth_perimeter_str;
    static const std::string save_str;
    static const std::string display_str;
    
    static void doFindRegion(std::string command, const Image & image, Selection & output);
    static void doFindPerimeter(Selection & output);
    static void doFindSmoothPerimeter(Selection & output);
    static void doSave(std::string command, Selection & output);
    static void doFindPerimeter(const Image & image, Selection & output, int cmd_num);
    
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
    

    static Selection findPerimeter(const Selection & region);
    static Selection findSmoothPerimeter(const Selection & region);


};
    
}

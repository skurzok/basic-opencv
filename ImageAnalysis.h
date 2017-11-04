#include <vector>

#include "Selection.h"

namespace image_analysis_service
{
    
class ImageAnalysis
{
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

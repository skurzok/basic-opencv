
#include "ImageAnalysis.h"

namespace image_analysis_service
{

cv::Mat ImageAnalysis::findPerimeter(const cv::Mat & region)
{
    cv::Mat perimeter(region.rows, region.cols, CV_8SC1);
    perimeter.setTo(UNCHECKED);
    
    int i, j;
    for (i = 1; i < region.rows - 1; ++i) {
        for (j = 1; j < region.cols - 1; ++j) {
            
            if (region.at(i, j) == FILLED &&
                (region.at(i + 1, j) == UNCHECKED ||
                region.at(i - 1, j) == UNCHECKED ||
                region.at(i, j + 1) == UNCHECKED ||
                region.at(i, j - 1) == UNCHECKED))
            {
                perimeter.at(i, j) = FILLED;
            }
        }
    }
    
    return perimeter;
    //TODO: What if whole input is single region?
}

}

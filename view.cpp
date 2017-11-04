#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace cv;

void show_mat(const cv::Mat &image, std::string const &win_name);

class EuclideanDistance
{
public:
    explicit EuclideanDistance(float threshold):
        threshold_(threshold)
    {}
    bool operator()(const cv::Vec3b & base, const cv::Vec3b & candidate)
    {
        auto diff = base - candidate;
        auto dist = std::sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
        return dist < threshold_;
    }
private:
    float threshold_;
};

template<class T>
cv::Mat findRegion(const cv::Mat image, int x, int y, T metric)
{
    struct Cord{int x, y;};
    std::vector<Cord> to_check(1, Cord{x, y});
    
    enum {UNCHECKED, EMPTY=100, FILLED=200};
    cv::Mat status(image.rows, image.cols, CV_8SC1);
    status.setTo(UNCHECKED);  // https://stackoverflow.com/questions/4337902/how-to-fill-opencv-image-with-one-solid-color
    
    auto pattern = image.at<char>(x, y);
    
    int dbg_filed = 0;
    while (to_check.size())
    {
        auto pos = to_check.back();
        to_check.pop_back();
        
        if (status.at<char>(pos.x, pos.y) == UNCHECKED &&            
            metric(pattern, image.at<char>(pos.x, pos.y)))
        {
            status.at<char>(pos.x, pos.y) = FILLED;
            dbg_filed++;
            if (pos.x - 1 >= 0 && // pos.x - 1 < image.rows &&
                status.at<char>(pos.x - 1, pos.y) == UNCHECKED)
            {
                to_check.push_back(Cord{pos.x - 1, pos.y});
            }
            if (pos.x + 1 < image.rows &&
                status.at<char>(pos.x + 1, pos.y) == UNCHECKED)
            {
                to_check.push_back(Cord{pos.x + 1, pos.y});
            }
            if (pos.y - 1 >= 0 && // pos.x - 1 < image.rows &&
                status.at<char>(pos.x, pos.y - 1) == UNCHECKED)
            {
                to_check.push_back(Cord{pos.x, pos.y - 1});
            }
            if (pos.y + 1 < image.cols &&
                status.at<char>(pos.x, pos.y + 1) == UNCHECKED)
            {
                to_check.push_back(Cord{pos.x, pos.y + 1});
            }
            //TODO: diagonal ?
        }
    }
    std::cout << "filed " << dbg_filed << std::endl;
    return status;
}

void fast_test()
{
    Mat image;
    image = imread("ship.jpg", 1);
    
    //show_mat(image, "Input");

    image = findRegion(image, 10, 10, EuclideanDistance(15));

    show_mat(image, "Output");
}

int main(int argc, char **argv) {
    
    fast_test();
    if (argc != 3) {
        printf("usage: Opencv_Test <Image_Path> <Output_Path>\n");
        return -1;
    }

    return 0;
}


void show_mat(const cv::Mat &image, std::string const &win_name) {
    namedWindow(win_name, WINDOW_AUTOSIZE);
    imshow(win_name, image);
    waitKey(0);
}

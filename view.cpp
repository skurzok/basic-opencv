#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

void show_mat(const cv::Mat &image, std::string const &win_name);

ostream & operator<<(ostream & stream, const cv::Vec3b & v)
{
    stream << "(" << (int)v[0] << ", " << (int)v[1] << ", " << (int)v[2] << ")";
    return stream;
}


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


enum {UNCHECKED = -128, FILLED = 127};

template<class T>
cv::Mat findRegion(const cv::Mat & image, int x, int y, T metric)
{
    struct Cord{int x, y;};
    std::vector<Cord> to_check(1, Cord{x, y});
    
    cv::Mat status(image.rows, image.cols, CV_8SC1);
    status.setTo(UNCHECKED);  // https://stackoverflow.com/questions/4337902/how-to-fill-opencv-image-with-one-solid-color
    
    auto pattern = image.at<cv::Vec3b>(x, y);
    
    //int dbg_filed = 0;
    while (to_check.size())
    {
        auto pos = to_check.back();
        to_check.pop_back();
        
        if (status.at<char>(pos.x, pos.y) == UNCHECKED &&            
            metric(pattern, image.at<cv::Vec3b>(pos.x, pos.y)))
        {
            status.at<char>(pos.x, pos.y) = FILLED;
            //dbg_filed++;
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
    //std::cout << "filed " << dbg_filed << std::endl;
    return status;
}

cv::Mat findPerimeter(const cv::Mat & region)
{
    cv::Mat perimeter(region.rows, region.cols, CV_8SC1);
    perimeter.setTo(UNCHECKED);
    
    int i, j;
    for (i = 1; i < region.rows - 1; ++i) {
        for (j = 1; j < region.cols - 1; ++j) {
            
            if (region.at<char>(i, j) == FILLED &&
                (region.at<char>(i + 1, j) == UNCHECKED ||
                region.at<char>(i - 1, j) == UNCHECKED ||
                region.at<char>(i, j + 1) == UNCHECKED ||
                region.at<char>(i, j - 1) == UNCHECKED))
            {
                perimeter.at<char>(i, j) = FILLED;
            }
        }
    }
    
    return perimeter;
    //TODO: What if whole input is single region?
}

void fast_test()
{
    Mat image;
    image = imread("ship.jpg", 1);
    
    //show_mat(image, "Input");

    image = findRegion(image, 10, 10, EuclideanDistance(50));
    show_mat(image, "Output1");
    image = findPerimeter(image);

    show_mat(image, "Output2");
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

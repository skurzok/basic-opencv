#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include "ImageAnalysis.h"


using namespace cv;
using namespace std;
namespace ia = image_analysis_service;

void show_mat(const cv::Mat &image, std::string const &win_name);

ostream & operator<<(ostream & stream, const cv::Vec3b & v)
{
    stream << "(" << (int)v[0] << ", " << (int)v[1] << ", " << (int)v[2] << ")";
    return stream;
}



void fast_test()
{
    auto image = ia::Image("edges.png");
    
    //show_mat(image, "Input");

    ia::Selection sec = ia::ImageAnalysis::findRegion(image, 10, 10, EuclideanDistance(50));
    sec.Show("Output1");
    sec = ia::ImageAnalysis::findPerimeter(sec);
    sec.SaveAsText("out1.txt", ia::Selection::SELECTED_PIXELS);

    sec.Show("Output2");
    sec.SaveAsText("out2.txt", ia::Selection::ALL_PIXELS);
}

int main(int argc, char **argv) {
    
    if (argc != 3) {
        printf("usage: view input_image commands\n");
        return -1;
    }
    
    auto image = ia::Image(argv[1]);
    
    auto out = ia::ImageAnalysis::execute(image, argv[2]);
    if (out != "")
    {
        cout << "ERROR: " << out;
        return -1;
    }

    return 0;
}

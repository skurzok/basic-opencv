#include <opencv2/opencv.hpp>

namespace image_analysis_service
{

class BasicImage
{
public:
    BasicImage(int rows, int columns):
        image_(rows, columns, CV_8SC1)
    {}
    explicit BasicImage(const cv::Mat & image):
        image_(image.clone())
    {}
    
    int rows() const
    {
        return image_.rows;
    }
    int cols() const
    {
        return image_.cols;
    }

    void Show(std::string const &win_name) const
    {
        namedWindow(win_name, cv::WINDOW_AUTOSIZE);
        imshow(win_name, image_);
        cv::waitKey(0);
    }
    void Save(const std::string & path) const
    {
        cv::imwrite(path, image_);
    }
protected:
    explicit BasicImage()
    {}
    
    cv::Mat image_;
};

class Selection: public BasicImage
{
public:
    enum {UNCHECKED = -128, FILLED = 127};
    enum SaveAsTextMethod {ALL_PIXELS, SELECTED_PIXELS};
    Selection(){}
    Selection(int rows, int columns):
        BasicImage(rows, columns)
    {
        image_.setTo(UNCHECKED);   // https://stackoverflow.com/questions/4337902/how-to-fill-opencv-image-with-one-solid-color
        
    }
    char & at(int x, int y)
    {
        return image_.at<char>(x, y);
    }
    const char & at(int x, int y) const
    {
        return image_.at<char>(x, y);
    }
    
    void SaveAsText(const std::string & path, SaveAsTextMethod method) const;
private:
    void SaveAllPixels(std::ostream & stream) const;
    void SaveSelected(std::ostream & stream) const;
};

class Image: public BasicImage
{
public:
    explicit Image(const std::string & path)
    {
        image_ = cv::imread(path, 1);
    }
    
    const cv::Vec3b & at(int x, int y) const
    {
        return image_.at<cv::Vec3b>(x, y);
    }
};

}

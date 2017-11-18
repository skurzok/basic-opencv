#include "Selection.h"


namespace image_analysis_service
{

class LinearSmoother
{
public:
    explicit LinearSmoother(int minPathSize = 16, int step = 5);

    Selection Apply(const Selection & perimeter);
private:
    
    struct Point{
        int x, y;
        bool operator==(const Point & rhs)
        {
            return x == rhs.x && y == rhs.y;
        }
        bool operator!=(const Point & rhs)
        {
            return !(*this == rhs);
        }
    };


    bool select(std::vector<Point> & points, int i, int j, cv::Mat & paths, int path_id);

    std::vector<Point> makeSequence(cv::Mat & paths, int path_id, const Point & start);

    int mark(int i, int j, const Selection & perimeter, cv::Mat & paths, int path_id);
    void fillLinear(const Point & a, const Point & b, Selection & selection);

    void addSmoothPath(Selection & selection, const std::vector<Point> & path, int step = 5);
        
    int minPathSize_;
    int step_;
};
}

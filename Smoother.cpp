#include "Smoother.h"

namespace image_analysis_service
{

LinearSmoother::LinearSmoother(int minPathSize, int step): 
    minPathSize_(minPathSize),
    step_(step)
{}
    
/// Interface to smoother, accepts perimeter, and return smoothed perimter
/// First, method searhch for sets of connected points of current perimeter,
/// Then if set is large enouth (small sets are considered artifacts) create sequence of adjacent points
/// finaly draw line between every `step` points
Selection LinearSmoother::Apply(const Selection & perimeter)
{
    cv::Mat paths(perimeter.rows(), perimeter.cols(), CV_32S);
    std::vector<int> sizes(1, -1);
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
    
    Selection smooth(perimeter.rows(), perimeter.cols());
    //int biggest_path_id = std::distance(sizes.begin(), std::max_element(sizes.begin(), sizes.end()));
    
    for (int i = 1; i < points.size(); i++)
    {
        if (sizes[i] < minPathSize_)
        {
            continue;
        }
        auto sequence = makeSequence(paths, i, points[i]);
        
        if (sequence.size())
        {
            addSmoothPath(smooth, sequence);
        }
    }
    return smooth;
}

/// method adds given point to sequence of points and recursively check 
// if it is possible to close loop of points drawing path in directon of adjacent point
// if some direction does not give a result, point in that direction is removed from sequence and other direction is checked
bool LinearSmoother::select(std::vector<LinearSmoother::Point> & points, int i, int j, cv::Mat & paths, int path_id)
{
    points.push_back(Point{i, j});
    paths.at<int>(i, j) = 0;
    
    
    auto check = [&points, &paths, path_id, this](int x, int y) -> bool
    {
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

/// Method for finding closed loop of points for given connected set of points
/// perimeter may consist of larger group of adjacent points, so finding right sequence may be not trival
std::vector<LinearSmoother::Point> LinearSmoother::makeSequence(cv::Mat & paths, int path_id, const Point & start)
{

    //debug_view = Selection(paths.rows, paths.cols);
    //std::cout << "start is at" << start.x << "," << start.y << std::endl;
    std::vector<Point> points;
    int td = select(points, start.x, start.y, paths, path_id);
    return points;
}

/// method used for finding sets of adjacent points in perimeter
/// assign given points to given set (by id) and recursively marks all adjacent points if they are in perimeter
int LinearSmoother::mark(int i, int j, const Selection & perimeter, cv::Mat & paths, int path_id)
{
    int total = 1;
    paths.at<int>(i, j) = path_id;
    
    auto check = [&total, &paths, &perimeter, path_id, this](int x, int y)
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

/// Method for drawing line between two given points
void LinearSmoother::fillLinear(const Point & a, const Point & b, Selection & selection)
{
    Point d{b.x - a.x, b.y - a.y};
    char method = 'n';
    
    if (d.x == 0 && d.y != 0)
    {
        method = 'y';
    }
    else if (d.y == 0 && d.x != 0)
    {
        method = 'x';
    }
    else
    {
        if (std::abs(d.y) > std::abs(d.x))
        {
            method = 'y';
        }
        else
        {
            method = 'x';
        }
    }
    auto ca = a;
    auto cb = b;
    
    selection.at(ca.x, ca.y) = Selection::FILLED;
    selection.at(cb.x, cb.y) = Selection::FILLED;
    if (method == 'x')
    {
        if (d.x < 0)
        {
            std::swap(ca, cb);
        }
        for (int x = ca.x; x <= cb.x; ++x)
        {
            int y = ca.y + d.y * (x - ca.x) / float(d.x);
            selection.at(x, y) = Selection::FILLED;
        }
    }
    else if (method == 'y')
    {
        if (d.y < 0)
        {
            std::swap(ca, cb);
        }
        for (int y = ca.y; y <= cb.y; ++y)
        {
            int x = ca.x + d.x * (y - ca.y) / d.y;
            selection.at(x, y) = Selection::FILLED;
        }
    }
}

/// Method for drawing smooth perimeter.
/// Draw line between every {step} points
void LinearSmoother::addSmoothPath(Selection & selection, const std::vector<Point> & path, int step)
{
    for (int i = 0; i < path.size(); i+= step)
    {
        if (i + step < path.size())
        {
            fillLinear(path[i], path[i + step], selection);
        }
        else
        {
            fillLinear(path[i], path[0], selection);
        }
    }
}
}

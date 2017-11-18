


## Requirements
- OpenCV 2.X
- CMake > 2.8

## Usage
    cmake .
    make
    ./view input_image commands

    ex: ./view edges.png "display|find_region(10,10)|display|find_perimeter|display|save(out.txt, all)|save(image_out.png)"
    
`commands` are string containging commands separated by '|'.
Allowed commands are 'find_region', 'find_perimeter', 'save', 'display'. 
`find_region1 - syntax: `find_region(x,y[,distance])`. Finds region starting in given x,y with pixels that maximum distance in euclidean space is equal to 'distance', default 'distance is 30. 

`find_perimeter` - extrakts perimeter of previously found region

`save` - syntax: `save(file_name[,all | selected)`, saves image or selection to file, selection will be saved as text file if second argument is provided"

`display` - displays current image or selection

## References
- http://answers.opencv.org/question/68718/distance-between-images-pixel-by-pixel/
- https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string
- https://stackoverflow.com/questions/5891610/how-to-remove-certain-characters-from-a-string-in-c

## Code documentation

### class `Image`
 - represents input image
 - allows to load, save and display image
 
### class `Selection`
 - represents region or perimeter
 - allows to load, save and display image

Classes `Image` and `Selection` shares some methods and internal representation, but are different types. It prevents some incorrect usage of objects of this classes.

### class `ImageAnalysis`
- `findRegion(const Image & image, int x, int y, T metric)` static method themplate, returns `Selection` with region starting in given x,y with pixels that maximum distance in euclidean space is equal to 'distance'.
- `findPerimeter(const Selection & region)` static method, returns `Selection` with perimter of given region


### class `LinearSmoother`
- `LinearSmoother(int minPathSize = 16, int step = 5)` `ctor` Constructor, sets minPathSize and step variables. minPathSize is a minimal size of perimeter that is not considered as artifact. Step is a smoothing factor, described below.
- `Selection Apply(const Selection & perimeter)` Interface to smoother, accepts perimeter, and return smoothed perimter.  First, method searhch for sets of connected points of current perimeter, then if set is large enouth (small sets are considered artifacts) create sequence of adjacent points. Finaly draw line between every `step` points
- `int mark(int i, int j, const Selection & perimeter, cv::Mat & paths, int path_id)` Method used for finding sets of adjacent points in perimeter. Assigns given points to given set (by id) and recursively marks all adjacent points if they are in perimeter
- `std::vector<Point> makeSequence(cv::Mat & paths, int path_id, const Point & start)` Method for finding closed loop of points for given connected set of points. Perimeter may consist of larger group of adjacent points, so finding right sequence may be not trival.
- `bool select(std::vector<Point> & points, int i, int j, cv::Mat & paths, int path_id)` Method adds given point to sequence of points and recursively check if it is possible to close loop of points drawing path in directon of adjacent point.
If some direction does not give a result, point in that direction is removed from sequence and other direction is checked.
- `void addSmoothPath(Selection & selection, const std::vector<Point> & path, int step)`  Draw line between every {step} points 
- `void fillLinear(const Point & a, const Point & b, Selection & selection)` Method for drawing line between two given points

### Possible improvements
- Linear interpolation is simplest possible algorithm for this task so it definitelty needs to be changed. Current data represetation (vector of points in perimeter) alows easly to use Catmull–Rom spline interpolation.
- Detecting of 'sharp edges' and use them as point in interpolation could result in better fit to original perimeter. Such detection could by done by finding spikes of derivative of points difference (absolute value (length) of second derivative of positon with respect to point index).
- finding sets of adjacent points in perimeter could be optimized. Recursion may be removed by using queue of points to check. Large numer points may result in stack overflow in current implementation
- sequence of points could be found using ex. Dijkstra algorithm. Current implementation may roll back almost to the beginning in pessimistic cases, and then repeat most of calculations. Dijkstra or A* would reduce pessimistic complexity.
- calculation of EuclideanDistance could be optimized by removing square root calculation. We can compare to sum of squares to square of threshold distance.


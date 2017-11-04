


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

### class Image
 - represents input image
 - allows to load, save and display image
 
### class Selection
 - represents region or perimeter
 - allows to load, save and display image

Classes `Image` and `Selection` shares some methods and internal representation, but are different types. It prevents some incorrect usage of objects of this classes.

### class `ImageAnalysis`
- `findRegion(const Image & image, int x, int y, T metric)` static method themplate, returns `Selection` with region starting in given x,y with pixels that maximum distance in euclidean space is equal to 'distance'.
- `findPerimeter(const Selection & region)` static method, returns `Selection` with perimter of given region

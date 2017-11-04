


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

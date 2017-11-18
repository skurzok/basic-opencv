#!/bin/bash

/view test/test3.png "find_region(100,100)|find_smooth|display|save(image_out.png)"

./view edges.png "display"

./view edges.png "find_region(10,10)|find_perimeter|display"

./view edges.png "find_region(10,10)|display|find_perimeter|display|save(out.txt, all)|save(image_out.png)"

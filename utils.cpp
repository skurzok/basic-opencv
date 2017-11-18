#include "utils.h"
#include <sstream>
#include <cstring>
#include <algorithm>

namespace image_analysis_service
{

    
template<typename Out>
void split(const std::string &s, char delim, Out result) { // https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}
    
std::vector<std::string> split(const std::string &s, char delim) { 
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

void removeCharsFromString( std::string &str, const char* charsToRemove ) {  // https://stackoverflow.com/questions/5891610/how-to-remove-certain-characters-from-a-string-in-c
   for ( unsigned int i = 0; i < std::strlen(charsToRemove); ++i ) { 
      str.erase( std::remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
   }
}
}

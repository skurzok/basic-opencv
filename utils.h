#include <vector>
#include <string>


namespace image_analysis_service
{
std::vector<std::string> split(const std::string &s, char delim);

void removeCharsFromString( std::string &str, const char* charsToRemove );

}

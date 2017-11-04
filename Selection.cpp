#include "Selection.h"

#include <fstream>

using namespace std;

namespace image_analysis_service
{

void Selection::SaveAsText(const std::string & path, SaveAsTextMethod method) const
{
    ofstream file(path, std::ofstream::out); //TODO: file open checks
    file << "rows: " << image_.rows << endl;
    file << "cols: " << image_.cols << endl;
    if (method == ALL_PIXELS)
    {
        SaveAllPixels(file);
    }
    else
    {
        SaveSelected(file);
    }
}

void Selection::SaveAllPixels(std::ostream & stream) const
{

    for (int i = 1; i < rows(); ++i)
    {
        for (int j = 1; j < cols(); ++j)
        {
            stream << ((at(i, j) == Selection::FILLED) ? 'X' : ' ');
        }
        stream << endl;
    }
}


void Selection::SaveSelected(std::ostream & stream) const
{

    for (int i = 1; i < rows(); ++i)
    {
        for (int j = 1; j < cols(); ++j)
        {
            if (at(i, j) == Selection::FILLED)
            {
                stream << i << ", " << j << endl;
            }
        }
    }
}


}

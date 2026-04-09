#include "../Headers/line_sort.h"
using namespace std;

string unlabeledReorder(const map<int, string>& fragments)
{
    string text;
    for (const auto& [lineNumber, line] : fragments)                //NOLINT
    {
        // If there is any text, add it and a newline to the output
        if (!line.empty())
        {
            const size_t start = line.find_first_not_of(' ');
            if (const size_t end = line.find_last_not_of(" \t\n\r\f\v"); start != string::npos && end != string::npos)
            {
                text += line.substr(start, end - start + 1) + "\n";
            }
        }
    }

    return text;
}

string labeledReorder(const map<int, string>& fragments)
{
    string text;
    for (const auto& [lineNumber, line] : fragments)
    {
        text += to_string(lineNumber) + " ";
        const size_t start = line.find_first_not_of(' ');
        if (const size_t end = line.find_last_not_of(" \t\n\r\f\v"); start != string::npos && end != string::npos)
        {
            text += line.substr(start, end - start + 1);
        }
        text += "\n";
    }

    return text;
}
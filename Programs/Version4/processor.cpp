// Program for combining the contents of multiple files into a single string.
// To be run as a child process.

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include "Headers/file_ops.h"
#include "Headers/line_sort.h"
using namespace std;

/// Combines the contents of multiple files by reading the contents and sorting by line numbers.
/// Writes its results to a scrap file.
///
/// @param processIndex The index of the process.
/// @param dataPath The path to the folder containing the disordered files.
void processor(int processIndex, const string& dataPath);


int main(const int argc, char* argv[])
{
    // Arguments:
    const int procIdx = stoi(argv[1]);  // The index of the process
    const string dataFolderPath = argv[2];  // The path to the data folder

    // Process the files
    processor(procIdx, dataFolderPath);

    return 0;
}


void processor(int processIndex, const string& dataPath)
{
    // Get the list of files in the data folder
    vector<string> files = getFilesIn(dataPath);

    // Open the distribution scrap file for this process
    string scrapName = "./Scrap/scrap_d" + to_string(processIndex) + ".txt";
    ifstream scrapFile(scrapName);
    // Collect indices from the scrap file
    string idxString; int idx;
    vector<int> fileIndices;
    while(getline(scrapFile, idxString, ' '))
    {
        try
        {
            idx = stoi(idxString);
            fileIndices.push_back(idx);
        } catch (...) { }
    }
    // Close and remove the scrap file
    scrapFile.close();
    remove(scrapName.c_str());

    map<int, string> lines;
    // Read each file's order number and contents
    for (int fileIndex : fileIndices)
    {
        string filePath = dataPath + "/" + files[fileIndex];
        ifstream file(filePath);

        string line;
        getline(file, line, ' ');
        while (getline(file, line))
        {
            istringstream iss(line);
            if (int lineNumber; iss >> lineNumber)
            {
                string text;
                getline(iss, text);
                lines[lineNumber] = text;
            }
        }
        file.close();
    }

    string pScrapPath = "./Scrap/scrap_p" + to_string(processIndex) + ".txt";
    ofstream pScrapFile(pScrapPath, ofstream::trunc);
    pScrapFile << labeledReorder(lines) << flush;

    pScrapFile.close();
}

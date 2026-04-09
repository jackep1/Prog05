// Version 1 of Prog05

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <filesystem>
#include "Headers/file_ops.h"
#include "Headers/line_sort.h"
using namespace std;

/// Distributes files between processes by reading the first line of each file.
/// Each file is assigned by writing its index to a scrap file for the appropriate process.
///
/// @param numProcesses The number of processes to distribute the files between.
/// @param start The index of the first file to process.
/// @param end The index of the last file to process.
/// @param dataPath The path to the data folder.
/// @param files The list of files to process.
/// @return A vector of vectors containing the indices of the files to process for each process, indexed by process.
vector<vector<int>> distributor(int numProcesses, int start, int end, const string& dataPath, const vector<string>& files);

/// Combines the contents of multiple files by reading the contents and sorting by line numbers.
/// Writes its results to a scrap file.
///
/// @param fileIndices The indices of the files to process.
/// @param dataPath The path to the data folder.
/// @param files The list of files to process.
/// @return A string containing the concatenated contents of the files.
string processor(const vector<int>& fileIndices, const string& dataPath, const vector<string>& files);


int main(const int argc, char* argv[])
{
    if (argc != 4)
    {
        cout << "Usage: " << argv[0] << " <number of processes> <path to data folder> <path to output folder>" << endl;
        return 1;
    }

    // Arguments:
    int numProcesses = stoi(argv[1]);  // The number n of child processes to create
    string dataFolderPath = argv[2];  // The path to the data directory
    string outputPath = argv[3];  // The path to the output file

    // Build the list of files present in the data folder
    vector<string> filesToProcess = getFilesIn(dataFolderPath);
    vector<vector<int>> processFileIndices(numProcesses);

    // Distribute the files between the processes
    int files_per_process = filesToProcess.size() / numProcesses;           //NOLINT
    int remaining_files = filesToProcess.size() % numProcesses;             //NOLINT
    for (int idx = 0; idx < numProcesses; idx++)
    {
        int start, end;
        if (idx < remaining_files)  // If there are leftover files, assign one extra to this process
        {
            start = idx * (files_per_process + 1);
            end = (idx + 1) * (files_per_process + 1) - 1;
        } else  // If no leftover files, assign the same number of files to each process
        {
            start = idx * files_per_process + remaining_files;
            end = (idx + 1) * files_per_process + remaining_files - 1;
        }
        // I get back from the distributor an array of n lists of file indices indicating
        // how the files should get distributed between the n processors.
        vector<vector<int>> partialList = distributor(numProcesses, start, end, dataFolderPath, filesToProcess);
        for (int procIdx = 0; procIdx < numProcesses; procIdx++)
        {
            processFileIndices[procIdx].insert(processFileIndices[procIdx].end(), partialList[procIdx].begin(), partialList[procIdx].end());
        }
    }

    ofstream outputFile(outputPath);
    // Run each pseudo-process
    for (int procIdx = 0; procIdx < numProcesses; procIdx++)
    {
        // Get the processed fragment
        string fragment = processor(processFileIndices[procIdx], dataFolderPath, filesToProcess);
        // Write that part to the reconstructed source file
        outputFile << fragment;
    }
    outputFile.close();

    return 0;
}



vector<vector<int>> distributor(int numProcesses, int start, int end, const string& dataPath, const vector<string>& files)
{
    vector<vector<int>> partialProcessFileIndices(numProcesses);
    for (int i = start; i <= end; i++)
    {
        // Read the file and determine which process should handle it
        string filePath = dataPath + "/" + files[i];
        // Open the file and read the first value
        ifstream file(filePath);
        string line;
        getline(file, line, ' ');
        file.close();
        // Determine the process that should handle this file
        int process = stoi(line);
        partialProcessFileIndices[process].push_back(i);
    }

    return partialProcessFileIndices;
}

string processor(const vector<int>& fileIndices, const string& dataPath, const vector<string>& files)
{
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

    return unlabeledReorder(lines);
}

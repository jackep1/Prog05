// Program for distributing files between processes.
// To be run as a child process.

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "Headers/file_ops.h"
using namespace std;

/// Distributes files between processes by reading the first line of each file.
/// Each file is assigned by writing its index to a scrap file for the appropriate process.
///
/// @param numProcesses The number of processes to distribute the files between.
/// @param start The index of the first file to process.
/// @param end The index of the last file to process.
/// @param dataPath The path to the data folder.
/// @param files The list of files to process.
/// @param processScrapFiles A vector with one file per child process for recording the files to process.
/// @return A vector of vectors containing the indices of the files to process for each process, indexed by process.
void distributor(int numProcesses, int start, int end, const string& dataPath, const vector<string>& files, vector<ofstream>& processScrapFiles);

int main(const int argc, char* argv[])
{
    // Arguments:
    const int numProcesses = stoi(argv[1]);  // The number n of child processes to create
    const int start = stoi(argv[2]);  // The index of the first file to process
    const int end = stoi(argv[3]);  // The index of the last file to process
    const string dataFolderPath = argv[4];  // The path to the data folder
    vector<ofstream> processScrapFiles(numProcesses);  // The files where each process will write the file indices it should process
    for (int i = 5; i < argc; i++)
    {
        processScrapFiles[i - 5].open(argv[i], ios_base::app);
    }

    // Distribute the files between the processes
    distributor(numProcesses, start, end, dataFolderPath, getFilesIn(dataFolderPath), processScrapFiles);
    for (int i = 0; i < numProcesses; i++)
    {
        processScrapFiles[i].close();  // Close the scrap files
    }

    return 0;
}


void distributor(int numProcesses, int start, int end, const string& dataPath, const vector<string>& files, vector<ofstream>& processScrapFiles)
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
        // Write this file to the appropriate scrap file
        processScrapFiles[process] << i << " " << endl;
    }
}

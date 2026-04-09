// Program for distributing files between processes.
// To be run as a child process.

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <dirent.h>
using namespace std;

/// Function for getting all file names in a directory.
/// Uses the sample code from the course notes.
///
/// @param rootDirectory The directory to get the files from.
///
/// @return A vector of strings containing the file names.
vector<string> getFilesIn(const string& rootDirectory)
{
    DIR* directory = opendir(rootDirectory.c_str());
    if (directory == nullptr)
    {
        cout << "data folder " << rootDirectory << " not found" << endl;
        exit(1);
    }

    dirent* entry;
    vector<string> fileNames;
    while ((entry = readdir(directory)) != nullptr)
    {
        const char* name = entry->d_name;
        if ((name[0] != '.') && (entry->d_type == DT_REG))
        {
            fileNames.emplace_back(entry->d_name);
        }
    }
    closedir(directory);

    return fileNames;
}

/// Function for distributing files between processes.
/// This function reads the first line of each file to determine which process should handle it.
///
/// @param numProcesses The number of processes to distribute the files between.
/// @param start The index of the first file to process.
/// @param end The index of the last file to process.
/// @param dataPath The path to the data folder.
/// @param files The list of files to process.
/// @param processScrapFiles A vector with one file per child process for recording the files to process.
///
/// @return A vector of vectors containing the indices of the files to process for each process, indexed by process.
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
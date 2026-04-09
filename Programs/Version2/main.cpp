// Version 2 of Prog05

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <map>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>
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
/// @param processScrapFiles A vector with one file per child process for recording the files to process.
/// @return A vector of vectors containing the indices of the files to process for each process, indexed by process.
void distributor(int numProcesses, int start, int end, const string& dataPath, const vector<string>& files, vector<ofstream>& processScrapFiles);

/// Combines the contents of multiple files by reading the contents and sorting by line numbers.
/// Writes its results to a scrap file.
///
/// @param processIndex The index of the process.
/// @param dataPath The path to the data folder.
/// @param files The list of files to process.
///
/// @return A string containing the concatenated contents of the files.
string processor(int processIndex, const string& dataPath, const vector<string>& files);


int main(const int argc, char* argv[])
{
    // Ensure proper usage
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

    // Create scrap files for each process
    vector<ofstream> processScrapFiles(numProcesses);
    for (int i = 0; i < numProcesses; i++)
    {
        processScrapFiles[i].open("scrap" + to_string(i) + ".txt");
    }

    // Distribute the files between the processes
    int files_per_process = filesToProcess.size() / numProcesses;           //NOLINT
    int remaining_files = filesToProcess.size() % numProcesses;             //NOLINT
    for (int idx = 0; idx < numProcesses; idx++)
    {
        int start, end;
        if (idx < remaining_files)
        {
            start = idx * (files_per_process + 1);
            end = (idx + 1) * (files_per_process + 1) - 1;
        } else
        {
            start = idx * files_per_process + remaining_files;
            end = (idx + 1) * files_per_process + remaining_files - 1;
        }

        pid_t p = fork();  // Fork and check for success
        if (p < 0) { cout << "Fork failed!" << endl << flush; return 1; }

        if (p == 0)
        {  //* CHILD PROCESS *//
            // Call func for building n lists of files, one for each future process
            distributor(numProcesses, start, end, dataFolderPath, filesToProcess, processScrapFiles);
            return 0;
        }  //* END CHILD PROCESS *//

        int status;
        while (waitpid(-1, &status, 0) > 0)
        { /* wait for child to finish */ }
    }

    // Run each process
    for (int procIdx = 0; procIdx < numProcesses; procIdx++)
    {
        // Create a new process
        pid_t p = fork();
        if (p < 0) { cout << "Fork failed!" << endl << flush; return 1; }

        if (p == 0)
        { //* CHILD PROCESS *//
            // Get the processed fragment
            string fragment = processor(procIdx, dataFolderPath, filesToProcess);

            // Write that part to this process's scrap file
            string pScrapPath = "scrap" + to_string(procIdx) + ".txt";
            ofstream pScrapFile(pScrapPath, ofstream::trunc);
            pScrapFile << fragment << flush;
            exit(0);
        } //* END CHILD PROCESS *//

        int status;
        while (waitpid(-1, &status, 0) > 0)
        { /* wait for child to finish */ }
    }

    map<int, string> lines;
    for (int fileIdx = 0; fileIdx < numProcesses; fileIdx++)
    {
        ifstream pScrapFile("scrap" + to_string(fileIdx) + ".txt");
        string line;
        while(getline(pScrapFile, line))
        {
            istringstream iss(line);
            if (int lineNumber; iss >> lineNumber)
            {
                string text;
                getline(iss, text);
                lines[lineNumber] = text;
            }
        }
        // Close and remove the file
        pScrapFile.close();
        remove(("scrap" + to_string(fileIdx) + ".txt").c_str());
    }
    string finalText = unlabeledReorder(lines);
    ofstream outputFile(outputPath);
    outputFile << finalText << flush;
    outputFile.close();

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

string processor(int processIndex, const string& dataPath, const vector<string>& files)
{
    ifstream scrapFile("scrap" + to_string(processIndex) + ".txt");
    // Collect indices from the scrap file
    string idxString; int idx;
    vector<int> fileIndices;
    while(getline(scrapFile, idxString, ' '))
    {
        try
        {
            idx = stoi(idxString);
            fileIndices.push_back(idx);
        } catch (...)
        {
            cout << idxString << endl << flush;
        }
    }

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

    return labeledReorder(lines);
}

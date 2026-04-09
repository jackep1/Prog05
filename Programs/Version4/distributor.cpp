// Program for distributing files between processes.
// To be run as a child process.

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
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
void distributor(int numProcesses, int start, int end, const string& dataPath, const vector<string>& files, vector<ofstream>& processScrapFiles);


int main(const int argc, char* argv[])
{
    // Arguments:
    const int procIdx = stoi(argv[1]);  // The index of the process
    const int numProcesses = stoi(argv[2]);  // The number n of child processes to create
    const int start = stoi(argv[3]);  // The index of the first file to process
    const int end = stoi(argv[4]);  // The index of the last file to process
    const string dataFolderPath = argv[5];  // The path to the data folder
    const int pipeFd = stoi(argv[6]);  // The file descriptor for the pipe

    // Create scrap files for each process
    vector<ofstream> processScrapFiles(numProcesses);
    for (int i = 0; i < numProcesses; i++)
    {
        processScrapFiles[i].open("./Scrap/scrap_d" + to_string(i) + ".txt", ios_base::app);
    }

    // Distribute the files between the processes
    distributor(numProcesses, start, end, dataFolderPath, getFilesIn(dataFolderPath), processScrapFiles);

    // Release the lock file
    releaseLock(procIdx);

    // Close all scrap files
    for (int i = 0; i < numProcesses; i++)
    {
        processScrapFiles[i].close();  // Close the scrap files
    }

    // Wait for all distributor processes to finish
    while (distLocked()) { usleep(1000); }

    const pid_t p = fork();
    if (p < 0) { perror("Fork failed"); return 1; }

    if (p == 0)
    {  //* CHILD PROCESS *//
        vector args = {
            "Pv4",  // The name of the program
            to_string(procIdx).c_str(),  // The index of the process
            dataFolderPath.c_str(),  // The path to the data folder
        };

        for (const vector<string> filesToProcess = getFilesIn(dataFolderPath); const auto& file : filesToProcess)
        {
            args.emplace_back(file.c_str());  // Add each file name
        } args.emplace_back(nullptr);

        // Execute the data processor
        execv("./Pv4", const_cast<char* const*>(args.data()));
        perror("exec failed");
        return 2;
    }  //* END CHILD PROCESS *//

    int status;
    while (waitpid(-1, &status, 0) > 0)
    { /* wait for child to finish */ }

    string pScrapPath = "./Scrap/scrap_p" + to_string(procIdx) + ".txt";
    ifstream pScrapFile(pScrapPath);
    if (!pScrapFile.is_open()) { perror("Scrap file not found"); return 3; }

    vector<string> lines;
    string line;
    while(getline(pScrapFile, line))
    {
        write(pipeFd, line.c_str(), line.size());
        write(pipeFd, "\n", 1);
        cout << line << endl;
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
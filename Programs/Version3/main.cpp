// Version 3 of Prog05

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

int main(const int argc, char* argv[])
{
    // Ensure proper usage
    if (argc != 4)
    {
        cout << "Usage: " << argv[0] << " <number of processes> <path to data folder> <path to output folder>" << endl;
        return 11;
    }

    // Arguments:
    int numProcesses = stoi(argv[1]);  // The number n of child processes to create
    string dataFolderPath = argv[2];  // The path to the data directory
    string outputPath = argv[3];  // The path to the output file

    // Build the list of files present in the data folder
    vector<string> filesToProcess = getFilesIn(dataFolderPath);
    vector<vector<int>> processFileIndices(numProcesses);

    // Create scrap files for each distributor process
    vector<string> processScrapFilePaths(numProcesses);
    for (int i = 0; i < numProcesses; i++)
    {
        processScrapFilePaths[i] = "scrap_d" + to_string(i) + ".txt";
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
            vector args = { // Assemble the arguments for the distributor process
                "Dv3",  // The name of the program
                to_string(numProcesses).c_str(),  // The number of processes
                to_string(start).c_str(),  // The index of the first file to process
                to_string(end).c_str(),  // The index of the last file to process
                dataFolderPath.c_str(),  // The path to the data folder
            };

            for (const auto& path : processScrapFilePaths)
            {
                args.emplace_back(path.c_str());  // Add each scrap file path
            } args.emplace_back(nullptr);

            // Execute the distributor process
            execv("./Dv3", const_cast<char* const*>(args.data()));
            perror("exec failed");
            return 2;
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
            vector args = {
                "Pv3",  // The name of the program
                to_string(procIdx).c_str(),  // The index of the process
                dataFolderPath.c_str(),  // The path to the data folder
            };

            for (const auto& file : filesToProcess)
            {
                args.emplace_back(file.c_str());  // Add each file name
            } args.emplace_back(nullptr);

            // Execute the data processor
            execv("./Pv3", const_cast<char* const*>(args.data()));
            perror("exec failed");
            return 2;
        } //* END CHILD PROCESS *//

        int status;
        while (waitpid(-1, &status, 0) > 0)
        { /* wait for child to finish */ }
    }

    map<int, string> lines;
    for (int fileIdx = 0; fileIdx < numProcesses; fileIdx++)
    {
        ifstream pScrapFile("scrap_p" + to_string(fileIdx) + ".txt"); if (!pScrapFile.is_open()) { cout << "Failed to open scrap file" << endl; return 3; }
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
        // Close and remove the scrap file
        pScrapFile.close();
        remove(("scrap_p" + to_string(fileIdx) + ".txt").c_str());
    }
    string finalText = unlabeledReorder(lines);
    ofstream outputFile(outputPath);
    outputFile << finalText << flush;
    outputFile.close();

    return 0;
}
// Version 4 of Prog05

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <fcntl.h>
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
        perror(("Usage: " + to_string(*argv[0]) + " <number of processes> <path to data folder> <path to output folder>").c_str());
        return 11;
    }

    // Arguments:
    int numProcesses = stoi(argv[1]);  // The number n of child processes to create
    string dataFolderPath = argv[2];  // The path to the data directory
    string outputPath = argv[3];  // The path to the output file

    // Build the list of files present in the data folder
    vector<string> filesToProcess = getFilesIn(dataFolderPath);
    vector<vector<int>> processFileIndices(numProcesses);

    // Create a folder for all scrap files
    filesystem::create_directory("Scrap");

    // Create pipes for communication between the parent and child processes
    int pipes[numProcesses][2];
    for (int i = 0; i < numProcesses; i++)
    {
        if (pipe(pipes[i]) < 0) { perror("pipe"); return 5; }
    }

    // Distribute the files between the processes
    int filesPerProcess = filesToProcess.size() / numProcesses;           //NOLINT
    int remainingFiles = filesToProcess.size() % numProcesses;             //NOLINT
    for (int idx = 0; idx < numProcesses; idx++)
    {
        int start, end;
        if (idx < remainingFiles)
        {
            start = idx * (filesPerProcess + 1);
            end = (idx + 1) * (filesPerProcess + 1) - 1;
        } else
        {
            start = idx * filesPerProcess + remainingFiles;
            end = (idx + 1) * filesPerProcess + remainingFiles - 1;
        }

        pid_t p = fork();  // Fork and check for success
        if (p < 0) { perror("Fork failed"); return 1; }

        if (p == 0)
        {  //* CHILD PROCESS *//
            close(pipes[idx][0]);  // Close the read end of the pipe
            createLock(idx);  // Set lock for distributor to release when it's done

            vector <const char*> args = { // Assemble the arguments for the process
                "Dv4",  // The name of the program
                to_string(idx).c_str(),  // The index of the process
                to_string(numProcesses).c_str(),  // The number of processes
                to_string(start).c_str(),  // The index of the first file to process
                to_string(end).c_str(),  // The index of the last file to process
                dataFolderPath.c_str(),  // The path to the data folder
                to_string(pipes[idx][1]).c_str(),  // The file descriptor for the pipe
                nullptr
            };

            // Execute the distributor process
            execv("./Dv4", const_cast<char* const*>(args.data()));
            perror("exec failed");
            return 2;
        }  //* END CHILD PROCESS *//

        close(pipes[idx][1]);  // Close the write end of the pipe

        int status;
        while (waitpid(-1, &status, 0) > 0)
        { /* wait for child to finish */ }
    }

    for (int i = 0; i < numProcesses; i++)
    {
        int flags = fcntl(pipes[i][0], F_GETFL, 0);
        fcntl(pipes[i][0], F_SETFL, flags | O_NONBLOCK);
    }

    map<int, string> lines;
    for (int pipeIdx = 0; pipeIdx < numProcesses; pipeIdx++)
    {
        char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(pipes[pipeIdx][0], buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[bytesRead] = '\0';
            istringstream iss(buffer);
            string line;
            while (getline(iss, line))
            {
                istringstream lineStream(line);
                if (int lineNumber; iss >> lineNumber)
                {
                    string text;
                    getline(iss, text);
                    lines[lineNumber] = text;
                }
            }
        }
        close(pipes[pipeIdx][0]);
    }

    while(wait(nullptr) > 0)
    { /* wait for all children to finish */ }
    // map<int, string> lines;
    // for (int fileIdx = 0; fileIdx < numProcesses; fileIdx++)
    // {
    //     ifstream pScrapFile("./Scrap/scrap_p" + to_string(fileIdx) + ".txt"); if (!pScrapFile.is_open()) { perror("Scrap fail not found"); return 3; }
    //     string line;
    //     while(getline(pScrapFile, line))
    //     {
    //         istringstream iss(line);
    //         if (int lineNumber; iss >> lineNumber)
    //         {
    //             string text;
    //             getline(iss, text);
    //             lines[lineNumber] = text;
    //         }
    //     }
    //     // Close and remove the scrap file
    //     pScrapFile.close();
    // }
    // Remove the scrap directory
    filesystem::remove_all("Scrap");

    string finalText = unlabeledReorder(lines);  // Order the lines and remove the line numbers
    ofstream outputFile(outputPath);
    outputFile << finalText << flush;  // Write the final text to the output file
    outputFile.close();

    return 0;
}
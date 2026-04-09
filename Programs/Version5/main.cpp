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
using namespace std;

// These are the child processes created by the server process. Each distributor gets a set of data
// files (defined by a range in the catalog list) and gets on to determine which process this data
// file was destined for. Figure 2 summarizes the situation from the point of view of distributor
// process d0. On the right side of the figure, we see the catalog list, with an index assigned to
// each file. We also get a peek into the content of each data file so that which process this file
// should be processed by. Each distributor process gets a range of file indices to sort and distribute.
// For example, we can see that Distributor d0 will be sorting the files with indices 0 to 5. Among
// those, it is only going to “keep” the second file(index 1), all the others being assigned to another
// process(e.g.the file with index 0 should be processed by Distributor d2.) The dashed arrows in the
// lower half of the figure summarize what should be accomplished from the point of view of d0. It
// must inform If the data file is one of those it was supposed to process, it adds the file’s name/id
// to its "to do list". If the file was destined for another process, it sends a message to the server
// with the index of the distributor process to which the message should be forwarded and the index of
// the data file that the destination process should add to its own "to do list". The plain arrows in the
// top part of the figure show how this “passing around” of information is implemented. It would be
// absurd(and very complex) to establish 2-way communication between all pairs of processes (that would
// be n(n −1) pipes for n processes), so we use a client-server architecture: All communications go
// through the server. In Figure 2, we see that the first data file that Distributor 0 looks at should
// be processed by Process 2. Therefore, it sends to the server the message 2 0 (indicated by a small
// blue box next to an arrow going from d0 to the server). The server forwards the message to Distributor
// 2, and we see the message in a small blue box next to an arrow arriving to d22. Conversely, while going
// through its own list of files, d1 finds that the File8 should be processed by Process 0, and we see
// the message mbox08 in a magenta box next to arrows going from d2 to the server and from the server to
// d0 respectively. When d0 receives the message, it adds file Index 8 to its “to do list” represented
// by the green box next to d0.

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

/// Function for reordering the fragments of text.
/// This version returns the text without the line numbers
///
/// @param fragments The fragments of text to reorder, indexed by line number.
///
/// @return A string containing the ordered text.
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
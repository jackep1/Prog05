#include "../Headers/file_ops.h"
#include <dirent.h>
#include <iostream>
#include <fstream>
using namespace std;

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

void createLock(const int processIndex)
{
    const string lockPath = "./Scrap/d_lock" + to_string(processIndex) + ".lk";
    ofstream lockFile(lockPath);
    lockFile.close();
}

void releaseLock(const int processIndex)
{
    const string lockPath = "./Scrap/d_lock" + to_string(processIndex) + ".lk";
    remove(lockPath.c_str());
}

bool distLocked()
{
    DIR* directory = opendir("./Scrap");
    if (directory == nullptr)
    {
        cout << "Scrap folder not found" << endl;
        exit(4);
    }
    vector<string> fileNames = getFilesIn("./Scrap");

    bool locked = false;
    for (const string& fileName : fileNames)
    {
        if (fileName.find("d_lock") != string::npos)
        {
            locked = true;
        }
    }

    return locked;
}
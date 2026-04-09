#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <string>
#include <vector>

/// Gets the names of all files in a directory as strings.
/// Uses the sample code from the course notes.
///
/// @param rootDirectory The directory to get the files from.
/// @return A vector of strings containing the file names.
std::vector<std::string> getFilesIn(const std::string& rootDirectory);

/// Creates a lock file "d_lock<processIndex>.lk" in the Scrap folder.
///
/// @param processIndex The index of the process creating the lock file.
void createLock(int processIndex);

/// Removes a lock file "d_lock<processIndex>.lk" in the Scrap folder.
///
/// @param processIndex The index of the process removing the lock file.
void releaseLock(int processIndex);

/// Checks if any distributor lock files still exist
///
/// @return True if any distributor lock files exist, false otherwise
bool distLocked();

#endif //FILE_OPS_H

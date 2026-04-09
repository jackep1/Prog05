#!/bin/bash

# Build the program
./Scripts/build.sh v1

# Folder containing all the data files
data_folder_path=$1
# Path to the output file to produce
output_file_path=$2

# Initialize the largest process index
largest_process_index=0

# Check each file in the folder
for file in "$data_folder_path"/*
do
    # Get the process index from the file's first line
    first_line=$(head -n 1 "$file")
    process_index=$(echo "$first_line" | grep -o '^[0-9]\+')
    if [ -n "$process_index" ] && [ "$process_index" -gt "$largest_process_index" ]
    then
        largest_process_index=$process_index
    fi
done

# Launch the server/dispatcher process -> takes the number of child processes to create, the path to the data folder, and the path to the output file
./v1 $((largest_process_index + 1)) "$data_folder_path" "$output_file_path"

# Remove the executable
rm v1
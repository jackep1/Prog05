#!/bin/bash
# Your script should be named eolFix.sh and reside in the Scripts folders (and will be run from
# the project’s root folder). It should take as arguments:

# the path to the directory containing a number of “data” files with problematic eol characters;

# the path to a “scrap folder” where to store any temporary files that your script may need to create;

# the path to the directory where to write the “fixed” data files with the proper line ending
# characters. Be careful that the user may choose the same folder as the input folder (this is
# the main purpose of the “scrap folder”).

# The files in the input directory are encoded using Windows line endings. The script must replace
# these line endings with Unix line endings. If your script creates temporary files, then it should
# create them in the indicated “scrap folder” (and create the scrap folder first in that case). At
# the end of execution, the script should delete the scrap folder and its contents if it created it.
# If you don’t need a scrap folder, then there is nothing to create and delete.

# There exist utilities that allow you to perform the end-of-line fix on an entire file with a single-line command. You are not allowed to use these commands. Again, I want to force you to write a loop in bash to iterate through every line of a text file, because this is an fundamental building block of bash programming. If you also happen to know a few extra convenient commands, this is great, but you won’t always find the “magic one-line command that solves exactly the problem.” And in that case, you will have to be able to roll your own loop.

# Check if the number of arguments is correct
if [ "$#" -ne 3 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

# Check if the input directory exists
if [ ! -d "$1" ]; then
    echo "Input directory does not exist"
    exit 1
fi

# Check if the scrap folder exists, if not create it
if [ ! -d "$2" ]; then
    mkdir "$2"
fi

# Check if the output directory exists, if not create it
if [ ! -d "$3" ]; then
    mkdir "$3"
fi

# Loop through all files in the input directory
for file in "$1"/*; do
    # Check if the file is a regular file
    if [ -f "$file" ]; then
        # Create a temporary file in the scrap folder
        tempFile="$2/$(basename "$file")"
        touch "$tempFile"
        original="$2/$(basename "$file")-o-ascii.txt"
        touch "$original"
        newascii="$2/$(basename "$file")-n-ascii.txt"
        touch "$newascii"

        # Loop through all lines in the file
        while IFS= read -r line; do
            # Check if the line uses Windows line endings, if not, add the line as-is
            if [[ "$line" != *$'\r' ]]; then
                echo "$line" >> "$tempFile"
                echo "$line" | od -An -vtu1 >> "$original"
                echo "$line" | od -An -vtu1 >> "$newascii"
                continue
            fi
            # Add the original line's ascii values to the original ascii file
            echo "$line" | od -An -vtu1 >> "$original"
            # Replace Windows line endings with Unix line endings
            echo "$line" | tr -d '\r' >> "$tempFile"
            # Add the altered line to the new ascii file
            echo "$line" | tr -d '\r' | od -An -vtu1 >> "$newascii"
        done < "$file"

        # Copy the temporary file to the output directory
        cp "$tempFile" "$3/$(basename "$file")"

        # Remove the temporary file
        rm "$tempFile"
    fi
done

# Remove the scrap folder
rm -r "$2"
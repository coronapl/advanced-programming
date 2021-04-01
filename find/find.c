/*******************************************************************************
* Author:
*   @coronapl
* Program:
*   find
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

void printFind(char *dir_name, int recursive, char *program, char *str);

int main(int argc, char *argv[]) {

    int recursive = 0;
    struct stat info;

    if (argc != 3 && argc != 4) {
        printf("usage: find string initial_directory [-r]\n");
        return -1;
    }

    if (lstat(argv[2], &info) != 0) {
        printf("find: No such file or directory");
        return -2;
    };

    if ((info.st_mode & S_IFMT) != S_IFDIR) {
        printf("find: Not a directory");
        return -2;
    }

    if (argc == 4) {
        if (strcmp(argv[3], "-r") != 0) {
            printf("usage: find string initial_directory [-r]\n");
            return -3;
        }

        recursive = 1;
    }

    printFind(argv[2], recursive, argv[0], argv[1]);

    return 0;
}

/**
 * dir_name -> name of directory where the search is executed
 * recursive (1/0) -> search in the whole tree
 * program -> name of the current program
 * str -> string to find in the name of the files
 */
void printFind(char *dir_name, int recursive, char *program, char *str)
{

    char path[PATH_MAX + NAME_MAX + 1];
    struct dirent *direntry;
    struct stat info;
    DIR *dir;

    if ((dir = opendir(dir_name)) == NULL) {
        perror(program);
        exit(-5);
    }

    while ((direntry = readdir(dir)) != NULL) {
        // Check if string exists in file name and print find
        if ((strstr(direntry->d_name, str)) != NULL) {
            printf("Name:%s\nPath: %s\n\n", direntry->d_name, dir_name);
        }
    }

    if (recursive) {
        // Move directory pointer to the beginning of the file
        rewinddir(dir);
        while ((direntry = readdir(dir)) != NULL) {

            if (strcmp(direntry->d_name, ".") != 0 &&
                strcmp(direntry->d_name, "..") != 0) {
                // Construct the path of the file
                sprintf(path, "%s/%s", dir_name, direntry->d_name);

                // Check if the file is a directory
                lstat(path, &info);
                if ((info.st_mode & S_IFMT) == S_IFDIR) {
                    // Search in the whole tree
                    printFind(path, recursive, program, str);
                }
            }
        }
    }

    closedir(dir);
}

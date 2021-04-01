/*******************************************************************************
* Author:
*   @coronapl
* Program:
*   copy
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef unsigned char uchar;

/*
* Test Position
* This functions receives a file descriptor and a
* position in the file. If the position exists in the
* file it returns 1; otherwhise, it returns -1.
*/
int test_pos(int desc, int pos) {

    uchar *c;

    lseek(desc, pos, SEEK_SET);
    if (read(desc, c, sizeof(uchar)) == 0) {
        return -1;
    }

    return 1;
}

int main(int argc, char* argv[]) {

    int source, destination;
    int start = 0, end, size;
    uchar *buffer;

    if (argc != 3 && argc != 4 && argc != 5) {
        printf("usage: %s source destination [start] [end]\n", argv[0]);
        return -1;
    }

    source = open(argv[1], O_RDONLY);
    if (source < 0) {
        printf("%s: No such file or directory\n", argv[0]);
        return -2;
    }

    destination = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (destination < 0) {
        perror(argv[0]);
        return -3;
    }

    if (argc >= 4) {
        start = atoi(argv[3]);
        if (start < 0) {
            printf("%s: start must be a positive number\n", argv[0]);
            return -4;
        }

        if (test_pos(source, start) == -1) {
            printf("%s: start must be a valid position in the file\n", argv[0]);
            return -6;
        }
    }

    if (argc == 5) {
        end = atoi(argv[4]);
        if (end < 0) {
            printf("%s: end must be a positive number\n", argv[0]);
            return -4;
        }

        if (end < start) {
            printf("%s: end must be greater than start\n", argv[0]);
            return -5;
        }

        if (test_pos(source, end) == -1) {
            printf("%s: end must be a valid position in the file\n", argv[0]);
            return -6;
        }
    }

    // Copy to the end of the file if the argument 'end' does not exist
    if (argc == 3 || argc == 4)
        end = lseek(source, 0, SEEK_END);

    // Calculate size of destination file and create buffer
    size = end - start;
    buffer = (uchar*) malloc(sizeof(uchar) * size);

    lseek(source, start, SEEK_SET);

    read(source, buffer, sizeof(uchar) * size);
    write(destination, buffer, sizeof(uchar) * size);

    free(buffer);
    close(source);
    close(destination);

    return 0;
}

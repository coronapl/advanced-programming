/*******************************************************************************
* Author:
*   @coronapl
* Program:
*   execute
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>


/**
 * This function receives the name of the file
 * and reads the program that must be executed
 * in a child process.
 */
void execute_file(char *filename) {

    char *program;
    char *parameters;

    int size;
    int fd;
    int pid;

    fd = open(filename, O_RDONLY, 0666);

    char *buffer = (char*) malloc(sizeof(char) * size);
    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    read(fd, buffer, sizeof(char) * size);
    close(fd);

    // Obtain the name of the program and the parameters
    program = strtok(buffer, ",");
    parameters = strtok(NULL, ",");

    if ((pid = fork()) == 0) {
        execlp(program, parameters, NULL);
    } else if (pid > 0) {
        wait(NULL);
        free(buffer);
    }
}

void handler_sigusr1(int sig) {
    execute_file("file1.txt");
}

void handler_sigusr2(int sig) {
    execute_file("file2.txt");
}

void handler_sighup(int sig) {
    execute_file("file3.txt");
}

void handler_sigint(int sig) {
    printf("Ending...\n");
    exit(0);
}

int main(int argc, char* argv[]) {

    int fd;

    if (argc != 4) {
        printf("usage: %s file1 file2 file3\n", argv[0]);
        return -1;
    }

    for (int i = 1; i < argc; i++)
    {
        // Check if file exists
        if ( (fd = open(argv[i], O_RDONLY, 0666)) < 0 ) {
            printf("%s: the file %s does not exist\n", argv[0], argv[i]);
            return -2;
        }

        // Check if file is not empty
        if (lseek(fd, 0, SEEK_END) == 0) {
            printf("%s: the file %s is empty\n", argv[0], argv[i]);
            return -3;
        }

        close(fd);
    }

    printf("Waiting for a signal...\n");

    // Set signal handler
    signal(SIGUSR1, handler_sigusr1);
    signal(SIGUSR2, handler_sigusr2);
    signal(SIGHUP, handler_sighup);
    signal(SIGINT, handler_sigint);

    // Wait for signals
    while(1) {
        pause();
    }

    return 0;
}

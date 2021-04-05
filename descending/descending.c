
/*******************************************************************************
* Author:
*   @coronapl
* Program:
*   descending
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * Add a tab per level
 */
void printtab(int level) {
    for (int i = 0; i <= level; i++) {
        printf("\t");
    }
}

/**
 * Level n - 1 creates n processes of level n
 * level -> current level of the tree
 * maxlevel -> level to stop creating child processes
 */
void child_process(int level, int maxlevel) {
    int pid;
    int i;

    // Recursive function base case
    if (level == maxlevel) {
        printtab(level - 1);
        printf("PPID = %i PID = %i LEVEL = %i\n", getppid(), getpid(), level);

        sleep(1);
        exit(0);
    } else {
        printtab(level - 1);
        printf("PPID = %i PID = %i LEVEL = %i\n", getppid(), getpid(), level);

        // Parent creates level + 1 child processes
        for (i = 0; i <= level; i++) {
            if ((pid = fork()) < 0) {
                perror("recursive fork");
                exit(-3);
            } else if (pid == 0) {
                child_process(++level, maxlevel);
                sleep(1);
                exit(0);
            } else {
                wait(NULL);
            }
        }
    }
}

int main(int argc, char * argv[]) {

    int levels;

    if (argc != 2) {
        printf("usage: %s number", argv[0]);
        return -1;
    }

    levels = atoi(argv[1]);
    if (levels <= 0) {
        printf("%s: the parameter must be a positive integer number", argv[0]);
        return -2;
    }

    child_process(0, levels);

    return 0;
}

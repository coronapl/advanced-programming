/*******************************************************************************
* Author:
*   @coronapl
* Program:
*   mini-shell
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"

#define MAX 512 // Max number of chars in a line

/**
 * Print the prompt of the shell.
 */
void print_prompt() {
    printf("mini-shell > ");
}

/**
 * Read user input in interactive mode.
 * @param input_line
 * @return bool
 */
int read_input(char *input_line) {
    char buffer[MAX];
    unsigned long len;

    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {

        trim(buffer);
        len = strlen(buffer);

        if (buffer[0] == '\n' || buffer[0] == '\0') {
            return 1;
        }

        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

        strcpy(input_line, buffer);
        return 0;
    }

    return 1;
}

/**
 * Execute program in a child process.
 * @param parsed
 */
void exec_program(char** arguments) {

    pid_t pid;
    if ((pid = vfork()) == -1) {
        perror("fork");
        return;
    } else if (pid == 0) {
        if (execvp(arguments[0], arguments) < 0) {
            printf("ERROR: can't execute program %s\n", arguments[0]);
            exit(-1);
        }
        exit(0);
    }
}

/**
 * Obtain programs to execute and arguments of each program.
 * @param input_line
 */
void process_line(char input_line[MAX]) {
    char *programs[MAX], *program_args[MAX];
    int exit_flag = 0;

    parse_str(input_line, programs, ";");

    int i = 0;
    while (programs[i] != NULL) {
        parse_str(programs[i], program_args, " ");

        if (strcmp(program_args[0], "quit") == 0 || strcmp(program_args[0], "CTRL-D") == 0) {
            exit_flag = 1;
            break;
        }

        exec_program(program_args);
        i++;
    }

    while (i > 0) {
        wait(NULL);
        i--;
    }

    if (exit_flag)
        exit(0);
}

int main(int argc, char *argv[]) {

    char input_line[MAX];

    if(argc != 1 && argc != 2) {
        printf("usage: %s [batchFile]\n", argv[0]);
        return -1;
    }

    if (argc == 1) {
        while (1) {
            print_prompt();
            if (read_input(input_line)) {
                continue;
            }

            process_line(input_line);
        }
    }

    if (argc == 2) {
        FILE *file;

        file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("fopen");
            return -2;
        }

        while(fgets(input_line, MAX, file)) {
            process_line(input_line);
        }

        fclose(file);
    }

    return 0;
}

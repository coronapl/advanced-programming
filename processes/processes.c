/*******************************************************************************
* Author:
*   @coronapl
* Program:
*   processes
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h>

/**
 * Parent process:
 * - Create process 1
 *      - Set SIGUSR2 -> generate_numbers()
 * - Creates process 2
 *      - Set SIGURS1 -> analyze_numbers()
 * - Set SIGUSR2 -> handler_sigusr2()
 * - Set SIGUSR1 -> handler_sigusr1()
 * - Send signal SIGUSR2 to process 1
 *
 * Process 1:
 * - Create file
 * - Generate random numbers
 * - Write numbers to file
 * - Send signal SIGUSR2 to parent process
 *
 * Parent process:
 * - Receive signal SIGUSR2
 * - Send Signal SIGUSR1 to process 2
 *
 * Process 2:
 * - Open random numbers file
 * - Calculate mean
 * - Calculate standard deviation
 * - Send signal SIGUSR1 to parent process
 *
 * Parent process:
 * - Receive signal SIGUSR1
 * - Wait for child processes
 */

int amount;
char *file;
int pid2;

void handler_sigusr2() {
    printf("Parent process received SIGUSR2\n");
    printf("\nSending SIGUSR1 to second child...\n");

    sleep(1);
    kill(pid2, SIGUSR1);
}

void handler_sigusr1() {
    printf("Parent process received SIGUSR1\n");
}

/**
 * Process 1:
 * - Create file
 * - Generate random numbers
 * - Write numbers to file
 * - Send signal SIGUSR2 to parent process
 */
void generate_numbers() {
    int fd;
    int random;
    int *buffer;

    fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open");
        exit(-4);
    }

    buffer = (int*) malloc(sizeof(int) * amount);

    printf("Numbers generated:\n");

    srand(1234);
    for (int i = 0; i < amount; i++) {
        buffer[i] = rand() % 100 + 1;
        printf("%i\n", buffer[i]);
    }

    write(fd, buffer, sizeof(int) * amount);
    free(buffer);
    close(fd);

    printf("\nSending signal SIGUSR2 to parent...\n");
    kill(getppid(), SIGUSR2);

    exit(0);
}

/**
 * Process 2:
 * - Open random numbers file
 * - Calculate mean
 * - Calculate standard deviation
 * - Send signal SIGUSR1 to parent process
 */
void analyze_numbers() {
    int fd;
    int *buffer;
    float mean, sd;
    int temp = 0;

    buffer = (int*) malloc(sizeof(int) * amount);

    if ((fd = open(file, O_RDONLY, 0666)) < 0 ) {
        perror("open");
        exit(-4);
    }

    read(fd, buffer, sizeof(int) * amount);
    close(fd);

    for (int i = 0; i < amount; i++) {
        temp += buffer[i];
    }

    mean = (float) temp / amount;

    for (int i = 0; i < amount; i++) {
        sd += pow(buffer[i] - mean, 2);
    }

    sd = sqrt(sd / amount);

    printf("\nMean: %f\n", mean);
    printf("Standard deviation: %f\n", sd);

    printf("\nSending signal SIGUSR1 to parent...\n");
    kill(getppid(), SIGUSR1);

    free(buffer);
    exit(0);
}

int main(int argc, char *argv[]) {
    amount = 100;
    int pid1;

    if (argc != 2 && argc != 3) {
        printf("usage: %s file [amount]", argv[0]);
        return -1;
    }

    file = argv[1];

    if (argc == 3) {
        amount = atoi(argv[2]);
        if (amount < 1) {
            printf("Amount must be a positive number");
            return -2;
        }
    }

    // Create child processes
    if ((pid1 = fork()) < 0) {
        perror("fork");
        exit(-3);
    } else if (pid1 == 0) {
        signal(SIGUSR2, generate_numbers);
        while(1) {
            pause();
        }
    }

    if ((pid2 = fork()) < 0) {
        perror("fork");
        exit(-3);
    } else if (pid2 == 0) {
        signal(SIGUSR1, analyze_numbers);
        while(1) {
            pause();
        }
    }

    // Parent process handlers
    signal(SIGUSR2, handler_sigusr2);
    signal(SIGUSR1, handler_sigusr1);

    sleep(1);
    kill(pid1, SIGUSR2);

    int i = 0;
    while (i < 2) {
        wait(NULL);
        i++;
    }

    return 0;
}
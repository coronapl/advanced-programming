/*******************************************************************************
* Author:
*   @coronapl
* Program:
*   multiply
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

// Calculate execution time
struct timeval startTime, stopTime;
int started = 0;

void start_timer() {
    started = 1;
    gettimeofday(&startTime, NULL);
}

double stop_timer() {
    long seconds, useconds;
    double duration = -1;

    if (started) {
        gettimeofday(&stopTime, NULL);
        seconds  = stopTime.tv_sec  - startTime.tv_sec;
        useconds = stopTime.tv_usec - startTime.tv_usec;
        duration = (seconds * 1000.0) + (useconds / 1000.0);
        started = 0;
    }
    return duration;
}

typedef struct {
    float **matrixA;
    float **matrixB;
    float **result;
    int start;
    int end;
    int size;
} Block;

/**
 * Function executed by each thread. It calculates the multiplication
 * of n rows.
 * @param param
 * @return
 */
void *multiply(void *param) {
    Block *block;
    block = (Block *) param;

    for (int i = block->start; i < block->end; i++)
        for (int j = 0; j < block->size; j++)
            for (int k = 0; k < block->size; k++)
                block->result[i][j] += block->matrixA[i][k] * block->matrixB[k][j];

    pthread_exit(NULL);
}

/**
 * Function used to read the values of each matrix.
 * @param file
 * @param matrix
 * @param size
 */
void read_matrix(FILE *file, float **matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fscanf(file, "%f", &matrix[i][j]);
        }
    }
}

int main(int argc, char *argv[]) {

    FILE *fileA, *fileB;
    float **matrixA, **matrixB, **result;
    Block *blocks;
    double time;

    pthread_t *pthreads;
    int n_threads, rows_thread;

    // Verify correct number of arguments
    if (argc != 7) {
        printf("usage: %s -a matrix_file_a.txt -b matrix_file_b.txt -t thread_amount\n", argv[0]);
        return -2;
    }

    // Verify correct parameters
    if (strcmp(argv[1],"-a") != 0) {
        printf("usage: %s -a matrix_file_a.txt -b matrix_file_b.txt -t thread_amount\n", argv[0]);
        return -3;
    }

    if (strcmp(argv[3],"-b") != 0) {
        printf("usage: %s -a matrix_file_a.txt -b matrix_file_b.txt -t thread_amount\n", argv[0]);
        return -4;
    }

    if (strcmp(argv[5],"-t") != 0) {
        printf("usage: %s -a matrix_file_a.txt -b matrix_file_b.txt -t thread_amount\n", argv[0]);
        return -5;
    }

    // Try to open the files
    fileA = fopen(argv[2], "r");
    if (fileA == NULL) {
        perror("fopen");
        return -6;
    }

    fileB = fopen(argv[4], "r");
    if (fileB == NULL) {
        perror("fopen");
        return -7;
    }

    // Verify is n_threads is an integer greater than 0
    n_threads = atoi(argv[6]);
    if (n_threads <= 0) {
        printf("ERROR: the number of threads must be positive.");
        return -8;
    }

    // Get size of each matrix
    int size, sizeB;
    fscanf(fileA, "%i", &size);
    fscanf(fileB, "%i", &sizeB);

    if (size != sizeB) {
        printf("ERROR: the matrices must have the same size.");
        return -9;
    }

    // Create matrices A and B and read values from file
    matrixA = (float **) malloc(sizeof(float *) * size);
    for (int i = 0; i < size; i++) {
        matrixA[i] = (float *) malloc(sizeof(float) * size);
    }

    matrixB = (float **) malloc(sizeof(float *) * size);
    for (int i = 0; i < size; i++) {
        matrixB[i] = (float *) malloc(sizeof(float) * size);
    }

    read_matrix(fileA, matrixA, size);
    read_matrix(fileB, matrixB, size);

    // Create result matrix
    result = (float **) malloc(sizeof(float *) * size);
    for (int i = 0; i < size; i++) {
        result[i] = (float *) malloc(sizeof(float *) * size);
    }

    // Do not create more threads than rows
    if (n_threads > size) {
        n_threads = size;
    }

    pthreads = (pthread_t*) malloc(sizeof(pthread_t) * n_threads);
    blocks = (Block *) malloc(sizeof(Block) * n_threads);
    rows_thread = size / n_threads;

    for (int i = 0; i < n_threads; i++) {
        Block block;
        block.matrixA = matrixA;
        block.matrixB = matrixB;
        block.result = result;
        block.start = i * rows_thread;

        if (i == n_threads - 1) {
            block.end = (i * rows_thread) + (size - (i * rows_thread));
        } else {
            block.end = (i * rows_thread) + rows_thread;
        }

        block.size = size;
        blocks[i] = block;
    }

    start_timer();
    for (int i = 0; i < n_threads; i++) {
        pthread_create(&pthreads[i], NULL, multiply, (void *) &blocks[i]);
    }
    time = stop_timer();

    for (int i = 0; i < n_threads + 1; i++) {
        pthread_join(pthreads[i], NULL);
    }

    printf("Time = %f\n", time);
    printf("%i\n", size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%.3f\t", result[i][j]);
        }
        printf("\n");
    }

    free(matrixA);
    free(matrixB);
    free(result);
    free(pthreads);
    free(blocks);

    fclose(fileA);
    fclose(fileB);

    return 0;
}

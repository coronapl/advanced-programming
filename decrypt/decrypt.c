/*******************************************************************************
* Author:
*   @coronapl
* Program:
*   decrypt
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef unsigned char uchar;
typedef unsigned long ulong;

/**
 * invert_bits - Swap the lower half for the upper half
 * Examples:
 * 1111 0000 -> 0000 1111
 * 1010 1111 -> 1111 1010
 */
uchar invert_bits(uchar c)
{
    uchar lower, upper;

    upper = (c & 0x0f) << 4;
    lower = (c & 0xf0) >> 4;

    return (upper | lower);
}

int main(int argc, char *argv[])
{

    int encrypted, decrypted; // File descriptors
    uchar *buffer;
    ulong size, i;

    if (argc != 3)
    {
        printf("usage: %s encrypted_file decrypted_file\n", argv[0]);
        return -1;
    }

    encrypted = open(argv[1], O_RDONLY);
    if (encrypted < 0)
    {
        perror(argv[0]);
        return -2;
    }

    decrypted = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (decrypted < 0)
    {
        perror(argv[0]);
        return -3;
    }

    // Obtain size of encrypted file
    size = lseek(encrypted, 0, SEEK_END);
    lseek(encrypted, 0, SEEK_SET);

    // Copy encrypted file to the buffer
    buffer = (uchar *)malloc(sizeof(uchar) * size);
    read(encrypted, buffer, sizeof(uchar) * size);

    // Invert each character
    i = 0;
    while (i <= size)
    {
        buffer[i] = invert_bits(buffer[i]);
        printf("%c", buffer[i]);
        ++i;
    }

    // Write buffer to decrypted file
    write(decrypted, buffer, sizeof(uchar) * size);

    free(buffer);
    close(encrypted);
    close(decrypted);

    return 0;
}

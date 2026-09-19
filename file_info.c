/* Lab 1: UNIX File Management - Eldana Adinew */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int fd = open("input.txt", O_RDONLY);
    if (fd == -1) { perror("open input.txt"); return EXIT_FAILURE; }
    int status = EXIT_SUCCESS;
    if (printf("File descriptor: %d\n", fd) < 0 || fflush(stdout) == EOF) {
        perror("stdout"); status = EXIT_FAILURE;
    }
    if (close(fd) == -1) { perror("close input.txt"); status = EXIT_FAILURE; }
    return status;
}

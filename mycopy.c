/* Lab 1: UNIX File Management - Eldana Adinew */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
/* write() may transfer fewer bytes than requested. */
static int write_all(int fd, const char *buffer, size_t count)
{
    size_t done = 0;
    while (done < count) {
        ssize_t n = write(fd, buffer + done, count - done);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) { errno = EIO; return -1; }
        done += (size_t)n;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s source destination\n", argv[0]);
        return EXIT_FAILURE;
    }
    int source = open(argv[1], O_RDONLY);
    if (source == -1) { perror(argv[1]); return EXIT_FAILURE; }
    int status = EXIT_FAILURE;
    struct stat src, dst;
    if (fstat(source, &src) == -1) { perror("fstat source"); goto close_source; }
    /* Protect an unchanged source/destination pair, including hard links.
       This classroom utility assumes paths are not changed concurrently. */
    if (stat(argv[2], &dst) == 0) {
        if (src.st_dev == dst.st_dev && src.st_ino == dst.st_ino) {
            fprintf(stderr, "Source and destination are the same file.\n");
            goto close_source;
        }
    } else if (errno != ENOENT) { perror(argv[2]); goto close_source; }
    int destination = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (destination == -1) { perror(argv[2]); goto close_source; }
    char buffer[4096];
    status = EXIT_SUCCESS;
    for (;;) {
        ssize_t n = read(source, buffer, sizeof buffer);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("read source"); status = EXIT_FAILURE; break;
        }
        if (n == 0) break;
        if (write_all(destination, buffer, (size_t)n) == -1) {
            perror("write destination"); status = EXIT_FAILURE; break;
        }
    }
    if (close(destination) == -1) {
        perror("close destination"); status = EXIT_FAILURE;
    }
close_source:
    if (close(source) == -1) { perror("close source"); status = EXIT_FAILURE; }
    return status;
}

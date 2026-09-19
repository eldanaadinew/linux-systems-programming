/* Lab 1: UNIX File Management - Eldana Adinew */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
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
    unsigned long limit = 10;
    const char *filename;
    if (argc == 2) filename = argv[1];
    else if (argc == 3) {
        const char *s = argv[1];
        if (*s == '\0') goto usage;
        for (const char *c = s; *c; ++c)
            if (*c < '0' || *c > '9') goto usage;
        char *end;
        errno = 0;
        limit = strtoul(s, &end, 10);
        if (errno == ERANGE || *end != '\0') goto usage;
        filename = argv[2];
    } else goto usage;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) { perror(filename); return EXIT_FAILURE; }
    unsigned long lines = 0;
    int status = EXIT_SUCCESS;
    char buffer[4096];
    while (lines < limit) {
        ssize_t n = read(fd, buffer, sizeof buffer);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("read file"); status = EXIT_FAILURE; break;
        }
        if (n == 0) break;
        size_t count = 0;
        while (count < (size_t)n && lines < limit) {
            if (buffer[count] == '\n') ++lines;
            ++count;
        }
        if (write_all(STDOUT_FILENO, buffer, count) == -1) {
            perror("write stdout"); status = EXIT_FAILURE; break;
        }
    }
    if (close(fd) == -1) { perror("close file"); status = EXIT_FAILURE; }
    return status;
usage:
    fprintf(stderr, "Usage: %s [nonnegative_line_count] filename\n", argv[0]);
    return EXIT_FAILURE;
}

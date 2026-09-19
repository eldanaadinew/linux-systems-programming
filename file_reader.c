/* Lab 1: UNIX File Management - Eldana Adinew */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

int main(void)
{
#ifdef FORCE_OPEN_ERROR
    const char *filename = "intentionally_missing.txt"; /* Part XIII test build. */
#else
    const char *filename = "input.txt";
#endif
    int fd = open(filename, O_RDONLY);
    if (fd == -1) { perror(filename); return EXIT_FAILURE; }
    char buffer[24]; /* Small buffer makes multiple reads visible. */
    int status = EXIT_SUCCESS;
    for (;;) {
        ssize_t n = read(fd, buffer, sizeof buffer);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("read input.txt"); status = EXIT_FAILURE; break;
        }
        /* Part V: diagnostics go to stderr; stdout contains only file bytes. */
        if (fprintf(stderr, "Read %zd bytes\n", n) < 0) {
            status = EXIT_FAILURE; break;
        }
        if (n == 0) break;
        if (write_all(STDOUT_FILENO, buffer, (size_t)n) == -1) {
            perror("write stdout"); status = EXIT_FAILURE; break;
        }
    }
    if (close(fd) == -1) { perror("close input.txt"); status = EXIT_FAILURE; }
    return status;
}

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
    const char out[] = "This is standard output.\n";
    const char err[] = "This is standard error.\n";
    int status = EXIT_SUCCESS;
    /* Build with -DUSE_LITERAL_DESCRIPTORS for Part VIII's initial version. */
#ifdef USE_LITERAL_DESCRIPTORS
    const int output_fd = 1, error_fd = 2;
#else
    const int output_fd = STDOUT_FILENO, error_fd = STDERR_FILENO;
#endif
    if (write_all(output_fd, out, sizeof out - 1) == -1) {
        perror("write stdout"); status = EXIT_FAILURE;
    }
    if (write_all(error_fd, err, sizeof err - 1) == -1) {
        perror("write stderr"); status = EXIT_FAILURE;
    }
    return status;
}

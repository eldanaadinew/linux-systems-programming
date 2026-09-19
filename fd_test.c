/* Lab 1: UNIX File Management - Eldana Adinew */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int fds[3] = {-1, -1, -1};
    const char *names[] = {"fd_a.txt", "fd_b.txt", "fd_c.txt"};
    int status = EXIT_SUCCESS;
    printf("stdin=%d stdout=%d stderr=%d\n",
           STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO);
    for (int i = 0; i < 3; ++i) {
        fds[i] = open(names[i], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fds[i] == -1) { perror(names[i]); status = EXIT_FAILURE; goto cleanup; }
        printf("%s: %d\n", names[i], fds[i]);
    }
    int released = fds[1];
    int result = close(fds[1]);
    fds[1] = -1; /* Never blindly retry close(): the fd may have been released. */
    if (result == -1) { perror("close middle file"); status = EXIT_FAILURE; goto cleanup; }
    printf("Closed descriptor: %d\n", released);
    fds[1] = open("fd_d.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fds[1] == -1) { perror("fd_d.txt"); status = EXIT_FAILURE; goto cleanup; }
    printf("fd_d.txt: %d\n", fds[1]);
cleanup:
    for (int i = 0; i < 3; ++i) {
        if (fds[i] != -1 && close(fds[i]) == -1) {
            perror("close file"); status = EXIT_FAILURE;
        }
    }
    if (fflush(stdout) == EOF || ferror(stdout)) {
        perror("stdout"); status = EXIT_FAILURE;
    }
    return status;
}

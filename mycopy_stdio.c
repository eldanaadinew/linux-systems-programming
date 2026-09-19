/* Lab 1: UNIX File Management - Eldana Adinew */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s source destination\n", argv[0]);
        return EXIT_FAILURE;
    }
    FILE *source = fopen(argv[1], "rb");
    if (source == NULL) { perror(argv[1]); return EXIT_FAILURE; }
    int status = EXIT_FAILURE;
    struct stat src, dst;
    if (stat(argv[1], &src) == -1) { perror(argv[1]); goto close_source; }
    /* stat() is metadata-only; all file data I/O uses Standard I/O. */
    if (stat(argv[2], &dst) == 0) {
        if (src.st_dev == dst.st_dev && src.st_ino == dst.st_ino) {
            fprintf(stderr, "Source and destination are the same file.\n");
            goto close_source;
        }
    } else if (errno != ENOENT) { perror(argv[2]); goto close_source; }
    FILE *destination = fopen(argv[2], "wb");
    if (destination == NULL) { perror(argv[2]); goto close_source; }
    unsigned char buffer[4096];
    status = EXIT_SUCCESS;
    for (;;) {
        size_t n = fread(buffer, 1, sizeof buffer, source);
        if (n > 0) {
            size_t done = 0;
            while (done < n) {
                size_t written = fwrite(buffer + done, 1, n - done, destination);
                done += written;
                if (ferror(destination) || written == 0) {
                    if (errno == 0) errno = EIO;
                    perror("fwrite destination");
                    status = EXIT_FAILURE; break;
                }
            }
        }
        if (status != EXIT_SUCCESS) break;
        if (n < sizeof buffer) {
            if (ferror(source)) {
                if (errno == 0) errno = EIO;
                perror("fread source"); status = EXIT_FAILURE;
            }
            if (ferror(source) || feof(source)) break;
        }
    }
    /* fclose() also checks for errors when buffered output is flushed. */
    if (fclose(destination) == EOF) {
        perror("fclose destination"); status = EXIT_FAILURE;
    }
close_source:
    if (fclose(source) == EOF) { perror("fclose source"); status = EXIT_FAILURE; }
    return status;
}

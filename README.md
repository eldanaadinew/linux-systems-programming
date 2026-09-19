# Linux Systems Programming and File Tools

A collection of C command-line utilities that demonstrate POSIX file descriptors, low-level and buffered file I/O, standard output/error handling, error recovery, and UNIX-style program behavior.

## Highlights

- Implements file copying with both POSIX system calls and C standard I/O.
- Recreates core `head` behavior with configurable line counts and buffered reads.
- Handles interrupted reads/writes, partial writes, invalid arguments, missing files, permission errors, and full output devices.
- Demonstrates file-descriptor allocation, reuse, and separation of stdout and stderr.
- Includes an automated suite covering compilation, normal behavior, edge cases, and failure paths.

## Programs

| Program | Purpose |
| --- | --- |
| `file_info` | Opens a file and reports its descriptor |
| `file_reader` | Reads file bytes and reports per-read diagnostics |
| `mycopy` | Copies files with `open`, `read`, and `write` |
| `mycopy_stdio` | Copies files with `fopen`, `fread`, and `fwrite` |
| `standard_io` | Demonstrates stdout and stderr handling |
| `fd_test` | Demonstrates descriptor allocation and reuse |
| `myhead` | Prints the first ten lines or a requested line count |

## Build and run

Requirements: Linux or another POSIX-compatible environment, GCC, GNU Make, and Python 3.

```bash
make
./file_info
./file_reader
./mycopy input.txt copy.txt
./mycopy_stdio input.txt copy2.txt
./myhead lines.txt
./myhead 5 lines.txt
```

## Tests

```bash
python3 run_tests.py
```

The included test suite verifies warning-free compilation and exercises text, binary, empty, large-file, invalid-input, permission, descriptor, and output-device failure cases. The saved run completed all 55 checks successfully.

## Technologies

- C11
- POSIX system calls
- Linux/UNIX
- GCC and GNU Make
- Python test automation

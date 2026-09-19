CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
PROGRAMS = file_info file_reader mycopy standard_io mycopy_stdio fd_test myhead

.PHONY: all clean
all: $(PROGRAMS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(PROGRAMS)



CC = gcc
INCLUDE := -I../src
CFLAGS := -g -Wall $(INCLUDE)

PROGNAME = ioctl mmap tx

all: $(PROGNAME)

.c.o:
	$(CC) $< -o $@

clean:
	rm -rf *.o
	rm -rf $(PROGNAME)

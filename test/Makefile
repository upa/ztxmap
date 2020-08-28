

CC = gcc
INCLUDE := -I../src
CFLAGS := -g -Wall $(INCLUDE)

PROGNAME = ioctl

all: $(PROGNAME)

.c.o:
	$(CC) $< -o $@

clean:
	rm -rf *.o
	rm -rf $(PROGNAME)

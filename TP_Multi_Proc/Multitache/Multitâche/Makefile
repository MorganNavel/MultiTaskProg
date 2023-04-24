CC = gcc
CFLAGS = -Wall

SRCS = $(wildcard *.c)
BINS = $(patsubst %.c, bin/%, $(SRCS))

all: $(BINS)

bin/%: %.c | bin
	$(CC) $(CFLAGS) $< -o $@

bin:
	mkdir -p bin

clean:
	rm -rf bin/*

.PHONY: all clean

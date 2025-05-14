CC=gcc
CFLAGS=-Wall -Wextra -O2 -D_DEFAULT_SOURCE
LDFLAGS=-lzmq
SRC=$(wildcard *.c)
OUT=server

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
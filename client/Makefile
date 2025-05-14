CC=gcc
CFLAGS=-Wall -Wextra -O2
LDFLAGS=-lzmq
SRC=main.c
OUT=client

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
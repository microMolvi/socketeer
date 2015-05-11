CC=gcc
CFLAGS=-g -Wall -Wextra
LDFLAGS=-lncurses -lpthread

all: server client

server:
	$(CC) $(CFLAGS) socketeers.c -o socketeers

client:
	$(CC) $(CFLAGS) socketeerc.c -o socketeerc $(LDFLAGS)

clean:
	rm socketeers socketeerc

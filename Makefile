CC=gcc
CFLAGS=-g -Wall -Wextra
LDFLAGS=-lncurses -lpthread

all: server client

server:
	$(CC) $(CFLAGS) socketeers.c -o socketeers

client:
	$(CC) $(CFLAGS) $(LDFLAGS) socketeerc.c -o socketeerc

clean:
	rm socketeers socketeerc

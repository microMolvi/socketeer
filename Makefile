CC=gcc
CFLAGS=-g -Wall -Wextra
LDFLAGS=-lncurses

all: server client win

server:
	$(CC) $(CFLAGS) chatroomserver.c -o chatroomserver

client:
	$(CC) $(CFLAGS) chatroomclient.c -o chatroomclient

win:
	$(CC) $(CFLAGS) $(LDFLAGS) chatwin.c -o chatwin

clean:
	rm -rf chatroomserver chatroomclient chatwin

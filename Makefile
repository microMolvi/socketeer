CC=gcc
CFLAGS=-g -Wall -Wextra
LDFLAGS=-lncurses -lpthread

DESTDIR=./SOCKETEER

all: server client

server:
	$(CC) $(CFLAGS) socketeers.c -o socketeers

client:
	$(CC) $(CFLAGS) socketeerc.c -o socketeerc $(LDFLAGS)

install:
	install -D socketeers ${DESTDIR}/socketeers
	install -D socketeerc ${DESTDIR}/socketeerc

clean:
	rm socketeers socketeerc

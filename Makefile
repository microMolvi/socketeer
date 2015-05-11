#CC=${CC}
#CFLAGS=-g -Wall -Wextra"
#LDFLAGS=-lncurses -lpthread"

all: server client

server:
	$(CC) $(CFLAGS) socketeers.c -o socketeers $(LDFLAGS)

client:
	$(CC) $(CFLAGS) socketeerc.c -o socketeerc $(LDFLAGS)

install:
	install -D socketeers ${DESTDIR}/socketeers
	install -D socketeerc ${DESTDIR}/socketeerc

clean:
	rm socketeers socketeerc

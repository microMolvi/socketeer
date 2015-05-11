CC=${CC}
CFLAGS=${CFLAGS}" -g -Wall -Wextra"
LDFLAGS=${LDFLAGS}" -lncurses -lpthread"

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

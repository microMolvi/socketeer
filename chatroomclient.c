#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT argv[2]
#define username argv[3]

int main(int argc, char *argv[])
{
    struct addrinfo hints, *p, *res;
    int gais, sd;

    char recvbytes[256];
    int nbytes;

    char sendbytes[256];

    fd_set readfds;
    FD_ZERO(&readfds);

    struct sockaddr_storage *serverAddr;
    socklen_t serverAddrLen;
    serverAddrLen = sizeof(struct sockaddr_storage);
    if(argc != 4)
    {
        printf("usage: chatroomclient host port username\n");
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((gais = getaddrinfo(argv[1], PORT, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gais));
        exit(1);
    }

    for(p = res; p != NULL; p = p->ai_next)
    {
        sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sd == -1)
        {
            perror("socket:");
            continue;
        }
        else
        {
            serverAddr = (struct sockaddr_storage*) p->ai_addr;
            if(connect(sd, (struct sockaddr*)serverAddr, serverAddrLen) == -1)
            {
                perror("connect");
                printf("Trying again\n");
                close(sd);
                continue;
            }
            else
            {
                break;
            }
        }
    }

    if(p == NULL)
    {
        printf("unable to connect\n");
        exit(2);
    }

    freeaddrinfo(res);

    printf("Connection Successful\n");
    FD_SET(sd, &readfds);

//    send(sd, "HOLA!\n", sizeof("HOLA!\n"), 0);

    if(!fork())
    {
//        printf("child speaking\n");
        while(1)
        {
            if((nbytes = recv(sd, recvbytes, sizeof(recvbytes), 0)) <= 0)
            {
                if(nbytes == 0)
                {
                    printf("server is gone!!\n");
                }
                else
                {
                    perror("recv:");
                    exit(5);
                }
            }
            else
            {
                recvbytes[nbytes] = '\0';
                printf("%s", recvbytes);
            }
        }
    }

//    printf("Parent Speaking\n");
    while(1)
    {
        fgets(sendbytes, 255, stdin);
        if((send(sd, username, strlen(username), 0) == -1))
        {
            perror("send");
        }
        if((send(sd, ": ", strlen(": "), 0) == -1))
        {
            perror("send");
        }
        if((send(sd, sendbytes, strlen(sendbytes), 0) == -1))
        {
            perror("send");
        }
    }

    return 0;
}

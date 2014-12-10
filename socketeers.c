/*
 * chatroomserver.c
 * Accepts connections from N_CLIENTS
 * Whatever is recv()ed is send()ed to all connected
 * #TODO append them names
 * #TODO implement own library for common routines
 * #FIXME write a wrapper for partial sends
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 5
#define PORT    "3199"

int main(void)
{
    struct addrinfo hints, *p, *res;
    int sd, nsd; //socket, new socket
    int gais; //getaddrinfo status
    int yes; //setsockopt needs this
    //char *str2send = "Selam Aleykum!!\n";

    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen;

    fd_set mS, mS_cp; //master fd_set and its copy
    int fdmax, i, j;

    int recvbytes;
    char recvdata[256];

    //setting up hints for getaddrinfo
    memset( &hints, 0, sizeof( struct addrinfo ) );
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    clientAddrLen = sizeof(struct sockaddr_storage);

    //zeroing both fd_sets
    FD_ZERO(&mS);
    FD_ZERO(&mS_cp);

    if((gais = getaddrinfo( NULL, PORT, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gais));
        exit(1);
    }

    for( p = res ;p != NULL; p = p->ai_next )
    {
        if((sd = socket( p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(2);
        }

        if(bind(sd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("server: bind");
            close(sd);  //need to close this one o/w subsequent bind()s will fail too
            continue;
        }
        break; //break on first sd that bind()s
    }

    if(p == NULL)
    {
        fprintf(stderr, "server: failed to bind()\n");
        exit(3);
    }

    freeaddrinfo(res); //we got ourselves a socket, dont need this no more

    if(listen(sd, BACKLOG) == -1)
    {
        perror("listen: ");
        exit(4);
    }

    // added the listening socket to the fd_set so that we can know
    // whenever someone is connecting
    FD_SET(sd, &mS);
    fdmax = sd;

    while(1)
    {
        mS_cp = mS;
        if(select(fdmax + 1, &mS_cp, NULL, NULL, NULL) == -1)
        {
            perror("select: ");
            exit(5);
        }

        for(i = 0 ; i <= fdmax; i++)
        {
            if(FD_ISSET(i, &mS_cp)) // somethings up!!
            {
                if(i == sd) // someone's connecting yay \(^_^)/
                {
                    nsd = accept(sd, (struct sockaddr*) &clientAddr, &clientAddrLen);
                    if(nsd == -1)
                    {
                        perror("accept: ");
                    }
                    else
                    {
                        FD_SET(nsd, &mS);
                        if(nsd > fdmax)
                        {
                            fdmax = nsd;
                        }
                    }
                    //send(nsd, str2send, strlen(str2send), 0);
                }
                else
                {
                    //serve the client(s)
                    if((recvbytes = recv(i, recvdata, sizeof(recvdata), 0)) <= 0)
                    {
                        if(recvbytes == 0)
                        {
                            printf("clients gone!!\n");
                        }
                        else
                        {
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &mS);
                    }
                    else
                    {
                        for(j = 0; j <= fdmax; j++)
                        {
                            if(FD_ISSET(j, &mS))
                            {
                                if(j == i || j == sd)
                                {
                                    continue;
                                }
                                else
                                {
                                    if((send(j, recvdata, recvbytes, 0) == -1))
                                    {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    } // end of broadcast
                } // end of serving clients
            } // end of activity
        } // end of fd loop
    } // end of while(1)
    return 0; //lel
} //end of main

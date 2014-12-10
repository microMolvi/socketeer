#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ncurses.h>
#include <pthread.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT argv[2]

void *getnsend(void *client);
void *recvnput(void *client);

WINDOW *chatbox, *sendbox, *userbox;
int cbox_n = 0; //cbox line number;

struct client_data
{
    int sockd;
    char *username;
};

int main(int argc, char** argv)
{

    ////////////////////////////////
    // Socket Initialization Code //
    ////////////////////////////////

    struct client_data *client;
    client = malloc(sizeof(struct client_data));
    memset(client, 0, sizeof(struct client_data));

    struct addrinfo hints, *p, *res;
    int gais, sd;

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

    client->username = malloc(sizeof(char)*20);
    strcpy(client->username, argv[3]);

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

    client->sockd = sd;
    ////////////////////////////////
    //  Curses Windows Init Code  //
    ////////////////////////////////

    int parent_x, parent_y; // parent window max limits
    int maxy, maxx; // chatbox max limits

    int sb_size = 3;
    int users_width = 15;
    initscr();
    noecho();
    curs_set(FALSE);

    // Getting max limits of the Parent Window
    getmaxyx(stdscr, parent_y, parent_x);

    // Getting ourselves some windows to work on
    chatbox = newwin(parent_y - sb_size, parent_x - users_width, 0, 0);
    sendbox = newwin(sb_size, parent_x - users_width, parent_y - sb_size, 0);
    userbox = newwin(parent_y, users_width, 0, parent_x - users_width);

    scrollok(chatbox, TRUE); // Allows the chatbox to scroll
    getmaxyx(chatbox, maxy, maxx);
    wsetscrreg(chatbox, 1, maxy-2); // Sets the scrolling region

    // Default borders
    box(chatbox, 0, 0);
    box(sendbox, 0, 0);
    box(userbox, 0, 0);

    wrefresh(chatbox);
    wrefresh(sendbox);
    wrefresh(userbox);

    ////////////////////////////////
    // Thread Initialization Code //
    ////////////////////////////////

    pthread_t sender, recver;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Spawning send & recv threads
    pthread_create(&sender, &attr, getnsend, (void *)client);
    pthread_create(&recver, &attr, recvnput, (void *)client);

    while(1);

    //lel
    endwin();
    return 0;
}

void *recvnput(void *client)
{
    char str[100]; // username: + recv_str
    char recv_str[80];

    int maxy, maxx;
    getmaxyx(chatbox, maxy, maxx);

    struct client_data cd = *(struct client_data *)client;

    int nbytes;

    while(1)
    {
        if((nbytes = recv(cd.sockd, recv_str, sizeof(recv_str), 0)) <= 0)
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
            recv_str[nbytes] = '\0'; // Terminating received string

            // Append username to str
            strcpy(str, recv_str);
            //strcat(str, ": ");
            //strcat(str, recv_str);

            // Update Chatbox
            if(cbox_n != maxy - 2)
            {
                cbox_n++; // move to next line before every new message
            }
            else
            {
                scroll(chatbox); // Scroll when chatbox is full
            }

            mvwprintw(chatbox, cbox_n, 1, str);
            box(chatbox, 0, 0);
            wrefresh(chatbox);
        }
    }
}

// Get input from user and send to the server
// The server send the data to all connected clients
// except for this one
void *getnsend(void *client)
{
    char str[100];     // the final string, send to rest & chatbox
    char send_str[80]; // input from the user

    int maxy, maxx;
    getmaxyx(chatbox, maxy, maxx);

    struct client_data cd = *(struct client_data *)client;

    while(1)
    {
        // Get user's message
        echo();
        mvwgetstr(sendbox, 1, 1, send_str);
        noecho();
        wclear(sendbox);
        box(sendbox, 0, 0);
        wrefresh(sendbox);

        // Append username to str
        strcpy(str, cd.username);
        strcat(str, ": ");
        strcat(str, send_str);


        // Send to the rest of the world
        if((send(cd.sockd, str, strlen(str), 0) == -1))
        {
            perror("send");
        }

        // Update Chatbox
        if(cbox_n != maxy - 2)
        {
            cbox_n++; // move to next line before every new message
        }
        else
        {
            scroll(chatbox); // Scroll when chatbox is full
        }

        mvwprintw(chatbox, cbox_n, 1, str);
        box(chatbox, 0, 0);
        wrefresh(chatbox);
    }
}

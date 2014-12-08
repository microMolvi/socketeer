#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


void *sendmessage();

WINDOW *chatbox, *sendbox, *userbox;
int cbox_n = 0; //cbox line number;

int main(void)
{
    int parent_x, parent_y;//, new_x, new_y;

    int sb_size = 3;
    int users_width = 15;
    initscr();
    noecho();
    curs_set(FALSE);

    // set up initial windows
    getmaxyx(stdscr, parent_y, parent_x);

    // getting ourselves some windows to work on
    chatbox = newwin(parent_y - sb_size, parent_x - users_width, 0, 0);
    sendbox = newwin(sb_size, parent_x - users_width, parent_y - sb_size, 0);
    userbox = newwin(parent_y, users_width, 0, parent_x - users_width);

    scrollok(chatbox, TRUE); // Allows the chatbox to scroll
    int maxy, maxx;
    getmaxyx(chatbox, maxy, maxx);
    wsetscrreg(chatbox, 1, maxy-2); // Sets the scrolling region

    box(chatbox, 0, 0);
    box(sendbox, 0, 0);
    box(userbox, 0, 0);

    wrefresh(chatbox);
    wrefresh(sendbox);
    wrefresh(userbox);

    // Set up threads
    pthread_t sender;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Spawn the listen/receive deamons
    pthread_create(&sender, &attr, sendmessage, NULL);

    while(1);

    //lel
    endwin();
    return 0;
}

// Send message from keyboard to server and update screen
void *sendmessage()
{
    int maxy, maxx;
    getmaxyx(chatbox, maxy, maxx);

    char str[80];

    while(1)
    {
        // Get user's message
        echo();
        mvwgetstr(sendbox, 1, 1, str);
        noecho();
        wclear(sendbox);
        box(sendbox, 0, 0);
        wrefresh(sendbox);

        // write it in chatbox
        if(cbox_n != maxy - 2)
        {
            cbox_n++;
        }
        else
        {
            scroll(chatbox);
        }

        mvwprintw(chatbox, cbox_n, 1, str);
        box(chatbox, 0, 0);
        wrefresh(chatbox);
    }
}

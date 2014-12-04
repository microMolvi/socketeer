#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>

#define SEND_SIZE 100
#define RECV_SIZE 100

void draw_borders(WINDOW *screen)
{
    int x, y, i;

    getmaxyx(screen, y, x);

    // 4 corners
    mvwprintw(screen, 0, 0, "+");
    mvwprintw(screen, y - 1, 0, "+");
    mvwprintw(screen, 0, x - 1, "+");
    mvwprintw(screen, y - 1, x - 1, "+");

    // sides
    for (i = 1; i < (y - 1); i++)
    {
        mvwprintw(screen, i, 0, "|");
        mvwprintw(screen, i, x - 1, "|");
    }

    // top and bottom
    for (i = 1; i < (x - 1); i++)
    {
        mvwprintw(screen, 0, i, "-");
        mvwprintw(screen, y - 1, i, "-");
    }
}

int main(int argc, char *argv[])
{
    int parent_x, parent_y, new_x, new_y;
    int sb_size = 3;
    int users_width = 15;
    int pfds[2]; // pipe fds

    if(pipe(pfds) == -1)
    {
        perror("pipe");
        exit(1);
    }
    initscr();
    noecho();
    curs_set(FALSE);

    // set up initial windows
    getmaxyx(stdscr, parent_y, parent_x);

    WINDOW *chatbox = newwin(parent_y - sb_size, parent_x - users_width, 0, 0);
    WINDOW *sendbox = newwin(sb_size, parent_x - users_width, parent_y - sb_size, 0);
    WINDOW *userbox = newwin(parent_y, users_width, 0, parent_x - users_width);
    draw_borders(chatbox);
    draw_borders(sendbox);
    draw_borders(userbox);

    while(1)
    {
        getmaxyx(stdscr, new_y, new_x);

        if (new_y != parent_y || new_x != parent_x)
        {
            parent_x = new_x;
            parent_y = new_y;

            wresize(chatbox, new_y - sb_size, new_x - users_width);
            wresize(sendbox, sb_size, new_x - users_width);
            wresize(userbox, new_y, users_width);

            // chatbox win does not need to be moved, since its origin is at 0, 0
            mvwin(sendbox, new_y - sb_size, 0);
            mvwin(userbox, 0, new_x - users_width);

            wclear(stdscr);
            wclear(chatbox);
            wclear(sendbox);
            wclear(userbox);

            draw_borders(chatbox);
            draw_borders(sendbox);
            draw_borders(userbox);
        }

        // draw to our windows
        mvwprintw(chatbox, 1, 2, "ChatBox");
        mvwprintw(sendbox, 1, 2, "SendBox");
        mvwprintw(userbox, 1, 2, "UserBox");

        // refresh each window
        wrefresh(chatbox);
        wrefresh(sendbox);
        wrefresh(userbox);
    }
    endwin();
    return 0;
}

#include <stdio.h>

// FOR THE AUTH
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

// FOR THE BOX
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <panel.h>

#include "auth.h"

#define BOX_HEIGHT 10
#define BOX_LENGTH 50

int ROWS;
int COLS;

int main() {

    // Get Window Size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    ROWS = w.ws_row - 5; // -1 is temporary to ensure that the console line doe not push first line off screen
    COLS = w.ws_col;

    //TODO: MAKE UI WITH CURSES
    //IMPORT ALL THE STUFF FROM THE TEST DIR
    //AND ALSO DECIDE ON A UI LAYOUT
    // 
    // NEW UI STARTS HERE
    initscr();
    keypad(stdscr, TRUE);
    noecho();

    // MAIN WINDOW
    WINDOW *win = newwin(20, 100, 0, 0);
    box(win, 0, 0);
    refresh();
    wrefresh(win);

    int window_length = 100;
    int gap = 6;
    int input_length = (window_length - 3*gap) / 2;

    WINDOW *login_box = subwin(win, 3, input_length, 16, gap);
    box(login_box, 0, 0);
    mvwprintw(login_box, 0, 1, " USERNAME ");
    refresh();
    touchwin(win);
    wrefresh(login_box);

    WINDOW *password_box = subwin(win,3, input_length, 16, input_length + 2*gap);
    box(password_box, 0, 0);
    mvwprintw(password_box, 0, 1, " PASSWORD ");
    refresh();
    touchwin(win);
    wrefresh(password_box);

    char b;
    do {
        b = getch(); //just wait for now before shutting down UI
    } while(b != 'q');
    endwin();
    exit(0);

    // OLD UI FROM HERE DOWN

    char username[32];
    const char *usernamePoint = &username[0];
    char passwd[32];
    const char *passwdPoint = &passwd[0];

    // INSECURE AND ONLY HERE FOR TESTING PURPOSES
    // DO NOT USE THIS WITH REAL DETAILS
    while (0) {

        printf("Wagwan, whats your username: ");
        //scanf("%s", &username);
        printf("\nWhats your password Gng: ");
        //scanf("%s", &passwd);

        pid_t child_pid;

        bool loggedIn = login(usernamePoint, passwdPoint, &child_pid);
        if (loggedIn) {
            int returnStatus;
            waitpid(child_pid, &returnStatus, 0);
            printf("CHILD TERMINATED WITH STATUS: %d \n", returnStatus);
            logout();
        }
        else {
            printf("FAILED TO AUTH, RETRY\n");
        }

    }
    return 0;
}

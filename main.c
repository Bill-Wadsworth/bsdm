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

const char *logo = 
"${c1}         -/oyddmdhs+:. \n"
"     -o${c2}dNMMMMMMMMNNmhy+${c1}-`\n"
"   -y${c2}NMMMMMMMMMMMNNNmmdhy${c1}+-\n"
" `o${c2}mMMMMMMMMMMMMNmdmmmmddhhy${c1}/`\n"
" om${c2}MMMMMMMMMMMN${c1}hhyyyo${c2}hmdddhhhd${c1}o`\n"
".y${c2}dMMMMMMMMMMd${c1}hs++so/s${c2}mdddhhhhdm${c1}+`\n"
" oy${c2}hdmNMMMMMMMN${c1}dyooy${c2}dmddddhhhhyhN${c1}d.\n"
"  :o${c2}yhhdNNMMMMMMMNNNmmdddhhhhhyym${c1}Mh\n"
"    .:${c2}+sydNMMMMMNNNmmmdddhhhhhhmM${c1}my\n"
"       /m${c2}MMMMMMNNNmmmdddhhhhhmMNh${c1}s:\n"
"    `o${c2}NMMMMMMMNNNmmmddddhhdmMNhs${c1}+`\n"
"  `s${c2}NMMMMMMMMNNNmmmdddddmNMmhs${c1}/.\n"
" /N${c2}MMMMMMMMNNNNmmmdddmNMNdso${c1}:`\n"
"+M${c2}MMMMMMNNNNNmmmmdmNMNdso${c1}/-\n"
"yM${c2}MNNNNNNNmmmmmNNMmhs+/${c1}-`\n"
"/h${c2}MMNNNNNNNNMNdhs++/${c1}-`\n"
"`/${c2}ohdmmddhys+++/:${c1}.`\n"
"  `-//////:--.\n";

int ROWS;
int COLS;

void drawAsciiArt(WINDOW *win, int y, int x, char *string);
void mvwaddncolstr(WINDOW *win, int y, int x, char *string, int n);

void mvwaddncolstr(WINDOW *win, int y, int x, char *string, int n) {
    wmove(win, y, x);
    int count = 0;
    char c = 'i'; //init to whatever
    while (count < n && c != '\0') {
        c = string[count];
        if (n-count > 5 && c == '$' && string[count+1] == '{' && string[count+2] == 'c') {
            wattron(win, COLOR_PAIR(string[count+3]-'0'));
            count += 5;
        } else {
            waddch(win,c);
            count++;
        }
    }
}

void wdrawAsciiArt(WINDOW *win, int y, int x, char *string) {
    char *line = strchr(string, '\n');
    char *strPoint = string;
    int count = 0;
    while (line) {
        mvwaddncolstr(win, y+count, x, strPoint, line-strPoint);
        count++;
        strPoint = line + 1; //+1 to skip the new line character
        line = strchr(strPoint, '\n');
    }
}

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
    
    // NEED SOME COLOr
    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    refresh();

    //MOVE TO SOMEWHERE NICER
    int window_height = 22;
    int window_length = 100;
    int gap = 6;
    int input_length = (window_length - 3*gap) / 2;

    int begin_y = (ROWS-window_height)/2;
    int begin_x = (COLS-window_length)/2;

    // MAIN WINDOW
    WINDOW *win = newwin(window_height, window_length, begin_y, begin_x);
    box(win, 0, 0);
    wrefresh(win);

    //TEST DATA
    mvwprintw(win, 4, window_length/2, "SIGNING INTO LARRY");

    WINDOW *launch_box = subwin(win, 3, input_length, begin_y+7, begin_x+window_length/2);
    box(launch_box, 0, 0);
    mvwprintw(launch_box, 0, 1, " LAUNCH ");
    mvwprintw(launch_box, 1, 0, "<");
    mvwprintw(launch_box, 1, input_length-1, ">");
    touchwin(win);
    wrefresh(launch_box);

    WINDOW *login_box = subwin(win, 3, input_length, begin_y+12, begin_x+window_length/2);
    box(login_box, 0, 0);
    mvwprintw(login_box, 0, 1, " USERNAME ");
    touchwin(win);
    wrefresh(login_box);

    WINDOW *password_box = subwin(win, 3, input_length, begin_y+17, begin_x+window_length/2);
    box(password_box, 0, 0);
    mvwprintw(password_box, 0, 1, " PASSWORD ");
    touchwin(win);
    wrefresh(password_box);

    wdrawAsciiArt(win, 2, 6, logo);
    wrefresh(win);

    //POPULATE SOME EXAMPLE DATA
    mvwprintw(launch_box, 1, 3, "X SERVER");
    wrefresh(launch_box);
    mvwprintw(login_box, 1, 3, "soup");
    wrefresh(login_box);
    mvwprintw(password_box, 1, 3, "********");
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

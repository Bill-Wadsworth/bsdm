#include <stdio.h>

// FOR THE AUTH
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

// FOR THE BOX
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <panel.h>

#include "auth.h"

#define BOX_HEIGHT 10
#define BOX_LENGTH 50
#define INPUT_MAX_SIZE 32 

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

struct inputBox {
    WINDOW *win;
    char input[INPUT_MAX_SIZE];
    int inputSize;
    bool showChar; 
};

void drawAsciiArt(WINDOW *win, int y, int x, char *string);
void mvwaddncolstr(WINDOW *win, int y, int x, char *string, int n);
struct inputBox newInputWindow(WINDOW *win, int height, int length, int y, int x, char *name, bool showChar);
void writeCharToInputWindow(struct inputBox *box, char c);

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

struct inputBox newInputWindow(WINDOW *win, int height, int length, int y, int x, char *name, bool showChar) {
    WINDOW *input_box = subwin(win, height, length, y, x);
    box(input_box, 0, 0);
    mvwprintw(input_box, 0, 1, name);
    touchwin(win);
    wrefresh(input_box);
    char inputFeild[INPUT_MAX_SIZE];
    struct inputBox ret = {
        input_box,
        *inputFeild,
        0,
        showChar,
    };
    return ret;
}

void writeCharToInputWindow(struct inputBox *box, char c) {
    if (box->inputSize >= INPUT_MAX_SIZE-1) {
        return;
    }
    box->input[box->inputSize] = c;
    // always insert a null pointer afterwards
    // its just safer
    box->input[box->inputSize+1] = '\0';
    // if we should hide the entered character
    if (box->showChar == false) {
        //TODO: why is this always being run? 
        //c = '*';
    }
    // add 4 to account for some padding on the left hand size
    mvwaddch(box->win, 1, box->inputSize+4, c);
    box->inputSize++;
    wrefresh(box->win);
}

int main() {

    // Get Window Size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    ROWS = w.ws_row;
    COLS = w.ws_col;

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    
    // NEED SOME COLOr
    // This is only for the ascii art and this will need to change in the future for it to work with
    // the art stolen from neofetch, so ... thatll be fun to make
    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    refresh();

    //move to be export variables
    //However I fear what variable size ascci art will do, i did not check if they are consistent sizes
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


    struct inputBox login_box = newInputWindow(win, 3, input_length, begin_y+12, begin_x+window_length/2, " USERNAME ", true);

    struct inputBox password_box = newInputWindow(win, 3, input_length, begin_y+17, begin_x+window_length/2, " PASSWORD ", false);

    //TODO: Need to ensure that ascii art is either consistently sized in neofetch repo or add some code to center the ascii art here
    wdrawAsciiArt(win, 2, 6, logo);
    wrefresh(win);

    //POPULATE SOME EXAMPLE DATA
    mvwprintw(launch_box, 1, 3, "X SERVER");
    wrefresh(launch_box);
    mvwprintw(password_box.win, 1, 4, "********");
    wrefresh(password_box.win);


    char b;
    do {
        //just wait for now before shutting down UI
        b = getch(); 
        // test to see if it works
        writeCharToInputWindow(&login_box, b);
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

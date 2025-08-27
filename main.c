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

char usernameBuffer[INPUT_MAX_SIZE];
char passwordBuffer[INPUT_MAX_SIZE];

struct inputBox {
    WINDOW *win;
    char *input;
    int inputSize;
    bool showChar; 
};

void drawAsciiArt(WINDOW *win, int y, int x, char *string);
void mvwaddncolstr(WINDOW *win, int y, int x, char *string, int n);
void writeCharToInputWindow(struct inputBox *box, char c);
void removeCharFromInputWindow(struct inputBox *box);
int getUserCredentials();

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

void writeCharToInputWindow(struct inputBox *box, char c) {
    if (box->inputSize >= INPUT_MAX_SIZE-1) {
        return;
    }
    box->input[box->inputSize] = c;
    // always insert a null pointer afterwards
    // its just safer
    box->input[box->inputSize+1] = '\0';
    if (box->showChar == false) {
        c = '*';
    }
    // add 4 to account for some padding on the left hand size
    mvwaddch(box->win, 1, box->inputSize+4, c);
    box->inputSize++;
    wrefresh(box->win);
}

void removeCharFromInputWindow(struct inputBox *box){
    if (box->inputSize <=0 ) {
        return; //THE buffer is empty
    } 
    box->inputSize--;
    box->input[box->inputSize] = '\0';
    mvwaddch(box->win, 1, box->inputSize+4, ' ');
    wrefresh(box->win);
}

/* This function draws up the UI and fills username and password buffer with what the user has entered */
int getUserCredentials() {
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

    //TODO: ACTUALLY GET THE MACHINE NAME 
    mvwprintw(win, 4, window_length/2, "SIGNING INTO LARRY");

    WINDOW *launch_box = subwin(win, 3, input_length, begin_y+7, begin_x+window_length/2);
    box(launch_box, 0, 0);
    mvwprintw(launch_box, 0, 1, " LAUNCH ");
    mvwprintw(launch_box, 1, 0, "<");
    mvwprintw(launch_box, 1, input_length-1, ">");
    touchwin(win);
    wrefresh(launch_box);


    //struct inputBox username_box = newInputWindow(win, 3, input_length, begin_y+12, begin_x+window_length/2, " USERNAME ", true, usernameBuffer);
    WINDOW *username_input_box = subwin(win, 3, input_length, begin_y+12, begin_x+window_length/2);
    box(username_input_box, 0, 0);
    mvwprintw(username_input_box, 0, 1, " USERNAME ");
    touchwin(win);
    wrefresh(username_input_box);
    struct inputBox username_box = {
        username_input_box,
        usernameBuffer,
        0,
        true,
    };

    //struct inputBox password_box = newInputWindow(win, 3, input_length, begin_y+17, begin_x+window_length/2, " PASSWORD ", false, passwordBuffer);
    WINDOW *password_input_box = subwin(win, 3, input_length, begin_y+17, begin_x+window_length/2);
    box(password_input_box, 0, 0);
    mvwprintw(password_input_box, 0, 1, " PASSWORD ");
    touchwin(win);
    wrefresh(password_input_box);
    struct inputBox password_box = {
        password_input_box,
        passwordBuffer,
        0,
        false,
    };

    //TODO: Need to ensure that ascii art is either consistently sized in neofetch repo or add some code to center the ascii art here
    wdrawAsciiArt(win, 2, 6, logo);
    wrefresh(win);

    //POPULATE SOME EXAMPLE DATA
    mvwprintw(launch_box, 1, 3, "X SERVER");
    wrefresh(launch_box);

    int c;
    struct inputBox inputs[2] = {username_box, password_box};
    int selectedIndex = 0;
    // main input loop
    do {
        c = getch(); 
        
        switch(c){
            case '\n':
                selectedIndex++;
                break;
            case KEY_BACKSPACE:
                removeCharFromInputWindow(&inputs[selectedIndex]);
                break;
            case KEY_DL: 
                //delete key pressed so for right now with testing we want to halt the program
                return 1;
            default:
                writeCharToInputWindow(&inputs[selectedIndex], c);
        }
    } while(selectedIndex != 2);
    endwin();
    return 0;
}

int main() {
    // Get Window Size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    ROWS = w.ws_row;
    COLS = w.ws_col;

    while(true) {
        int creditStatus = getUserCredentials();

        if (creditStatus != 0) {
            printf("ERROR, got credit status: %d \n", creditStatus);
            exit(0);
        }

        printf("ENTERED USERNAME: %s, PASSWORD: %s \n", usernameBuffer, passwordBuffer);
        pid_t child_pid;
        bool loggedIn = login(usernameBuffer, passwordBuffer, &child_pid);
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

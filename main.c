#include <stdio.h>

// FOR THE AUTH
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

// FOR THE BOX
#include <stdlib.h>

#include "auth.h"

#define BOX_HEIGHT 10
#define BOX_LENGTH 20

struct screenBuffer {
    char *buffer;
    int rows;
    int cols;
};

int ROWS;
int COLS;

// only takes password length as the password should not be shown and want to minimise passing around plaintext password
void renderUI(const char* username, int passwordLength);
void writeBufferToScreen(struct screenBuffer buffer);
void writeSquareToBuffer(struct screenBuffer buffer, int x, int y, int width, int heigth);

void writeScreenBuffer(struct screenBuffer buffer) {
    //ensure last character is a terminator 
    buffer.buffer[buffer.rows*buffer.cols+1] = '\0';
    printf("%s", buffer.buffer);
}

void writeSquareToBuffer(struct screenBuffer buffer, int x, int y, int width, int heigth) {

    //char TOP_LEFT = "\u250C";
    //char TOP_RIGHT = "\u2510";
    //char BOTTOM_LEFT = "\u2514";
    //char BOTTOM_RIGHT = "\u2518";
    //char VERTICAL = "\u2502";
    //char HORIZONTAL = "\u2500";
}

void renderUI(const char* username, int passwordLength) {
    // initialise a screen buffer to write the UI to
    char* screenBuffer = (char*)malloc((ROWS*COLS+1)*sizeof(char));
    // Initialise the Buffer to just be spaces
    for (int i=0; i<ROWS*COLS; i++) {
        screenBuffer[i] = '.';
        if (i==18) screenBuffer[i] = 'U';
        if (i==0) screenBuffer[i] = 'o';
    }
    // DO NOT OVERWRITE THIS VALUE
    screenBuffer[ROWS*COLS+1] = '\0';
    int verticalPadding = (ROWS - BOX_HEIGHT)/2;
    int horizontalPadding = (COLS - BOX_LENGTH)/2;

    writeBufferToScreen(screenBuffer);
    free(screenBuffer);
}



int main() {

    // Get Window Size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    ROWS = w.ws_row - 1; // -1 is temporary to ensure that the console line doe not push first line off screen
    COLS = w.ws_col;

    printf("The Terminal has %d Rows and %d Cols \n", ROWS, COLS);

    for (int i=0; i<COLS; i++) {
        printf("i");
    }

    char username[32];
    const char *usernamePoint = &username[0];
    char passwd[32];
    const char *passwdPoint = &passwd[0];

    renderUI(usernamePoint, 8);
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

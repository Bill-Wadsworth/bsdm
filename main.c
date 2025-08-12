#include <stdio.h>

// FOR THE AUTH
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

// FOR THE BOX
#include <stdlib.h>
#include <string.h>

#include "auth.h"

#define BOX_HEIGHT 10
#define BOX_LENGTH 50

struct screenBuffer {
    char *buffer;
    int rows;
    int cols;
};

int ROWS;
int COLS;

// only takes password length as the password should not be shown and want to minimise passing around plaintext password
void renderUI(const char* username, int passwordLength);
void writeBufferToScreen(struct screenBuffer *buffer);
void writeRectToBuffer(struct screenBuffer *buffer, int x, int y, int width, int heigth);
void writeTextToBuffer(struct screenBuffer *buffer, int x, int y, char *text);

void writeBufferToScreen(struct screenBuffer *buffer) {
    //ensure last character is a terminator 
    buffer->buffer[buffer->rows*buffer->cols+1] = '\0';
    printf("%s", buffer->buffer);
}

void writeRectToBuffer(struct screenBuffer *buffer, int x, int y, int width, int height) {

    //ensure its in bounds
    if (x+width > buffer->cols) return;
    if (y+height > buffer->rows) return;
    if (width < 1 || height < 1 || x < 1 || y < 1) return;

    //TOP LINE!
    int startIndex = x + y * buffer->cols;
    int endIndex = x + y * buffer->cols + width;
    for (int i=startIndex; i<=endIndex; i++) {
        buffer->buffer[i] = '#';
    }
    //LEFT LINE
    startIndex = x + y * buffer->cols;
    endIndex = x + (y + height)*buffer->cols;
    for (int i=startIndex; i<=endIndex; i+=buffer->cols) {
        buffer->buffer[i] = '#';
    }
    //BOTTOM LINE
    startIndex = x + (y+height) * buffer->cols;
    endIndex = x + width + (y+height) * buffer->cols;
    for (int i=startIndex; i<=endIndex; i++) {
        buffer->buffer[i] = '#';
    }
    //RIGHT LINE
    startIndex = x + width + y * buffer->cols;
    endIndex = x + width + (y+height) * buffer->cols;
    for (int i=startIndex; i<=endIndex; i+=buffer->cols) {
        buffer->buffer[i] = '#';
    }
    
}

void writeTextToBuffer(struct screenBuffer *buffer, int x, int y, char *text) {

    int len = strlen(text);

    //check the text will stay within the bounds of the screen
    if ( x+len > buffer->cols ) return;
    if ( y > buffer->rows ) return;
    if (x < 1 || y < 1) return;

    int startIndex = x + y * buffer->cols;
    for (int i=0; i<len; i++) {
        buffer->buffer[startIndex+i] = text[i];
    }
}

void renderUI(const char* username, int passwordLength) {
    // initialise a screen buffer to write the UI to
    char* screenMem = (char*)malloc((ROWS*COLS+1)*sizeof(char));
    // Initialise the Buffer to just be spaces
    for (int i=0; i<ROWS*COLS; i++) {
        screenMem[i] = ' ';
    }

    struct screenBuffer buffer = {
        screenMem,
        ROWS,
        COLS,
    };

    // DO NOT OVERWRITE THIS VALUE
    screenMem[ROWS*COLS+1] = '\0';
    int verticalPadding = (ROWS - BOX_HEIGHT)/2;
    int horizontalPadding = (COLS - BOX_LENGTH)/2;

    writeRectToBuffer(&buffer, horizontalPadding, verticalPadding, BOX_LENGTH, BOX_HEIGHT);
    writeTextToBuffer(&buffer, horizontalPadding+2, verticalPadding+2, "USERNAME");
    writeTextToBuffer(&buffer, horizontalPadding+8, verticalPadding+4, "_______________");
    writeTextToBuffer(&buffer, horizontalPadding+2, verticalPadding+6, "PASSWORD");
    writeTextToBuffer(&buffer, horizontalPadding+8, verticalPadding+8, "_______________");
    writeBufferToScreen(&buffer);
    free(screenMem);
}



int main() {

    // Get Window Size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    ROWS = w.ws_row - 5; // -1 is temporary to ensure that the console line doe not push first line off screen
    COLS = w.ws_col;

    printf("The Terminal has %d Rows and %d Cols \n", ROWS, COLS);

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

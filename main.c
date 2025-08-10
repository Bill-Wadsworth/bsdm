#include <stdio.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include "auth.h"

int ROWS;
int COLS;

void renderUI() {
}

int main() {

    // Get Window Size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    ROWS = w.ws_row;
    COLS = w.ws_col;

    printf("The Terminal has %d Rows and %d Cols \n", ROWS, COLS);

    for (int i=0; i<COLS; i++) {
        printf("i");
    }

    char username[32];
    const char *usernamePoint = &username[0];
    char passwd[32];
    const char *passwdPoint = &passwd[0];

    // INSECURE AND ONLY HERE FOR TESTING PURPOSES
    // DO NOT USE THIS WITH REAL DETAILS
    printf("Wagwan, whats your username: ");
    scanf("%s", &username);
    printf("\nWhats your password Gng: ");
    scanf("%s", &passwd);

    printf("%s, %s", username, passwd);
    
    login(usernamePoint, passwdPoint, 0);
    return 0;
}

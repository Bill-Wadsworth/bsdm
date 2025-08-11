#include <stdio.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

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
    while (1) {

        printf("Wagwan, whats your username: ");
        scanf("%s", &username);
        printf("\nWhats your password Gng: ");
        scanf("%s", &passwd);

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

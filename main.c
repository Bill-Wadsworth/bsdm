#include <stdio.h>

#include <sys/ioctl.h>
#include <unistd.h>

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
    return 0;
}

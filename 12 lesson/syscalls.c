#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void printer(){
    struct winsize window;
    ioctl(0, TIOCGWINSZ, &window);
    write(STDOUT_FILENO, "\033[H", 4);
    write(STDOUT_FILENO, "\033[J", 4);

    char position_set[20];
    sprintf(position_set, "\033[%d;%dH", window.ws_row / 2, (window.ws_col / 2) - 6);

    write(STDOUT_FILENO, position_set, sizeof(position_set));

    char text[13] = "Hello world!";
    write(STDOUT_FILENO, text, sizeof(text));
}

void reprint(){
    printer();
}

void end(){
    write(STDOUT_FILENO, "\033[H", 4);
    write(STDOUT_FILENO, "\033[J", 4);
    write(STDOUT_FILENO, "Goodbye\n", 12);
    _exit(0);
}

int main() {
    signal(SIGWINCH, reprint);
    signal(SIGINT, end);

    printer();
    
    while(1){
        pause();
    }
    return 0;
}
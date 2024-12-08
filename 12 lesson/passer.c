#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <inttypes.h>

void sender(){

}

void receiver(){

}

int main() {
    struct sigaction test;

    pthread_t send, receive;

    pthread_create(&send, NULL, sender, NULL);
    pthread_create(&receive, NULL, receiver, NULL);

    pthread_join(send, NULL);
    pthread_join(send, NULL);
    
    return 0;
}
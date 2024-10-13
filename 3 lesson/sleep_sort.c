#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
    int p, s;

    for(int i = 1; i < argc; i++){
        p = fork();

        if(p == 0){
            usleep(atoi(argv[i]) * 10000);
            printf("%d ", atoi(argv[i]));
            exit(0);
        }
    }

    for(int i = 1; i < argc; i++) wait(&s);
}
#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <stdlib.h>

int main() {
    pid_t idp = getpid();
	printf("parent pid: %i\n", idp);

    int p = 1;
    for(int i = 1; i <= 6; i++){
        if(p == 0){
            pid_t id = getpid();
            pid_t idp = getppid();
            printf("child pid: %i, parent pid: %i\n", id, idp);
            exit(0);
            //sleep(1);
        }
        else{
            p = fork();
            sleep(1);
        }
    }

    /* Easier way
    for(...){
        fork
    }
    */
}
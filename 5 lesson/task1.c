#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int fd[2];

    int ifPipeWorked = pipe(fd);
    if(ifPipeWorked < 0){
        perror("read failed");
        exit(1);
    }

    int p = fork();

    if(p == 0){
        sleep(1);
        char text[100] = "Hello world\n";

        int bytesWrite = write(fd[1], text, strlen(text));
        if(bytesWrite < 0){
            perror("write failed");
            exit(1);
        }
    }
    else{
        char buffer[100];
        
		int data = read(fd[0], buffer, 100);
        if(data < 0){
            perror("read failed");
            exit(1);
        }

		write(1, buffer, data);
    }
    return 0;
}
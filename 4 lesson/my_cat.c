#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void ReadAndWWrite(int fd){
    char buffer[1024];
    int bytesWrite = 0;

    int bytesRead = read(fd, buffer, sizeof(buffer));
    if(bytesRead < 0){
        perror("read failed");
        exit(1);
    }

    while(bytesWrite < bytesRead){
        bytesWrite += write(STDOUT_FILENO, buffer, bytesRead);

        if(bytesWrite < 0){
            perror("write failed");
            exit(1);
        }
    }
    return;
}


int main(int argc, char * argv[]) {
    int fd;

    if (argc == 1){
        fd = STDIN_FILENO;
        ReadAndWWrite(fd);
    }

    else{
        for(int i = 0; i < argc - 1; i++) {
            fd = open(argv[i + 1], O_RDWR);

            if(fd < 0){
                perror("open failed");
                exit(1);
            }
            ReadAndWWrite(fd);
        }
    }
    return 0;
}
#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char * argv[]) {
    struct timespec time1, time2;
    int fd[2];
    int bytes = 0, words = 0, lines = 0;

    int ifPipeWorked = pipe(fd);
    if(ifPipeWorked < 0){
        perror("read failed");
        exit(1);
    }

    int p = fork();

    clock_gettime(CLOCK_MONOTONIC_RAW, &time1);

    if(p == 0){
        close(1);
        dup(fd[1]);
        close(fd[1]);
        execvp(argv[1], argv + 1);
    }

    else{
        close(fd[1]);
        int bytesRead = 1;
        char buffer[1024];

        while (bytesRead > 0){
            bytesRead = read(fd[0], buffer, 100);
            if (bytesRead == 0) break;
            if(bytesRead < 0){
                perror("read failed");
                exit(1);
            }
            
            const char* it = buffer;
            do{
                switch(*it){
                    case '\n': lines += 1;
                    case ' ': words += 1;
                    case '\t': words += 1;
                }
            } while(*it++);
            bytes += bytesRead;
        }
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &time2);

    int sec = time2.tv_sec - time1.tv_sec;
    double msec =((double)time2.tv_nsec - (double)time1.tv_nsec) / 1000000;
    if (msec < 0){
        sec -= 1;
        msec = 1000 + msec;
    }

    printf("Время выполнения программы %s занимает %d секунд и %.0f миллисекунд\n", argv[1], sec, msec);
    printf("Слов: %d Линий: %d Байт: %d\n", words, lines, bytes);
    return 0;
}
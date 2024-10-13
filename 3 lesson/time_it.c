#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char * argv[]) {
    int p, s;
    struct timespec time1, time2;

    p = fork();
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &time1);
    
    if(p == 0)
        execvp(argv[1], argv + 1);

    wait(&s);

    clock_gettime(CLOCK_MONOTONIC_RAW, &time2);

    int sec = time2.tv_sec - time1.tv_sec;
    double msec =((double)time2.tv_nsec - (double)time1.tv_nsec) / 1000000;

    printf("Время выполнения программы %s занимает %d секунд и %.2f миллисекунд\n", argv[1], sec, msec);
    return 0;
}
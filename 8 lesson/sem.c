#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

int main() {
    int p, s;
    int idsem = semget(IPC_PRIVATE, 1, IPC_CREAT|0700); // get semaphore

    int idshm = shmget(IPC_PRIVATE, 1024, IPC_CREAT|0700);
    char* pointer = shmat(idshm, NULL, 0);

    p = fork();
    if(p == 0){
        struct sembuf* ops = {0, -1, 0};
        semop(idsem, ops, 1);
        printf("%s\n", pointer);
        exit(1);
    }
    else{
        strcpy(pointer, "Hello world");
        struct sembuf* ops = {0, +1, 0};
        semop(idsem, ops, 1);
        exit(0);
    }
   
    wait(&s);
    shmdt(pointer);
    shmctl(idshm, IPC_RMID, 0);
    semctl(idsem, IPC_RMID, 0);
    return 0;
}
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
#include <sys/msg.h>
#include <string.h>
#include <semaphore.h>

#define BUFFER_SIZE 100

sem_t  *empty, *full;

struct msgbuf {
    long mtype;
};

int main() {
    empty = sem_open("/empty_sem", O_CREAT, 0644, BUFFER_SIZE);
    if (empty == SEM_FAILED) {
        perror("Failed to open semphore for empty");
        exit(-1);
    }

    full = sem_open("/full_sem", O_CREAT, 0644, 0);
    if (full == SEM_FAILED) {
        sem_close(empty);
        perror("Failed to open semphore for full");
        exit(-1);
    }
    //sem_post(full);
    //int idshm = shmget(IPC_PRIVATE, 1024, IPC_CREAT|0700);
    //char* pointer = shmat(idshm, NULL, 0);

    struct msgbuf buf;

    int p = fork();
    if(p == 0){
        // pcat

    }
    else{
        // ccat

        exit(0);
    }
    

}


//also worked
/*
    if(p != 0){ //new
        strcpy(pointer, "Hello world");
        buf.mtype = 3;
        msgsnd(id, &buf, 0, 0);
        exit(0);
    }
    else{
        msgrcv(id, &buf, 0, 3, 0);
        printf("%s\n", pointer);
    }
*/
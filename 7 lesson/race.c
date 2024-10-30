#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>


struct msgbuf {
    long mtype;
    //int number;
};


void judje(int runners, int id) {
    struct msgbuf buf;
    int arrived_number = 0;
    while(arrived_number != runners){
        int msg_error = msgrcv(id, &buf, 0, 0, 0);
        if (msg_error < 0){
            perror("recievce judje");
            exit(1);
        }
        arrived_number += 1;
    }
    printf("runners registered\n");

    buf.mtype = 1;
    int msg_sent = msgsnd(id, &buf, 0, 0);
    if (msg_sent < 0){
        perror("sent judje");
        exit(1);
    }
    printf("judje passed the stick\n");

    int msg_error = msgrcv(id, &buf, 0, runners + 1, 0);
    if (msg_error < 0){
        perror("recievce judje");
        exit(1);
    }
    printf("judje got the stick\n");
}


void runner(int i, int id, int runners) {
    printf("runner %d arrived\n", i);
    struct msgbuf buf = {100};
    int msg_sent = msgsnd(id, &buf, 0, 0);
    if (msg_sent < 0){
        perror("sent runner");
        exit(1);
    }
    
    int msg_error = msgrcv(id, &buf, 0, i, 0);
    if (msg_error < 0){
        perror("recievce runner");
        exit(1);
    }
    printf("runner %ld received stick\n", buf.mtype);

    buf.mtype = i+1;
    msg_sent = msgsnd(id, &buf, 0, 0);
    if (msg_sent < 0){
        perror("sent runner");
        exit(1);
    }

    if(i < runners){
        printf("runner %ld passed stick to runner %ld \n", buf.mtype - 1, buf.mtype);
    }
    else{
        printf("runner %ld passed stick to judje\n", buf.mtype - 1);
    }
}


int main() {
    int p, s;
    int n = 3;
    int runners = n;
    int id = msgget(IPC_PRIVATE, IPC_CREAT|0700);

    p = fork();
    if(p == 0){ //judje
        judje(runners, id);
        exit(1);
    }
    else{ //runners
        for(int i = 1; i <= runners; i++){
            int p = fork();
            if(p == 0){
                runner(i, id, runners);
                exit(1);
            }
        }
    }

    for(int i = 0; i < runners + 1; i++) wait(&s); 

    msgctl(id, IPC_RMID, 0);
    return 0;
}
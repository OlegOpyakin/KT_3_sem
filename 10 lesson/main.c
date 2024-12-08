#include <pthread.h>
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

struct a{
    int count;
    pthread_mutex_t mut;
};

void * func1(void * p){
    sleep(1);
    printf("i am a func\n");
    return p;
}

void * func2(void * p_){
    int * p = (int *)p_;
    sleep(1);
    *p = *p + 1;
    return NULL;
}

void * func3(void * st_){
    struct a* st = (struct a*)st_;

    pthread_mutex_lock(&st->mut);
    st->count = st->count + 1;
    pthread_mutex_unlock(&st->mut);
    
    return NULL;
}

int main() {
    pthread_t tid[1000];
    //pthread_mutex_t mut;
    //int count = 0;
    struct a st;

    st.count = 0;

    pthread_mutex_init(&st.mut, NULL);

    for(int i = 0; i < 1000; i++)
        pthread_create(&tid[i], NULL, func3, &st);
    
    for(int i = 0; i < 1000; i++)
        pthread_join(tid[i], NULL);

    printf("count = %d\n", st.count);
    pthread_mutex_destroy(&st.mut);
    return 0;
}
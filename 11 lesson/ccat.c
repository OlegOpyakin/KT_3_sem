#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 4096

int main(int argc, char * argv[]) {

    sem_t *empty, *full;
    FILE* output;

    output = fopen("output.txt", "w");
    if(output == NULL){
        perror("Output error");
        exit(-1);
    }

    empty = sem_open("/empty_sem", O_CREAT, 0777, 1);
    if (empty == SEM_FAILED) {
        sem_close(empty);
        perror("Failed to open semphore for empty");
        exit(-1);
    }

    full = sem_open("/full_sem", O_CREAT, 0777, 0);
    if (full == SEM_FAILED) {
        sem_close(full);
        perror("Failed to open semphore for full");
        exit(-1);
    }

    shm_unlink("/malex.hello03.01");
	int fd = shm_open("/malex.hello03.01", O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
        perror("shm open: ");
        exit(-1);
    }

    int err = ftruncate(fd, BUF_SIZE + sizeof(long int));
    if (err < 0) {
        perror("ftruncate: ");
        exit(-1);
    }

    char* ptr = (char*) mmap(NULL, BUF_SIZE + sizeof(long int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    *((long int*)(ptr + BUF_SIZE)) = 1;
    while(*((long int*)(ptr + BUF_SIZE)) > 0)
    {  
        if((sem_wait(full)) < 0)
            perror("full_sem wait failed");

        fwrite(ptr, 1, *((long int*)(ptr + BUF_SIZE)), output);

        if((sem_post(empty)) < 0)
            perror("empty_sem post failed");
    }

    printf("ccat finished\n");

    sem_unlink("/empty_sem");
    sem_unlink("/full_sem");

    munmap(ptr, BUF_SIZE + sizeof(long int));
    shm_unlink("/malex.hello03.01");
    close(fd);
	return 0;
}
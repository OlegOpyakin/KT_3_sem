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
    FILE* input;

    input = fopen("text.txt", "r");
    if(input == NULL){
        perror("Input error");
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

    char* ptr = (char*) mmap(NULL, BUF_SIZE + sizeof(long int), PROT_WRITE, MAP_SHARED, fd, 0);

    *((long int*)(ptr + BUF_SIZE)) = 1;
    while(*((long int*)(ptr + BUF_SIZE)) > 0)
    {
        if((sem_wait(empty)) < 0)
            perror("empty_sem wait failed");
        for(int i = 0; i < BUF_SIZE; i++)
            ptr[i] = 0;
        
        *((long int*)(ptr + BUF_SIZE)) = (fread(ptr, 1, BUF_SIZE, input));

        if((sem_post(full)) < 0)
            perror("full_sem post failed");
    }

    printf("pcat finished\n");

	return 0;
}
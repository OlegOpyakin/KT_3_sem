#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

const size_t BUFFER_SIZE = 1024;
const char shm_filename[]   = "/pccat_shm";
const char full_filename[]  = "/pccat_full_sem";
const char emty_filename[]  = "/pccat_empty_sem";

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Incorrect input");
        return 0;    
    }

    sem_t *full = sem_open(full_filename, O_CREAT, 0777, 0);
    if (full == SEM_FAILED) {
        perror("full sem_open failed");
        return 0;
    }

    sem_t *empty = sem_open(emty_filename, O_CREAT, 0777, 1);
    if (empty == SEM_FAILED) {
        perror("empty sem_open failed");
        return 0;
    }

    int file = open(argv[1], O_RDONLY);
    if (file < 0) {
        perror("file open failed");
        return 0;
    }

    int fd = shm_open(shm_filename, O_CREAT | O_RDWR, 0777);
    if (fd < 0 && errno != EEXIST) {
        perror("shm_open failed");
        close(file);
        return 0;
    } else if (fd < 0 && errno == EEXIST) {
        fd = shm_open(shm_filename, O_RDWR, 0777);
        if (fd == -1) {
            perror("shm_open failed (second attempt)");
            close(file);
            return 0;
        }
    }

    if (ftruncate(fd, BUFFER_SIZE + 1) < 0 && errno != EINVAL) {
        perror("ftruncate failed");
        close(file);
        return 0;
    }

    char *shmp = (char*) mmap(NULL, BUFFER_SIZE + 1, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        perror("mmap failed");
        close(file);
        return 0;
    }
 

	while (1) {
        sem_wait(empty);
        int rd = read(file, shmp, BUFFER_SIZE);
        if (rd < 0) {
            perror("Read failed");
            break;
        }
        else if (rd == 0) {
            shmp[BUFFER_SIZE] = 'f';
            sem_post(full);
            break;
        }
        sem_post(full);
	}
    close(file);
    munmap(shmp, BUFFER_SIZE);
    shm_unlink(shm_filename);
    sem_unlink(full_filename);
    sem_unlink(emty_filename);
}
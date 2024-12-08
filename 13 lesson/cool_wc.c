#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <poll.h>

const int BUFFER_SIZE = 1024;
const double DEGREE = 1000000000.0;

struct buf {
    int fd[2];
    char buffer[BUFFER_SIZE];
	int line, word, byte;
	int rt, word_flag;
};

void calculate_words(struct buf *b) {
	for (int i = 0; i < b->rt; i++) {
		if ((b->buffer)[i] == '\n') (b->line)++;

		if ((b->buffer)[i] == '\n' || (b->buffer)[i] == ' ' || (b->buffer)[i] == '\t') b->word_flag = 1;
		else if (b->word_flag) {
			(b->word)++;
			b->word_flag = 0;
		}
	}
}

int main(int argc, char* argv[]) {
    struct buf out = {.line = 0, .word = 0, .byte = 0, .word_flag = 1};
    struct buf err = {.line = 0, .word = 0, .byte = 0, .word_flag = 1};

    if(pipe(out.fd) < 0){
        perror("pipe out");
        exit(1);
    }

    if(pipe(err.fd) < 0){
        perror("pipe out");
        exit(1);
    }

    struct pollfd *pfds = (struct pollfd *) calloc(2, sizeof(struct pollfd));

    pfds[0].fd = out.fd[0]; 
    pfds[0].events = POLLIN;

    pfds[1].fd = err.fd[0]; 
    pfds[1].events = POLLIN;

	struct timespec tp1 = {0, 0};
	clock_gettime(CLOCK_MONOTONIC, &tp1);

	if (fork() == 0) {
        close(out.fd[0]); 
        close(err.fd[0]);

        if(dup2(out.fd[1], 1) < 0){
            perror("dup2 out");
            exit(1);
        }

        if(dup2(err.fd[1], 1) < 0){
            perror("dup2 err");
            exit(1);
        }

		execvp(argv[1], argv + 1);
		return 0;
	}

    close(out.fd[1]); 
    close(err.fd[1]);
    
    int num_open_pfds = 2;
    while (num_open_pfds > 0) {
        poll(pfds, 2, -1);
        if (pfds[0].revents) {
            if (pfds[0].revents & POLLIN) {
                out.rt = read(out.fd[0], out.buffer, BUFFER_SIZE);
                calculate_words(&out);
                out.byte += out.rt;
            }
            else {
                num_open_pfds--; 
                pfds[0].fd = -1;
            }
        }

        if (pfds[1].revents) {
            if (pfds[1].revents & POLLIN) {
                err.rt = read(err.fd[0], err.buffer, BUFFER_SIZE);
                calculate_words(&err);
                err.byte += err.rt;
            }
            else {
                num_open_pfds--; 
                pfds[1].fd = -1;
            }
        }
    }

	struct timespec tp2 = {0, 0};
	clock_gettime(CLOCK_MONOTONIC, &tp2);

	double sec = tp2.tv_sec - tp1.tv_sec + (tp2.tv_nsec - tp1.tv_nsec) / DEGREE;
	printf("time is %.3f sec.\n", sec);
	printf("stdout: %6d\t%6d\t%6d\n", out.line, out.word, out.byte);
    printf("stderr: %6d\t%6d\t%6d\n", err.line, err.word, err.byte);

    free(pfds);
    return 0;
}


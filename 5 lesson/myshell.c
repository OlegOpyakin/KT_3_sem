#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <uuid/uuid.h>


struct buffer{
    size_t size_;
    char* data_;

    size_t length_;
    size_t progs_number_;
    int args_in_program_[1024];
    char* data_mas_[25][25];
};


void LineParse(struct buffer* Buffer){
    char *saveptr1, *saveptr2;
    char *lexema1, *lexema2;

    int iterator1 = 0;
    int iterator2 = 0;
    
    Buffer->progs_number_ = 1;
    for(int i = 0; i < Buffer->length_; i++){ // calculate program number to use it later
            if (Buffer->data_[i] == '|') Buffer->progs_number_++;
    }
    
    while(1){
        lexema1 = strtok_r(Buffer->data_, "|\n", &saveptr1); // divide line by "|" symbols 
                                                             // into programs and arguments
        if(lexema1 == NULL)
            break;

        //printf("lexema1: %s\n", lexema1);
        Buffer->args_in_program_[iterator1] = 0;

        while(1){
            lexema2 = strtok_r(lexema1, " ", &saveptr2); // divide piece of line
                                                         // into progmam and its args
            if(lexema2 == NULL) 
                break;
            Buffer->args_in_program_[iterator1] += 1; 
            Buffer->data_mas_[iterator1][iterator2] = lexema2;

            //printf("lexema2 for iterator [%d][%d]: %s\n", iterator1, iterator2, Buffer->data_mas[iterator1][iterator2]);

            iterator2 += 1;
            
            lexema1 = NULL;
        }
        //printf("args in program %d: %d\n", iterator1, Buffer->args_in_program[iterator1]);
        iterator1 += 1;
        iterator2 = 0;

        Buffer->data_ = NULL;
    }
    for(size_t i = 0; i < Buffer->progs_number_; i++){
            Buffer->data_mas_[i][Buffer->args_in_program_[i]] = NULL;
    }
}


void ExecFiles(struct buffer* Buffer) {
	int* fd = (int*) calloc(Buffer->progs_number_ * 2, sizeof(int)); // allocate memory for file descriptors
    int s;

	fd[0] = 0;                             // set first prog for input
	fd[Buffer->progs_number_ * 2 - 1] = 1; // set last prog for output

	for (int i = 0; i < Buffer->progs_number_; i++) {
		if (i < Buffer->progs_number_ - 1) { // change all our needed file descriptors
			int tmp[2];
			int if_pipe_worked = pipe(tmp);

            if(if_pipe_worked < 0){
                perror("pipe work failed");
                exit(EXIT_FAILURE);
            }

			fd[2 * i + 1] = tmp[1];
			fd[2 * i + 2] = tmp[0];
		}
		if (fork() == 0) { // our child that executes the program
			dup2(fd[2 * i], 0); // change in and out file descriptors for those we want
			dup2(fd[2 * i + 1], 1);
            int if_execvp_worked = execvp(Buffer->data_mas_[i][0], Buffer->data_mas_[i]);
            if(if_execvp_worked < 0){
                perror("execvp work failed");
                exit(EXIT_FAILURE);
            }
			exit(EXIT_SUCCESS);
		}
		if (i > 0) close(fd[2 * i]);
		if (i < Buffer->progs_number_ - 1) close(fd[2 * i + 1]);
	}
	for (int i = 0; i < Buffer->progs_number_; i++) wait(&s); // wait our children to finish their work
	free(fd); // free fd memory
}


int main(){
    struct buffer Buffer;
    struct passwd* usr;

    uid_t cur_uid = getuid();
    usr = getpwuid(cur_uid);

    Buffer.size_ = 1024;
    Buffer.data_ = (char*)malloc(Buffer.size_);

    while(1){
        write(1, usr->pw_name, strlen(usr->pw_name));
        write(1, " ~ ", 3);
        write(1, "% ", 2);

        Buffer.length_ = getline(&Buffer.data_, &Buffer.size_, stdin); // get data from the user
        if(Buffer.length_ == -1){
            perror("Getline error");
            exit(EXIT_FAILURE);
        }

        if(strncmp(Buffer.data_, "exit", 4) == 0){ // exit check
            break;
        }

        LineParse(&Buffer);
        ExecFiles(&Buffer);
    }
    free(Buffer.data_);
    return 0;
}
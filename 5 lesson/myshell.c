#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

struct buffer{
    size_t size_;
    char* data_;
    size_t exit_flag_;
    size_t length_;
    size_t progs_number_;
    char*** progs_and_args_;
};

void LineParse(struct buffer* Buffer){
    char *saveptr1, *saveptr2;
    char *lexema1, *lexema2;
    
    while(1){
        lexema1 = strtok_r(Buffer->data_, "|\n", &saveptr1);
        if(lexema1 == NULL) 
            break;

        printf("lexema1: %s\n", lexema1);

        while(1){
            lexema2 = strtok_r(lexema1, " ", &saveptr2);

            if(lexema2 == NULL) 
                break;

            printf("lexema2: %s\n", lexema2);
            
            lexema1 = NULL;
        }

        Buffer->data_ = NULL;
    }
}

int main(){
    struct buffer Buffer;
    Buffer.exit_flag_ = 0;
    Buffer.size_ = 1024;
    Buffer.data_ = (char*)malloc(Buffer.size_);

    while(Buffer.exit_flag_ == 0){
        write(1, "% ", 2);
        Buffer.length_ = getline(&Buffer.data_, &Buffer.size_, stdin);

        if(Buffer.length_ == -1){
            perror("Getline error");
            exit(1);
        }

        if(strncmp(Buffer.data_, "exit", 4) == 0){
            break;
        }
        LineParse(&Buffer);
        //ExecFiles(&Buffer);
    }
    free(Buffer.data_);
    return 0;
}
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
};

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

        if(strncmp(Buffer.data_, "q", 1) == 0){
            break;
        }
        //LineParse(&Buffer);
        //ExecFiles(&Buffer);
    }
    free(Buffer.data_);
    return 0;
}
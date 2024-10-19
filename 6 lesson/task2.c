#include <dirent.h>
#include <stdio.h>
#include <errno.h>

void parse(char* argv){
    DIR* dp = opendir(argv);
    if(dp == NULL){
        perror("Directory open error");
        return;
    }

    struct dirent* read;
    char line[1024];

    while (read = readdir(dp)) {
        if(read->d_name[0] != '.'){
            printf("#%llu \t%hhu \t%s\n", (long long int)read->d_ino, read->d_type, read->d_name);
            sprintf(line, "%s/%s", argv, read->d_name);
            if(read->d_type == DT_DIR){
                parse(line);
            }
        }
    }
    if(errno){
        perror("Directory entry error");
    }
    closedir(dp);
}


int main(int argc, char * argv[]) {
    parse(argv[1]);
    return 0;
}
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
    DIR* dp = opendir(argv[1]);

    if(dp == NULL){
        perror("open failed");
        exit(1);
    }

    struct dirent* read;

    while ((read = readdir(dp)) != NULL) {
        printf("#%llu \t%hhu \t%s\n", (long long int)read->d_ino, read->d_type, read->d_name);
    }
    
    closedir(dp);
    return 0;
}
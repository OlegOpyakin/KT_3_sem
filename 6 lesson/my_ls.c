#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <getopt.h>
#include <pwd.h>
#include <grp.h>


#define S_IFDIR  0040000  /* directory */
#define S_IFREG  0100000  /* regular */

#define S_IRUSR 0000400    /* R for owner */
#define S_IWUSR 0000200    /* W for owner */
#define S_IXUSR 0000100    /* X for owner */

#define S_IRGRP 0000040    /* R for group */
#define S_IWGRP 0000020    /* W for group */
#define S_IXGRP 0000010    /* X for group */

#define S_IROTH 0000004    /* R for other */
#define S_IWOTH 0000002    /* W for other */
#define S_IXOTH 0000001    /* X for other */


struct CompileFlags{ // struct to keep our compiling flags
    int flag_R_, flag_l_, flag_i_, flag_a_, flag_d_;
};


void PrintMode(struct stat* st){
    if(st->st_mode & S_IFDIR) printf("d");  // directory
    if(st->st_mode & S_IFREG) printf("-");  // regular
    
    printf( (st->st_mode & S_IRUSR) ? "r" : "-");
    printf( (st->st_mode & S_IWUSR) ? "w" : "-");
    printf( (st->st_mode & S_IXUSR) ? "x" : "-");

    printf( (st->st_mode & S_IRGRP) ? "r" : "-");
    printf( (st->st_mode & S_IWGRP) ? "w" : "-");
    printf( (st->st_mode & S_IXGRP) ? "x" : "-");

    printf( (st->st_mode & S_IROTH) ? "r" : "-");
    printf( (st->st_mode & S_IWOTH) ? "w" : "-");
    printf( (st->st_mode & S_IXOTH) ? "x" : "-");
}


void PrintInode(struct dirent* read, struct CompileFlags * compile_flags){
    if(compile_flags->flag_i_ == 1){ // info
        printf("%llu ", (long long int)read->d_ino); // print inode number
    }
}


void PrintLongListing(struct stat *st, struct CompileFlags * compile_flags){
    if(compile_flags->flag_l_ == 1){ // long listing
        PrintMode(st); // print our mode

        printf(" %lld ", (long long int)st->st_nlink); // number of hard links

        printf("%s ", getpwuid(st->st_uid)->pw_name); // user name

        printf("%s ", getgrgid(st->st_gid)->gr_name); // group name

        char buff[100];
        strftime(buff, sizeof buff, "%D %T  ", gmtime(&st->st_mtimespec.tv_sec));
        printf("%s ", buff); // print modification time
    }
}


void PrintName(struct dirent* read, struct CompileFlags * compile_flags){
    printf("%s", read->d_name); // file name
    printf( (compile_flags->flag_l_ == 0 && compile_flags->flag_i_ == 0) ? "     " : "\n");
}


void Parse(char* path, struct CompileFlags * compile_flags){
    DIR* dp = opendir(path);
    if(dp == NULL){
        perror("Directory open error");
        return;
    }

    struct stat st;
    struct dirent* read;
    char line[1024];
    char pathtofile[1024];
    char directories_to_parse[1024][256];
    int directories_iterator = 0;

    if(compile_flags->flag_d_ == 1){ // special case if we deal with directory
        if (path[strlen(path) - 1] == '/') path[strlen(path) - 1] = 0;
        DIR* dp_special = opendir(".");
        while (read = readdir(dp_special)) {
            if(strcmp(path, read->d_name) == 0)
            {
                sprintf(pathtofile, "./%s", read->d_name);
                stat(pathtofile, &st);

                PrintInode(read, compile_flags); // inode print

                PrintLongListing(&st, compile_flags); // long listing print

                PrintName(read, compile_flags); // print name
            }
        }
    }
    else{ // general listing
        while (read = readdir(dp)) {
            if( (read->d_name[0] != '.' && compile_flags->flag_a_ == 0 ) || compile_flags->flag_a_ == 1 )
            {
                sprintf(pathtofile, "%s/%s", path, read->d_name);

                stat(pathtofile, &st);

                PrintInode(read, compile_flags); // inode print

                PrintLongListing(&st, compile_flags); // long listing print

                PrintName(read, compile_flags); // print name

                sprintf(line, "%s/%s", path, read->d_name);
                if(read->d_type == DT_DIR && compile_flags->flag_R_ == 1 && read->d_name[0] != '.'){
                    sprintf(directories_to_parse[directories_iterator], "%s", line);
                    directories_iterator += 1;
                }
            }
        }
    }
    if(errno){
        perror("Directory entry error");
    }
    closedir(dp);
    
    for(int i = 0; i < directories_iterator; i++){ // parsing deeper into directories
        printf("\n%s:\n", directories_to_parse[i]);
        Parse(directories_to_parse[i], compile_flags);
    }
    directories_iterator = 0;
}


int main(int argc, char * argv[]) {
    struct CompileFlags compile_flags = {0, 0, 0, 0, 0};

    static struct option longopts[] = { // add ability to use long flags versions
        { "long", no_argument, NULL, 'l'},
        { "inode", no_argument, NULL, 'i'},
        { "recursive", no_argument, NULL, 'R'},
        { "all", no_argument, NULL, 'a'},
        { "directory", no_argument, NULL, 'd'},
        { NULL, 0, NULL, 0}
    };

    int ch;
    while ((ch = getopt_long(argc, argv, "Rliad", longopts, NULL)) != -1) {
        switch (ch) {
            case 'R':
                compile_flags.flag_R_ = 1; // Recursive
                break;
            case 'l':
                compile_flags.flag_l_ = 1; // Long
                break;
            case 'i':
                compile_flags.flag_i_ = 1; // inode
                break;
            case 'a':
                compile_flags.flag_a_ = 1; // all
                break;
            case 'd':
                compile_flags.flag_d_ = 1; // directory
                break;
        }
    }


    if(optind == argc){         // current directory case
        Parse(".", &compile_flags); 
        if(compile_flags.flag_l_ == 0 && compile_flags.flag_i_ == 0) printf("\n");
    }
    else{       // directories names given
        for(int i = optind; i < argc; i++){
            printf("%s:\n", argv[i]);
            Parse(argv[i], &compile_flags);
            printf("\n");
        }
    }
    return 0;
}
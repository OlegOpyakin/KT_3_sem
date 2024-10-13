#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <uuid/uuid.h>

int main(int argc, char * argv[]) {
    //if (getpwnam(argv[1]) != NULL) return 0;

	uid_t cur_uid;
    gid_t list[32];
    
    struct passwd* usr;
    int num_of_groups = 32;
    
    if(argc == 1)
    {
        cur_uid = getuid();
        usr = getpwuid(cur_uid);
        num_of_groups = getgroups(32, list);
    } 
    else
    {
        usr = getpwnam(argv[1]);
        if(usr == NULL) { 
            fprintf(stderr,"id: wefrwer: no such user\n"); 
            return 0; 
        }
        getgrouplist(argv[1], usr->pw_gid, list, &num_of_groups);
    }

    printf("uid=%i(%s) ", usr->pw_uid, usr->pw_name);

    struct group* str;
    str = getgrgid(list[0]);
    printf("gid=%i(%s) ", usr->pw_gid, str->gr_name);

    //struct group* str;
    printf("groups=");
    for(int i = 0; i < num_of_groups; i++){
        str = getgrgid(list[i]);
        printf("%i(%s)", str->gr_gid, str->gr_name);
        if(i != num_of_groups - 1) printf(",");
        else printf("\n");
    }

	return 0;
}
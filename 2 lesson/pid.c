#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <uuid/uuid.h>

int main() {
	//pid_t id = getpid();
	//printf("pid id: %i\n", id);

	//pid_t idp = getppid();
	//printf("parent pid: %i\n", idp);
	
	gid_t gidgroup[64];
	int groups_num = getgroups(64, gidgroup);
	uid_t uid = getuid();
	
	gid_t gid = getgid();

	printf("uid=%i(%s)\n", uid, getpwuid(uid)->pw_name);
	printf("gid=%i(%s)\n", gid, getgrgid(gid)->gr_name);
	printf("groups: %i\n", groups_num);

	for (int i = 0; i < groups_num; i++)
		printf("group=%i(%s)\n", gidgroup[i], getgrgid(gidgroup[i])->gr_name);

	

	return 0;
}

#define _GNU_SOURCE
#include <sys/wait.h>
#include <sched.h>
#include <sys/mount.h>
#include "duckerscript.h"

#define Usage(code) ({printf("Usage: ducker run duckerfile\n"); exit(code);})

#define Module "ducker"
#define Container_StackSize (1<<13)
#define Default_Hostname "ducker_container"
#define Default_Root "/"
#define Proc_Path_Capacity 1024 

int CloneFn(void *);

static void run(DuckerScript_Table *);

int CloneFn(void *arg) {
	DuckerScript_Table *table = (DuckerScript_Table *)arg;

	char *hostname = DuckerScript_TableFindDefault(table, "hostname", Default_Hostname);
    if (sethostname(hostname, strlen(hostname)) == -1) Error(Module, "sethostname failed");
	printf("[INF] %s: hostname set to `%s`\n", Module, hostname);

	char *root = DuckerScript_TableFindDefault(table, "root", Default_Root);
	Ducker_RemoveTrailing(&root, '/');
	char *proc = (char *)malloc(sizeof(char)*Proc_Path_Capacity);
	snprintf(proc, Proc_Path_Capacity, "%s/proc", root);

	if (mount("/proc", proc, "proc", 0, NULL) != 0) Error(Module, "mount failed");
	printf("[INF] %s: /proc mounted to `%s`\n", Module, proc);

	root = DuckerScript_TableFindDefault(table, "root", Default_Root);
	if (chroot(root) != 0) Error(Module, "chroot failed");
	printf("[INF] %s: root set to `%s`\n", Module, root);
	
	DuckerScript_TableCmdExecute(table);

	if (umount(proc) != 0) Error(Module, "umount failed");
	printf("[INF] %s: /proc `%s` unmounted\n", Module, proc);
	
	exit(EXIT_SUCCESS);
}

static void run(DuckerScript_Table *table) {
    char *stack = (char *)malloc(sizeof(char)*Container_StackSize);
    if (!stack) Error(Module, "malloc failed");

	int flags = CLONE_NEWPID | CLONE_NEWUTS | SIGCHLD;
    int pid = clone(&CloneFn, stack+Container_StackSize, flags, (void *)table);
    if (pid == -1) {
        free(stack);
        Error(Module, "clone failed");
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        free(stack);
        Error(Module, "waitpid failed");
    }

    free(stack);
    if (WIFEXITED(status)) exit(WEXITSTATUS(status));
    else Error(Module, "child process did not terminate normally");
}


int main(int argc, char *argv[], char *envp[]) {
	if (argc != 3) Usage(EXIT_FAILURE);
	DuckerScript_Table *table = NULL;
	DuckerScript_ParseFile(argv[2], &table);
	if (strcmp(argv[1], "run") == 0) run(table);
	Usage(EXIT_FAILURE);
}

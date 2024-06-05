#define _GNU_SOURCE
#include <sys/wait.h>
#include <sched.h>
#include "duckerscript.h"

#define Usage(code) ({printf("Usage: ducker run duckerfile\n"); exit(code);})

#define Module "ducker"
#define Container_StackSize (1<<13)
#define Default_Hostname "ducker_container"
#define Default_Root "/"

int CloneFn(void *);

static void run(DuckerScript_Table *);

int CloneFn(void *arg) {
	DuckerScript_Table *table = (DuckerScript_Table *)arg;

	char *hostname = DuckerScript_TableFindDefault(table, "hostname", Default_Hostname);
    if (sethostname(hostname, strlen(hostname)) == -1) Error(Module, "sethostname failed");
	printf("[INF] %s: set hostname = `%s`\n", Module, hostname);

	char *root = DuckerScript_TableFindDefault(table, "root", Default_Root);
    if (chroot(root) != 0) Error(Module, "chroot failed");
	printf("[INF] %s: set root = `%s`\n", Module, root);
	
	DuckerScript_TableCmdExecute(table);
}

static void run(DuckerScript_Table *table) {
    char *stack = (char *)malloc(sizeof(char)*Container_StackSize);
    if (!stack) Error(Module, "malloc failed");
	
    int pid = clone(&CloneFn, stack+Container_StackSize, CLONE_NEWUTS | SIGCHLD, (void *)table);
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

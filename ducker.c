#define _GNU_SOURCE
#include <sys/wait.h>
#include <sched.h>
#include "duckerscript.h"

#define Usage(code) ({printf("Usage: ducker run duckerfile\n"); exit(code);})

#define Container_StackSize (1<<13)
#define Default_Hostname "ducker_container"

int CloneFn(void *);

static void run(DuckerScript_Table *);

int CloneFn(void *arg) {
	DuckerScript_Table *table = (DuckerScript_Table *)arg;
	char *hostname = DuckerScript_TableFind(table, "hostname");
	if (!hostname) hostname = Default_Hostname;
    if (sethostname(hostname, strlen(hostname)) != -1) DuckerScript_TableCmdExecute(table);
	else Error("ducker", "sethostname failed");
}

static void run(DuckerScript_Table *table) {
    char *stack = (char *)malloc(sizeof(char)*Container_StackSize);
    if (!stack) Error("ducker", "malloc failed");
	
    int pid = clone(&CloneFn, stack+Container_StackSize, CLONE_NEWUTS | SIGCHLD, (void *)table);
    if (pid == -1) {
        free(stack);
        Error("ducker", "clone failed");
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        free(stack);
        Error("ducker", "waitpid failed");
    }

    free(stack);
    if (WIFEXITED(status)) exit(WEXITSTATUS(status));
    else Error("ducker", "child process did not terminate normally");
}


int main(int argc, char *argv[], char *envp[]) {
	if (argc != 3) Usage(EXIT_FAILURE);
	DuckerScript_Table *table = NULL;
	DuckerScript_ParseFile(argv[2], &table);
	if (strcmp(argv[1], "run") == 0) run(table);
	Usage(EXIT_FAILURE);
}

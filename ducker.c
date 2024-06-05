#define _GNU_SOURCE
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include "duckerscript.h"

#define Usage(code) ({printf("Usage: ducker run <command> [args]\n"); exit(code);})

#define Container_StackSize (1<<13)
#define HostName "ducker_container"

typedef struct CloneFnArgs {
	char *cmd;
	char **args;
	char **envp;
} CloneFnArgs;

int CloneFn(void *);

static void run(int, char *[], char *[]);

int CloneFn(void *args) {
    CloneFnArgs *cloneFnArgs = (CloneFnArgs *)args;
    if (sethostname(HostName, strlen(HostName)) != -1) {
		execve(cloneFnArgs->cmd, cloneFnArgs->args, cloneFnArgs->envp);
		Error("ducker", "execve failed");
	}
	Error("ducker", "sethostname failed");
}

static void run(int argc, char *argv[], char *envp[]) {
    char *stack = (char *)malloc(sizeof(char)*Container_StackSize);
    if (!stack) Error("ducker", "malloc failed");
    CloneFnArgs args = { .cmd = argv[2], .args = &argv[2], .envp = envp };

    int pid = clone(&CloneFn, stack+Container_StackSize, CLONE_NEWUTS | SIGCHLD, (void *)&args);
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
	if (argc < 3) Usage(EXIT_FAILURE);
	if (strcmp(argv[1], "run") == 0) run(argc, argv, envp);
	Usage(EXIT_FAILURE);
}

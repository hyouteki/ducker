#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>

#define Error(message) ({printf("Error: %s\n", message); exit(EXIT_FAILURE);})
#define Usage(code) ({printf("Usage: ducker run <command> [args]\n"); exit(code);})

#define Container_StackSize (1<<13)
#define HostName "ducker_container"

typedef struct CloneFnArgs {
	char *cmd;
	char **args;
	char **envp;
} CloneFnArgs;

int CloneFn(void *);

int CloneFn(void *args) {
    CloneFnArgs *cloneFnArgs = (CloneFnArgs *)args;
    if (sethostname(HostName, strlen(HostName)) != -1) {
		execve(cloneFnArgs->cmd, cloneFnArgs->args, cloneFnArgs->envp);
		Error("execve failed");
	}
	Error("sethostname failed");
}

static void run(int, char *[], char *[]);

static void run(int argc, char *argv[], char *envp[]) {
    char *stack = (char *)malloc(sizeof(char)*Container_StackSize);
    if (!stack) Error("malloc failed");
    CloneFnArgs args = { .cmd = argv[2], .args = &argv[2], .envp = envp };

    int pid = clone(&CloneFn, stack+Container_StackSize, CLONE_NEWUTS | SIGCHLD, (void *)&args);
    if (pid == -1) {
        free(stack);
        Error("clone failed");
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        free(stack);
        Error("waitpid failed");
    }

    free(stack);
    if (WIFEXITED(status)) exit(WEXITSTATUS(status));
    else Error("child process did not terminate normally");
}


int main(int argc, char *argv[], char *envp[]) {
	if (argc < 3) Usage(EXIT_FAILURE);
	if (strcmp(argv[1], "run") == 0) run(argc, argv, envp);
	Usage(EXIT_FAILURE);
}

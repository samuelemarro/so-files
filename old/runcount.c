#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    FILE *file = fopen("/tmp/countpid", "r");
    
    pid_t pid;
    
    fscanf(file, "%d", &pid);

    kill(pid, SIGUSR1);

    char *args[argc];
    for (int i = 0; i < argc - 1; i++) {
        args[i] = argv[i + 1];
    }
    args[argc - 1] = NULL;
    if (fork() == 0) {
        wait(NULL);
    }
    else {
        execvp(args[0], args);
    }

    kill(pid, SIGUSR2);
    sleep(1);
}
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    char *args[argc];
    for (int i = 0; i < argc - 1; i++) {
        args[i] = argv[i + 1];
    }
    args[argc - 1] = NULL;
    if (fork()) {
        execvp(args[0], args);
    }
    else {
        wait(NULL);
    }
}
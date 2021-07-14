#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> // sleep
#include <string.h> // strlen

int main(int argc, char **argv) {
    int fd[2];
    pipe(fd);

    int pid = fork();

    FILE *from = fdopen(fd[1], "w");
    FILE *to = fdopen(fd[0], "r");

    if (pid) {
        close(fd[1]);
    }
    else {
        close(fd[0]);
    }
    printf("%d\n", pid);

    int i = 0;
    while (1)
    {
        if (pid) {
            char str[100];
            read(fd[0], str, 10);
            printf("Read!\n");
            sscanf(str, ">%d|", &i);
            printf("Output: %d\n", i);
            sleep(2);
        }
        else {
            printf("Writing...\n");
            char str[100];

            sprintf(str, ">%d|", i);
            write(fd[1], str, strlen(str));
            printf("Written!\n");
            i++;
            sleep(1);
        }
    }
    
    return 0;
}
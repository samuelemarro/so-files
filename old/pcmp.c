#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

long getSize(FILE* f) {
    fseek(f, 0L, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    return sz;
}

void difference(char *name1, char *name2) {
    printf("%s %s differ\n", name1, name2);
}

int main(int argc, char** argv) {
    char c;
    char *parallelismArg;
    int parallelism = 2;
    char *end;
    while ((c = getopt (argc, argv, "l:")) != -1) {
        switch (c) {
            case 'l':
                parallelism = strtol(optarg, &end, 10);

                if (parallelism == 0) {
                    printf("Invalid value passed to -l\n");
                    return 1;
                }
                break;
            case '?':
                if (optopt == 'l')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                return 1;
        }
    }
    
    if (argc != 3 && argc != 5) {
        printf("Usage: pcmp [-l number] file1 file2\n");
        return 1;
    }

    char file1[256];
    char file2[256];

    if (argc == 3) {
        strcpy(file1, argv[1]);
        strcpy(file2, argv[2]);
    }
    else {
        strcpy(file1, argv[3]);
        strcpy(file2, argv[4]);
    }

    FILE* f1 = fopen(file1, "r");
    FILE* f2 = fopen(file2, "r");

    if (f1 == NULL) {
        printf("%s does not exist.\n", file1);
        return 1;
    }
    if (f2 == NULL) {
        printf("%s does not exist.\n", file2);
        return 1;
    }

    long size1 = getSize(f1);
    long size2 = getSize(f2);

    if (size1 != size2) {
        difference(file1, file2);
        return 0;
    }

    int pid;
    long start = 0;
    long slice = size1 / parallelism;
    long extra = size1 % parallelism;
    long readLength = slice;

    int p[2];
    pipe(p);

    for (int j = 0; j < parallelism - 1; j++) {
        if (pid = fork()) {
            if (j == parallelism - 1) {
                // Per evitare errori dovuti a divisioni intere
                readLength = size1 - start;
            }
            break;
        }
        start += slice;
    }
    printf("PID: %d", pid);

    if (pid) {
        close(p[0]);
    }
    else {
        close(p[1]);
    }

    if (start != 0) {
        fseek(f1, start, SEEK_SET);
        fseek(f2, start, SEEK_SET);
    }

    int foundDifference = 0;

    for (long i = 0; i < readLength; i++) {
        // reading file
        char c1 = fgetc(f1);
        char c2 = fgetc(f2);
 
        // end of file indicator
        if (feof(f1) || feof(f2)) {
            printf("Unexpected End of File\n");
            return 1;
        }
 
        if (c1 != c2) {
            foundDifference = 1;
            break;
        }
    }

    if (pid) {
        char c[2];
        c[0] = '0' + foundDifference;
        c[1] = '\0';
        write(p[1], c, 1);
        close(p[1]);
    }
    else {
        for (int i = 0; i < parallelism - 1; i++) {
            char buff[2];
            read(p[0], buff, 1);
            if (buff[0] == '1') {
                foundDifference = 1;
            }
        }

        close(p[0]);

        if (foundDifference == 1) {
            difference(file1, file2);
        }
    }

    return 0;
}
#include <stdio.h>
#include <dirent.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define DAY 24 * 3600
#define CMPLEN 256

void age(char *path, int modification, int access, int days) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        // Ignore non-existent files, but print the error
        perror(path);
        return;
    }

    if (S_ISDIR(statbuf.st_mode)) {
        // Ignore directories
        return;
    }

    struct utimbuf ut;
    ut.modtime = statbuf.st_mtim.tv_sec - days * DAY * (modification ? 1 : 0);
    ut.actime = statbuf.st_atim.tv_sec - days * DAY * (access ? 1 : 0);
    utime(path, &ut);
}

void ageAll(int access, int modification, int days) {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strncmp(dir->d_name, ".", CMPLEN) != 0 && strncmp(dir->d_name, "..", CMPLEN) != 0) {
                age(dir->d_name, access, modification, days);
            }
        }
        closedir(d);
    }
}

int main(int argc, char** argv) {
    char c;
    int access = -1;
    int modification = -1;
    int days = 10;
    char *end;

    while ((c = getopt (argc, argv, "amhd:")) != -1) {
        switch (c) {
            case 'a':
                access = 0;
                break;
            case 'm':
                modification = 0;
                break;
            case 'h':
                printf("Usage: ager2 [-amh] [-d delay] [files...]\n");
                printf("Ages all specified files. If files is empty, ages all files in current directory.\n");
                printf("Options:\n");
                printf("\t-a: \t\t\tChange only access time.\n");
                printf("\t-m: \t\t\tChange only modification time.\n");
                printf("\t-d delay: \t\tSet aging time (in days). Default: 10.\n");
                printf("\t-h: \t\t\tPrint this message and return.\n");
                printf("Note: If neither -a or -m are passed, it is treated as -am.\n");
                return 0;
            case 'd':
                days = strtol(optarg, &end, 10);

                if (days <= 0) {
                    printf("Invalid value passed to -l\n");
                    return 1;
                }
                break;
            case '?':
                if (optopt == 'd')
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

    if (access != 0 && modification != 0) {
        access = 0;
        modification = 0;
    }

    if (optind == argc) {
        ageAll(access, modification, days);
    }
    else {
        for (int i = optind; i < argc; i++) {
            age(argv[i], access, modification, days);
        }
    }

    return(0);
}
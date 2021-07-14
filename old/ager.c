#include <stdio.h>
#include <dirent.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define DAY 24 * 3600
#define CMPLEN 256

void age(char *path) {
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
    ut.modtime = statbuf.st_mtim.tv_sec - 10 * DAY;
    ut.actime = statbuf.st_atim.tv_sec - 10 * DAY;
    utime(path, &ut);
}

void ageAll() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strncmp(dir->d_name, ".", CMPLEN) != 0 && strncmp(dir->d_name, "..", CMPLEN) != 0) {
                age(dir->d_name);
            }
        }
        closedir(d);
    }
}

int main(int argc, char** argv) {
    if (argc == 1) {
        ageAll();
    }
    else {
        for (int i = 1; i < argc; i++) {
            age(argv[i]);
        }
    }

    return(0);
}
#include <stdio.h>
#include <unistd.h> // getopt
#include <ctype.h> // isprint
#include <stdlib.h> // strtol

int main(int argc, char **argv) {
    char c;
    char *end;
    int days;
    while ((c = getopt (argc, argv, "ab:h")) != -1) {
        switch (c) {
            case 'a':
                // Flag booleano
                break;
            case 'h':
                // Help
                return 0;
            case 'b':
                days = strtol(optarg, &end, 10);

                if (days <= 0) {
                    fprintf(stderr, "Invalid value passed to -l\n");
                    return 1;
                }
                break;
            case '?':
                // Qui controlli per tutti quelli del tipo -nome valore
                if (optopt == 'b')
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
    // optind contiene ora l'indice successivo
}
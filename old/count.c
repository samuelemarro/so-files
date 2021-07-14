#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <string.h>

int main(int argc, char **argv) {
    int counter = 0;

    pid_t pid = getpid();
    FILE* file = fopen("/tmp/countpid", "w");
    fprintf(file, "%d", pid);
    fclose(file);

    int sfd;
	sigset_t mask;
 
	/* We will handle SIGTERM and SIGINT. */
	sigemptyset (&mask);
	sigaddset (&mask, SIGTERM);
	sigaddset (&mask, SIGINT);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
 
	/* Block the signals thet we handle using signalfd(), so they don't
	 * cause signal handlers or default signal actions to execute. */
	if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
		perror ("sigprocmask");
		return 1;
	}
 
	/* Create a file descriptor from which we will read the signals. */
	sfd = signalfd (-1, &mask, 0);
	if (sfd < 0) {
		perror ("signalfd");
		return 1;
	}
 
	while (1) {
 
		/** The buffor for read(), this structure contains information
		 * about the signal we've read. */
		struct signalfd_siginfo si;
 
		ssize_t res;
 
		res = read (sfd, &si, sizeof(si));
 
		if (res < 0) {
			perror ("read");
			return 1;
		}
		if (res != sizeof(si)) {
			fprintf (stderr, "Something wrong\n");
			return 1;
		}
 
		if (si.ssi_signo == SIGTERM)
			printf ("Got SIGTERM\n");
		else if (si.ssi_signo == SIGINT) {
			printf ("Bye!\n");
			break;
		}
        else if (si.ssi_signo == SIGUSR1 || si.ssi_signo == SIGUSR2) {
            printf("%d", si.ssi_signo);
            if (si.ssi_signo == SIGUSR1) {
                counter++;
            }
            else {
                counter--;
            }

            printf("Count: %d\n", counter);
        } 
		else {
			fprintf (stderr, "Got some unhandled signal\n");
			return 1;
		}
	}
 
	/* Close the file descriptor if we no longer need it. */
	close (sfd);
}
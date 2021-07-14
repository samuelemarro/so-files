#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <string.h>

int main(int argc, char **argv) {
    int sfd;
	sigset_t mask;
 
	sigemptyset (&mask);
	sigaddset (&mask, SIGTERM);
	sigaddset (&mask, SIGINT);
    // Questo esempio usa SIGUSR1 e SIGUSR2
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
		struct signalfd_siginfo si;
 
		ssize_t res;
 
		res = read (sfd, &si, sizeof(si));
 
		if (res < 0) {
			perror ("read");
			return 1;
		}
		if (res != sizeof(si)) {
			fprintf (stderr, "Something went wrong.\n");
			return 1;
		}
 
		if (si.ssi_signo == SIGTERM)
			printf ("Got SIGTERM\n");
		else if (si.ssi_signo == SIGINT) {
			break;
		}
        else if (si.ssi_signo == SIGUSR1) {
        } 
		else {
			fprintf (stderr, "Got some unhandled signal\n");
			return 1;
		}
	}
 
	close (sfd);
}
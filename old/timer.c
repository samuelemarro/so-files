#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>

#define TIMEOUT 3
#define POLL_PERIOD 1

int main(int argc, char *argv[])
{
    int ret;
    int fd = -1;
    struct itimerspec timeout;
    unsigned long long missed;

    /* create new timer */
    fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (fd <= 0) {
        printf("Failed to create timer\n");
        return 1;
    }

    /* set to non-blocking */
    /*ret = fcntl(fd, F_SETFL, O_NONBLOCK);
    if (ret) {
        printf("Failed to set to non blocking mode\n");
        return 1;
    }*/

    /* set timeout */
    timeout.it_value.tv_sec = TIMEOUT;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = TIMEOUT; /* recurring */
    timeout.it_interval.tv_nsec = 0;
    ret = timerfd_settime(fd, 0, &timeout, NULL);
    if (ret) {
        printf("Failed to set timer duration\n");
        return 1;
    }

    struct pollfd fds[1];
    fds[0].events = POLLIN;
    fds[0].fd = fd;

    while (1) {
        poll(fds, 1, -1);
        printf("Polling\n");
        read(fd, &missed, sizeof(missed));
        printf("Number of expiries missed: %lld\n", missed);
    }

    return 0;
}
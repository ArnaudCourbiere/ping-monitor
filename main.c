#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>

#define PID_DIR "/var/run/ping_monitor"
#define PID_FILE "/var/run/ping_monitor/pid"

static void handler(int sig, siginfo_t *siginfo, void *context)
{
    unlink(PID_FILE);
    _exit(0);
}

int main(int argc, char *argv[])
{
    void ping_log(char*, ...);
    //static void handler(int, siginfo_t*, void*);

    // Setup signal handler
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_sigaction = &handler;
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGTERM, &act, NULL) < 0) {
        ping_log("sigaction: unable to register signal handler for SIGTERM.");
        exit(EXIT_FAILURE);
    }   
    if (sigaction(SIGINT, &act, NULL) < 0) {
        ping_log("sigaction: unable to register signal handler for SIGINT.");
        exit(EXIT_FAILURE);
    }   
    if (sigaction(SIGHUP, &act, NULL) < 0) {
        ping_log("sigaction: unable to register signal handler for SIGHUP.");
        exit(EXIT_FAILURE);
    }   

    // Setup ping daemon
    pid_t ppid, pid, sid;

    if (mkdir(PID_DIR, 0755) == -1 && errno != EEXIST) {
        printf("Unable to create pid directory %s.\n", PID_DIR);
        exit(EXIT_FAILURE);
    }

    ppid = getpid();
    printf("Parent pid is %d.\n", ppid);

    // Fork off parent process
    printf("Forking child process...\n");
    pid = fork();

    if (pid < 0) {
        printf("Unable to fork child\n");
        exit(EXIT_FAILURE);
    }

    // Exit the parent process
    if (pid > 0) {
        FILE *fp = fopen(PID_FILE, "w");

        if (fp == NULL) {
            printf("Failed to open file %s.\n", PID_FILE);
            exit(EXIT_FAILURE);
        }

        int len = fprintf(fp, "%d", pid);

        if (len < 0) {
            printf("Failed to write pid to pid file.\n");
            exit(EXIT_FAILURE);
        }

        fclose(fp);
        printf("Child pid is %d.\n", pid);
        printf("Exiting parent process...\n");
        exit(EXIT_SUCCESS);
    }

    // Change the file mode mask
    umask(0);

    // Open any logs here

    // Create a new SID for the child process
    sid = setsid();

    if (sid < 0) {
        ping_log("Unable to create a session id");
        exit(EXIT_FAILURE);
    }

    // Change the cwd to root dir
    if (chdir("/") < 0) {
        ping_log("Unable to change the CWD to root");
        exit(EXIT_FAILURE);
    }

    // Close the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Daemon code
    int online = 0;
    int success;

    while (1) {
        success = system("ping -c 3 -w 15 8.8.8.8");

        if (success == 0) {
            if (online == 0) {
                // Online
                online = 1;
                ping_log("Online");
            }
        } else if (online == 1) {
            // Offline
            online = 0;
            ping_log("Offline");
        }

        sleep(30);
    }

    exit(EXIT_SUCCESS);
}

void ping_log( char *buff,...)
{
    va_list arglist;

    va_start(arglist,buff);

    openlog("ping_monitor", LOG_PID, 0);
    syslog(LOG_MAKEPRI(LOG_DAEMON, LOG_INFO), buff, arglist);
    closelog();

    va_end(arglist);
}


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>


#define AMOUNT_OF_CHILD 2

int index_message = 0;
int child_index = 0;
pid_t child_pids[AMOUNT_OF_CHILD];

void printTime(){
    struct timeval current_time;
    struct tm *today;
    gettimeofday(&current_time, NULL);
    today = localtime(&current_time.tv_sec);

    printf("\tTime: %d:%0d:%0d:%ld\t",
           today->tm_hour,
           today->tm_min,
           today->tm_sec,
           current_time.tv_usec
    );
}

void show_info(int local_child_index, char* status, int index_sig){
    printf("Number of message: %d Pid: %d PPid: %d", index_message++, getpid(), getppid());
    printTime();
    printf("Son: %d Status: %s SIGUSR %d\n", local_child_index, status, index_sig);
}

int find_index_child_pid(pid_t child_pid){
    for (int i = 0; i < AMOUNT_OF_CHILD; ++i){
        if (child_pids[i] == child_pid){
            return i;
        }
    }
    return -1;
}

void parent_signal_handler(int sig, siginfo_t *info_sig, void *zal){
    show_info(find_index_child_pid(info_sig->si_pid),
              "put", 2);

    if (index_message >= 5){
        kill(getppid(), SIGTERM);
        return;
    }

    struct timespec period = {.tv_nsec = 100e6, .tv_sec = 0};
    nanosleep(&period, NULL);

    kill(0, SIGUSR1);
}

void child_signal_handler(int sig){
    show_info(child_index, "get", 1);
    kill(getppid(), SIGUSR2);
}

void parent_term(int sig){
    signal(SIGTERM , SIG_IGN);
    kill(0, SIGTERM);
    signal(SIGTERM, parent_term);

    while(wait(NULL) != -1 || errno == EINTR);

    if (errno != ECHILD){
        perror("wait");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    struct sigaction sig;
    sig.sa_flags = SA_SIGINFO;
    sig.sa_sigaction = parent_signal_handler;
    sigaction(SIGUSR2, &sig, NULL);

    signal(SIGUSR1, SIG_IGN);
    signal(SIGTERM, parent_term);

    pid_t pid = 1;
    for (int i = 0; i < AMOUNT_OF_CHILD && pid > 0; ++i){
        switch (pid = fork()) {
            case -1:
                perror("Couldn't create child process");
                return EXIT_FAILURE;
            case 0:
                printf("Child Process %d PID: %d PPID: %d",
                       child_index, getpid(), getppid());

                signal(SIGUSR1, (__sighandler_t) child_signal_handler);
                break;
            default:
                child_pids[i] = pid;
                child_index++;
                break;
        }
    }

    if (pid > 0){
        sleep(1);
        kill(0, SIGUSR1);
    }

    while(1);
}
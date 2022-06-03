#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

#define PROCESS_COUNT 9
#define AMOUNT_PAIRS 9
#define SIGDEF -2
#define NOSIG -1
#define AMOUNT_OF_SIGNAL 2

typedef struct pair {
    int flag;
    int signal;
    int from;
    int to;
} Pair;

Pair queue[] = {
        {1, NOSIG, 0, 1},
        {1, SIGUSR1, 1, 2},
        {1, SIGUSR1, 1, 3},
        {1, SIGUSR1, 2, 4},
        {1, SIGUSR1, 2, 5},
        {1, SIGUSR1, 5, 6},
        {1, SIGUSR1, 6, 7},
        {1, SIGUSR1, 6, 8},
        {0, SIGUSR2, 8, 1}
};

int queueIndex;
int nodeIndex = 0;
pid_t pidArray[PROCESS_COUNT];

int sentSignalsAmount[AMOUNT_OF_SIGNAL];

int findChildrenPGID(int index, pid_t *pgid, int *sig) {
    for (int i = 0; i < PROCESS_COUNT; ++i) {
        if (queue[i].from == index){
            *pgid = getpgid(pidArray[queue[i].to]);
            if (sig != NULL) {
                *sig = queue[i].signal;
            }
            return 1;
        }
    }
    return 0;
}

void printTime(char *status, int index_signal) {
    struct timeval current_time;
    struct tm *today;
    gettimeofday(&current_time, NULL);
    today = localtime(&current_time.tv_sec);

    printf("%-5d %-5d %-5d %6s   USR%1d Time: %d:%0d:%0d:%ld\n",
           nodeIndex,
           getpid(),
           getppid(),
           status,
           index_signal,
           today->tm_hour,
           today->tm_min,
           today->tm_sec,
           current_time.tv_usec
    );
    fflush(stdout);
}

int signalChildren(int index, int sig) {
    int s;
    pid_t pgid;
    if (findChildrenPGID(index, &pgid, &s)) {
        int signal = (sig == SIGDEF ? s : sig);
        kill(-pgid, signal);
        return signal;
    }
    return NOSIG;
}

void userSignal(int sig) {
    printTime("get", sig == SIGUSR1 ? 1 : 2);

    if ((nodeIndex == 1) && (sentSignalsAmount[0] + sentSignalsAmount[1]) == 101) {
        kill(getpid(), SIGTERM);
        return;
    }

    int signal = signalChildren(nodeIndex, SIGDEF);
    if (signal != NOSIG) {
        int sigNum = signal == SIGUSR1 ? 1 : 2;
        printTime("put", sigNum);
        sentSignalsAmount[sigNum - 1]++;
    }
}

void sigterm(int sigIndex) {
    printf("%-5d %-5d %-5d has terminated. Sent signals: SIGUSR1 %4d SIGUSR2 %4d\n",
           nodeIndex, getpid(), getppid(), sentSignalsAmount[0], sentSignalsAmount[1]);


    pid_t pgid;
    if (findChildrenPGID(nodeIndex, &pgid, NULL)) {
        kill(-pgid, SIGTERM);
        waitpid(-pgid, NULL, 0);
    }

    exit(0);
}

int main(int argc, char* argv[]) {
    pidArray[0] = getpid();
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;

    int pgid = 0;
    for (int i = 0; i < AMOUNT_PAIRS; ++i) {
        if (i == 0) {
            pgid = 0;
        }

        if (nodeIndex == queue[i].to && queue[i].signal != NOSIG) {
            signal(queue[i].signal, userSignal);
        }

        if (nodeIndex == queue[i].from && queue[i].flag) {
            pid_t pid;
            switch (pid = fork()) {
                case -1:
                    perror("Couldn't create child process");
                    kill(0, SIGKILL);
                    exit(EXIT_FAILURE);

                case 0:
                    nodeIndex = queue[i].to;
                    pidArray[nodeIndex] = getpid();
                    i = -1;
                    signal(SIGTERM, sigterm);
                    break;

                default:
                    if (!pgid) {
                        pgid = pid;
                    }
                    setpgid(pid, pgid);
                    pidArray[queue[i].to] = pid;
                    break;
            }
        }
    }

    if (nodeIndex == 0)
        printf("INDEX PID   PPID  STATUS SIGNAL TIME\n");

    sleep(1);

    if (nodeIndex == 1) {
        int sig = signalChildren(nodeIndex, SIGDEF);
        if (sig != NOSIG) {
            int type_sig = (sig == SIGUSR1 ? 1 : 2);
            ++sentSignalsAmount[type_sig - 1];
        }
    }

    if (nodeIndex == 0) {
        getchar();
        while (wait(NULL) != -1);
        return 0;
    }
    else {
        while(1) pause();
    }

}
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define QUEUE_LENGTH 8

typedef struct pair {
    int from;
    int to;
} Pair;

Pair queue[] = {
        {0, 1},
        {1, 2},
        {1, 3},
        {2, 4},
        {2, 5},
        {5, 6},
        {6, 7},
        {6, 8}
};

int nodeIndex = 0;
int queueIndex;

void action(int sig) {
    queueIndex++;

    if (queueIndex == QUEUE_LENGTH) {
        return;
    }
    
    if (queue[queueIndex].from == nodeIndex) {
        printf("Creating new process from index = %d\n", nodeIndex);
        switch (fork()) {
            case -1: 
                perror("Couldn't create child process");
                kill(0, SIGKILL);
                exit(-1);
            case 0:
                signal(SIGUSR1, action);
                nodeIndex = queue[queueIndex].to;
                printf("New process created with index = %d\n", nodeIndex);
                kill(0, SIGUSR1);
                break;
            default: 
                break;
        }
    }
}


int main() {
    queueIndex = -1;
    signal(SIGUSR1, action);
    action(SIGUSR1);

    if (nodeIndex == 0) {
        getchar();
        kill(0, SIGKILL);
    }
    else {
        while(1) pause();
    }

    return 0;
}
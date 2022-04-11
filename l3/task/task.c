#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

void printCurrentTime(){
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char bufftime[40], buf[100];
    strftime(bufftime, sizeof bufftime, "%F %T", gmtime(&ts.tv_sec));
    sprintf(buf, "Child process:\n\tPID %d\n\tPPID %d\nCurrent time: %s:%03ld UTC\n", getpid(), getppid(), bufftime, ts.tv_nsec/1000000); 
    write(1, buf, strlen(buf));
    exit(0);
}

int main(){
    pid_t pid = fork();
    if (pid > 0){
        pid = fork();
        if (!pid){
            printCurrentTime();
        }
        else if (pid < 0){
            perror("Process forking failed(2).");
        }
    }
    else if (pid < 0) {
        perror("Process forking failed(1).");
    }
    else{
        printCurrentTime();
    }
    if (pid){
        system("ps -x");
    }
    return 0;
}
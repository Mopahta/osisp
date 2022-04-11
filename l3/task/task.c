#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

void printCurrentTime(){
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%F %T", gmtime(&ts.tv_sec));
    fprintf(stdout, "Child process:\n\tPID %d\n\tPPID %d\nCurrent time: %s:%03ld UTC\n", getpid(), getppid(), buff, ts.tv_nsec/1000000); 
    fflush(stdout); 
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
    wait(NULL);
    return 0;
}
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    if (argc < 4){
        fprintf(stderr, "Not enough parameters.\nUsage:\n\t%s <dir> <concurrent_processes>\n", argv[0]);
        return;
    }

    return 0;
}
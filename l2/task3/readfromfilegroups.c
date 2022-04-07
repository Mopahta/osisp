#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

void main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr, "Not enough parameters.\nUsage:\n\treadfromfilegroups file_name lines_in_group\n");
        return;
    }

    FILE *readFile;
    char *endptr;

    long linesAmount = strtol(argv[2], &endptr, 10), groupNumber = 0;

    if (errno != 0) {
        if (errno == ERANGE && linesAmount == LONG_MAX){
            fprintf(stderr, "Value overflow.\n");
        }
        else if (errno == ERANGE && linesAmount == LONG_MIN){
            fprintf(stderr, "Value underflow.\n");
        }
        else {
            fprintf(stderr, "Value is not a valid number.\n");
        }
        exit(EXIT_FAILURE);
    }

    if (endptr == argv[2] || (endptr - argv[2] < strlen(argv[2]))) {
        fprintf(stderr, "Invalid number.\n");
        exit(EXIT_FAILURE);
    }

    readFile = fopen(argv[1], "r");
    char inp;
    long currLinesAmount = 1;
    
    while ((inp = fgetc(readFile)) != EOF){
        fputc(inp, stdout);

        if (inp == '\n'){
            currLinesAmount++;
            if (currLinesAmount > linesAmount && linesAmount){
                getchar();
                currLinesAmount = 1;
            }
        }
    }

    fclose(readFile);
}
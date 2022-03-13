#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[]){
    if (argc < 3){
        perror("Input file not specified.\nUsage:\n\treadfromfilegroups file_name lines_in_group");
        return;
    }

    FILE *readFile;

    int linesAmount = atoi(argv[2]), groupNumber = 0;

    readFile = fopen(argv[1], "r");
    char inp;
    int currLinesAmount = 1;
    
    while ((inp = fgetc(readFile)) != EOF){
        fputc(inp, stdout);

        if (inp == '\n'){
            currLinesAmount++;
            if (currLinesAmount > linesAmount){
                getchar();
                putc('\n', stdout);
                currLinesAmount = 1;
            }
        }
    }

    fclose(readFile);
}
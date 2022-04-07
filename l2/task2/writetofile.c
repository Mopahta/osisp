#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void main(int argc, char* argv[]){
    if (argc < 2){
        perror("Output file not specified.\nUsage:\n\twritetofile file_name");
        return;
    }

    FILE *outputFile;

    outputFile = fopen(argv[1], "w");

    if (!outputFile){
        fprintf(stderr, "Couldn't open file %s", argv[1]);
        return;
    }
    char inp;

    while ((inp = fgetc(stdin)) != 6){
        fputc(inp, outputFile);
    }

    fclose(outputFile);
}
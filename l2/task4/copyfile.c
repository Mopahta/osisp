#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr, "Input file not specified.\nUsage:\n\tcopyfile src dest");
        return;
    }

    FILE *srcFile, *destFile;

    char inp = 0;
    struct stat info;

    srcFile = fopen(argv[1], "r");
    
    if (srcFile == NULL){
        printf("Can't open file %s", argv[1]);
        return;
    }

    destFile = fopen(argv[2], "w");

    if (destFile == NULL){
        printf("Can't open file %s", argv[2]);
    }

    if(stat(argv[1], &info)){
        fprintf(stderr, "Couldn't get file status: %s\n", argv[1]);
    }

    while ((inp = fgetc(srcFile)) != EOF){
        fputc(inp, destFile);
    }


    chmod(argv[2], info.st_mode);

    fclose(srcFile);
    fclose(destFile);
}
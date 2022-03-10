#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[]){
    if (argc < 3){
        perror("Input file not specified.\nUsage:\n\tcopyfile src dest");
        return;
    }

    FILE *srcFile, *destFile;

    char inp = 0;
    struct stat info;

    srcFile = fopen(argv[1], "rb");
    destFile = fopen(argv[2], "wb");

    stat(argv[1], &info);


    while ((inp = fgetc(srcFile)) != EOF){
        fputc(inp, destFile);
    }


    chmod(argv[2], info.st_mode);

    fclose(srcFile);
    fclose(destFile);
}
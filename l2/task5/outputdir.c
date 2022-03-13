#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

void printDirContents(char *path){
    DIR *dir = opendir(path);
    struct dirent *d;
    while ((d = readdir(dir)) != NULL){
        puts(d->d_name);
    }
    closedir(dir);
}

void main(){
    DIR *dir;

    printf("Current dir contents:\n");
    printDirContents(".");
    printf("Root dir contents:\n");
    printDirContents("/");
}
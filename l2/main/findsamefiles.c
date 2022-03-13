#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "stack.h"
#include "list.h"

FILE* outputFile;
char header;

void processDir(listHead *list, char *path, struct stack_t *stack);

char *completeDir(char *path){
    if (path[strlen(path)-1] == '/'){
		return path;
	}
	else
		return strcat(path, "/");
}

void searchForFiles(listHead *list, char *path){
    struct stack_t *stack = newStack();

    path = completeDir(path);
    push(stack, path);

    while(stack->head){
        char *currPath = pop(stack);
        processDir(list, currPath, stack);
        free(currPath);
    }
    clearStack(stack);
}

struct stat isRegularFile(char *path){
    struct stat fileStat;
    if (stat(path, &fileStat)){
        fprintf(stderr, "Couldn't get file status: %s\n", path);
        fprintf(outputFile, "Couldn't get file status: %s\n", path);
        return (struct stat) {.st_size = 0};
    }
    else {
        return fileStat;
    }
}

void processDir(listHead *list, char *path, struct stack_t *stack){
    DIR *dir;
    struct dirent *item;
    struct stat fileStat;
    off_t size;

    dir = opendir(path);
    if (dir == NULL){
        fprintf(stderr, "Couldn't open directory: %s", path);
        fprintf(outputFile, "Couldn't open directory: %s", path);
        exit(EXIT_FAILURE);
    }

    int currDirLen = strlen(path);

    while ((item = readdir(dir)) != NULL){
        if (strcmp(item->d_name, ".") & strcmp(item->d_name, "..")){
            if (item->d_type == DT_REG){
                char *tempDir = copyString(path);
                tempDir = completeDir(tempDir);
                strcat(tempDir, item->d_name);
                fileStat = isRegularFile(tempDir);
                if (fileStat.st_size){
                    addToList(list, 
                        (fileInfo) {
                            .name = tempDir, 
                            .size = fileStat.st_size, 
                            .inode = fileStat.st_ino, 
                            .acc_mode = fileStat.st_mode, 
                            .cr_time = fileStat.st_ctime
                            });
                }
            }
            else if (item->d_type == DT_DIR){
                char *tempDir = (char *) malloc(sizeof(char)*currDirLen*strlen(item->d_name));
                strcpy(tempDir, path);
                tempDir = completeDir(tempDir);
                push(stack, strcat(tempDir, item->d_name));
            }
        }
    }
    closedir(dir);
}

int areFilesEqual(char *path1, char *path2){
    FILE *file1 = fopen(path1, "rb");
    if (!file1){
        fprintf(stderr, "Couldn't open file %s\n", path1);
        fprintf(outputFile, "Couldn't open file %s\n", path1);
        return 0;
    }
    FILE *file2 = fopen(path2, "rb");
    if (!file2){
        fprintf(stderr, "Couldn't open file %s\n", path2);
        fprintf(outputFile, "Couldn't open file %s\n", path2);
        fclose(file1);
        return 0;
    }

    char a = 0, b = 0;

    while (a != EOF || b != EOF){
        a = getc(file1);
        b = getc(file2);
        if (a != b){
            fclose(file1);
            fclose(file2);
            return 0;
        }
    }

    fclose(file1);
    fclose(file2);
    return 1;
}

void formatOutput(FILE *stream, listEl *info){
    fprintf(stream, "%s\n\tSize: %i B\n\tTime created: %s\tAccess mode: %s%s%s%s%s%s%s%s%s%s\n\tInode number: %i:\n", 
            info->info.name, 
            info->info.size, 
            ctime(&(info->info.cr_time)), 
            S_ISDIR(info->info.acc_mode) ? "d" : "-",
            info->info.acc_mode & S_IRUSR ? "r" : "-",
            info->info.acc_mode & S_IWUSR ? "w" : "-",
            info->info.acc_mode & S_IXUSR ? "x" : "-",
            info->info.acc_mode & S_IRGRP ? "r" : "-",
            info->info.acc_mode & S_IWGRP ? "w" : "-",
            info->info.acc_mode & S_IXGRP ? "x" : "-",
            info->info.acc_mode & S_IROTH ? "r" : "-",
            info->info.acc_mode & S_IWOTH ? "w" : "-",
            info->info.acc_mode & S_IXOTH ? "x" : "-",
            info->info.inode);
}

void printEqualFiles(listEl *tmp1, listEl *tmp2){
    listEl *prev;
    while (tmp2){
        if (tmp1 != tmp2 && tmp1->info.size == tmp2->info.size && areFilesEqual(tmp1->info.name, tmp2->info.name)){
            if (header){
                printf("\n");
                formatOutput(stdout, tmp1);
                formatOutput(outputFile, tmp1);                
                header = 0;
            }
            formatOutput(stdout, tmp2);
            formatOutput(outputFile, tmp2);
            free(tmp2->info.name);
            if (tmp2->next){
                tmp2->info = tmp2->next->info;
                tmp2->next = tmp2->next->next;
            }
        }
        else {
            tmp2 = tmp2->next;
        }
    }
}

void compareFiles(listHead *list1, listHead *list2){
    struct stat fileStat;
    listEl *tmp1 = list1->head;
    listEl *tmp2;
    while (tmp1){
        header = 1;
        tmp2 = list1->head;
        printEqualFiles(tmp1, tmp2);
        tmp2 = list2->head;
        printEqualFiles(tmp1, tmp2);

        tmp1 = tmp1->next;
    }
}


void main(int argc, char *argv[]){
    if (argc < 4){
        fprintf(stderr, "Not enough parameters.\nUsage:\n\t%s <dir_1> <dir_2> <output_file>\n", argv[0]);
        return;
    }

    outputFile = fopen(argv[3], "wr+");
	if (outputFile == NULL){
		fprintf(stderr, "Couldn't create file %s", outputFile);
		return;
	}
	
    listHead *list1 = initList();
    searchForFiles(list1, argv[1]);
    //printList(list1);

    listHead *list2 = initList();
    searchForFiles(list2, argv[2]);
    //printList(list2);

    compareFiles(list1, list2);

	fclose(outputFile);
}
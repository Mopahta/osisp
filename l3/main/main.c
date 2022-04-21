#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>

#include <sys/stat.h>
#include <sys/wait.h>

#include "stack.h"
#include "list.h"

long maxChildProcesses;

void updateProcStatus(listHead *list){
    listEl *tmp = list->head;

    while (tmp){
        waitpid(tmp->pid, &(tmp->status), WNOHANG);
        tmp = tmp->next;
    }
}

void deleteTerminated(listHead *list){
    while (list->head && WIFEXITED(list->head->status)){
        deleteFirstEl(list);
    }
    listEl *tmp = list->head;
    
    while (list->amount > 1 && tmp->next){
        if (WIFEXITED(tmp->next->status)){
            list->amount--;
            listEl *toDelete = tmp->next;
            tmp->next = tmp->next->next;
            free(toDelete);
        }
        else {
            tmp = tmp->next;
        }
    }
}

int countWordsInFile(char *filename){
    FILE *file = fopen(filename, "r");
    if (!file){
        fprintf(stderr, "Couldn't open file %s\n", filename);
        return 1;
    }

    char wordDelimiters[] = " \n\r";

    int bytesRead = -1, wordsCount = 0;

    char a = 0, wordPresent = 0;
    while (a != EOF){
        a = getc(file);
        bytesRead++;

        if (wordPresent && (strchr(wordDelimiters, a) != NULL)){
            wordsCount++;
            wordPresent = 0;
        }
        else if (strchr(wordDelimiters, a) == NULL) {
            wordPresent = 1;
        }
    }

    wordsCount += wordPresent;

    int code = fclose(file);

    if (code){
        fprintf(stderr, "Couldn't close file %s\n", filename);
    }

    char *buf = (char *) malloc(sizeof(char)*PATH_MAX);
    char *fullpath = realpath(filename, NULL);

    sprintf(buf, "Process PID %d finished counting in file:\n%s\n\tBytes read: %d\n\tWords amount: %d\n", 
            getpid(), fullpath, bytesRead, wordsCount);
    free(fullpath);

    write(1, buf, strlen(buf));
    free(buf);
    return 0;
}

char *completeDir(char *path){
    if (path[strlen(path)-1] == '/'){
		return path;
	}
	else
		return strcat(path, "/");
}

void processDir(char *path, struct stack_t *stack, listHead *list){
    struct dirent *item;
    DIR *dir = opendir(path);

    if (dir == NULL){
        fprintf(stderr, "Couldn't open directory: %s\n", path);
        return;
    }

    while ((item = readdir(dir)) != NULL){
        if (strcmp(item->d_name, ".") & strcmp(item->d_name, "..")){
            if (item->d_type == DT_REG){
                pid_t pid = fork();
                if (!pid){
                    char *tempDir = copyString(path);
                    tempDir = completeDir(tempDir);
                    strcat(tempDir, item->d_name);
                    countWordsInFile(tempDir);
                    free(path);
                    free(tempDir);
                    exit(EXIT_SUCCESS);
                }
                else if (pid > 0){
                    addToList(list, 1, pid);
                    while (list->amount >= maxChildProcesses){
                        updateProcStatus(list);
                        deleteTerminated(list);
                    }
                }
                else {
                    fprintf(stderr, "Process forking failed.\n");
                }
            }
            else if (item->d_type == DT_DIR){
                char *tempDir = (char *) malloc(sizeof(char)*PATH_MAX);
                strcpy(tempDir, path);
                tempDir = completeDir(tempDir);
                push(stack, strcat(tempDir, item->d_name));
                free(tempDir);
            }
        }
    }

    closedir(dir);
}

void analyzeFiles(char *path){
    struct stack_t *stack = newStack();
    listHead *list = initList();

    path = completeDir(path);
    push(stack, path);

    while(stack->head){
        char *currPath = pop(stack);
        processDir(currPath, stack, list);
        free(currPath);
    }
    clearStack(stack);
    freeList(list);
}

int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr, "Not enough parameters.\nUsage:\n\t%s <dir> <concurrent_processes>\n", argv[0]);
        return 1;
    }

    char *endptr;
    maxChildProcesses = strtol(argv[2], &endptr, 10);

    if (errno != 0) {
        if (errno == ERANGE && maxChildProcesses == LONG_MAX){
            fprintf(stderr, "Value overflow.\n");
        }
        else if (errno == ERANGE && maxChildProcesses == LONG_MIN){
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

    char *path = (char *) malloc(sizeof(char) * PATH_MAX);
    strcpy(path, argv[1]);

    analyzeFiles(path);

    free(path);
    return 0;
}
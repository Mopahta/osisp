#include <stdlib.h>
#include <stdio.h>

typedef struct fileInfo{
    char* name;
    off_t size;
    ino_t inode;
    time_t cr_time;
    mode_t acc_mode;
} fileInfo;

typedef struct listEl{
    fileInfo info;
    struct listEl *next;
} listEl;

typedef struct listHead{
    listEl *head;
    int amount;
} listHead;

listHead* initList(){
    listHead *list = (listHead *) malloc(sizeof(listHead));
    if (list){
        list->head = NULL;
        list->amount = 0;
    }
    return list;
}

void addToList(listHead *list, fileInfo info){
    if (list->head){
        listEl *listItem = list->head;
        listEl *newEl = (listEl *) malloc(sizeof(listEl));
        newEl->info = info;
        newEl->next = NULL;

        while(listItem->next != NULL) 
            listItem = listItem->next;
        listItem->next = newEl;
    }
    else{
        list->head = (listEl *) malloc(sizeof(listEl));
        list->head->info = info;
        list->head->next = NULL;
    }
    list->amount++;
}

void freeList(listHead *list){
    listEl *tmp = list->head;
    while (list->head){
        tmp = list->head;
        list->head = list->head->next;
        free(tmp->info.name);
        free(tmp);
    }
}

void printList(listHead *list){
    listEl *tmp = list->head;
    while(tmp){
        printf("%s %i\n", tmp->info.name, tmp->info.size);
        tmp = tmp->next;
    }
}

void deleteFirstEl(listHead *list){
    if (list->head){
        listEl *tmp = list->head;
        list->head = list->head->next;
        free(tmp->info.name);
        free(tmp);
    }
}
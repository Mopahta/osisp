#include <stdlib.h>
#include <stdio.h>

typedef struct listEl{
    int status;
    int pid;
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

listEl* addToList(listHead *list, int childStatus, int pid){
    list->amount++;
    if (list->head){
        listEl *listItem = list->head;
        listEl *newEl = (listEl *) malloc(sizeof(listEl));
        newEl->status = childStatus;
        newEl->pid = pid;
        newEl->next = NULL;

        while(listItem->next != NULL) 
            listItem = listItem->next;
        listItem->next = newEl;
        return newEl;
    }
    else{
        list->head = (listEl *) malloc(sizeof(listEl));
        list->head->status = childStatus;
        list->head->pid = pid;
        list->head->next = NULL;
        return list->head;
    }
}

void freeList(listHead *list){
    listEl *tmp = list->head;
    while (list->head){
        tmp = list->head;
        list->head = list->head->next;
        free(tmp);
    }
}

void printList(listHead *list){
    listEl *tmp = list->head;
    printf("Amount of elements: %i\n", list->amount);
    while(tmp){
        printf("PID: %i Status: %i\n", tmp->pid, tmp->status);
        tmp = tmp->next;
    }
}

void deleteFirstEl(listHead *list){
    if (list->head){
        listEl *tmp = list->head;
        list->head = list->head->next;
        list->amount--;
        free(tmp);
    }
}
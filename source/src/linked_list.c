#include "../include/linked_list.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

struct node{
    void* item;
    struct node* next;
}; 

typedef struct linked_list{
    struct node* head;
    struct node* tail;
    int size;
    int sizeItem;
} S_LinkedList;

S_LinkedList* create_list(int sizeItem){
    S_LinkedList* list = (S_LinkedList*) malloc(sizeof(S_LinkedList));
    struct node* new_node = (struct node*)malloc(sizeof(struct node));
    new_node->item = malloc(sizeItem);
    new_node->next = NULL;

    list->head = new_node;
    list->tail = list->head;
    list->size = 0;
    list->sizeItem = sizeItem;

    return list;
}

void push(S_LinkedList* list, void* element){
    struct node* node = (struct node*)malloc(sizeof(struct node));
    node->item = malloc(list->sizeItem);

    memcpy(node->item,element,list->sizeItem);
    
    list->tail->next = node;
    list->tail = node;
    list->size++;
}

void pop(S_LinkedList* list, void* element){
    struct node* node = (struct node*) malloc(sizeof(struct node));

    if(!list->size){
        return;
    }

    node = list->head->next;
    list->head->next = node->next;
    list->size --;
    if(list->head->next == NULL){
        list->tail = list->head;
    }
    memcpy(element,node->item,list->sizeItem);
    free(node);
}

void getItem(S_LinkedList* list, int index, void* element){
    struct node* curr = list->head;
    while(index != 0 && curr != NULL){
        curr = curr->next;
        index--;
    }
    memcpy(element,curr->item,list->sizeItem);
}

void setItem(S_LinkedList* list, int index, void* element){
    struct node* curr = list->head;
    while(index != 0 && curr != NULL){
        curr = curr->next;
        index--;
    }
    memcpy(curr->item,element,list->sizeItem);
}

int getSize(S_LinkedList* list){
    return list->size;
}
#include "../include/queue.h"

#include <stdlib.h>
#include <string.h>

typedef struct cell{
    void* data;
    struct cell* next;
} cell;

typedef struct queue{
    cell* head;
    cell* tail;
    int size;
    size_t sizeofItem;
} S_Queue;

S_Queue* create_queue(int sizeofData){
    S_Queue* queue = (S_Queue*) malloc(sizeof(S_Queue));


    queue->head = (cell*)malloc(sizeof(cell));
    queue->tail = queue->head;
    queue->size = 0;
    queue->sizeofItem = sizeofData;
    
    return queue;
}

void enqueue(S_Queue* queue,void* data){
    cell* newCell = (cell*) malloc(sizeof(cell));
    
    memcpy(newCell->data,data,queue->sizeofItem);
    queue->tail->next = newCell;
    queue->tail = newCell;
    queue->size++;
}

void* dequeue(S_Queue* queue){
    cell* auxCell;
    void* data;
    
    // data = queue->head->next->data;
    memcpy(data,queue->head->next->data,queue->sizeofItem);
    auxCell = queue->head;
    queue->head = queue->head->next;
    free(auxCell);
    queue->size--;

    return data;
}

int isEmpty(S_Queue* queue){
    return !queue->size;
}
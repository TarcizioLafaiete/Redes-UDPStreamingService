#include "../include/queue.h"

typedef struct{
    datagram data;
    cell* next;
} cell;

typedef struct queue{
    cell* head;
    cell* tail;
    int size;
} S_Queue;

S_Queue* create_queue(){
    S_Queue* queue = (S_Queue*) malloc(sizeof(S_Queue));


    queue->head = (cell*)malloc(sizeof(cell));
    queue->tail = queue->head;
    queue->size = 0;
    
    return queue;
}

void enqueue(S_Queue* queue,datagram data){
    cell* newCell = (cell*) malloc(sizeof(cell));
    
    newCell->data = data;
    queue->tail->next = newCell;
    queue->tail = newCell;
    queue->size++;
}

datagram dequeue(S_Queue* queue){
    cell* auxCell;
    datagram data;
    
    data = queue->head->next->data;
    auxCell = queue->head;
    queue->head = queue->head->next;
    free(auxCell);
    queue->size--;

    return data;
}

int isEmpty(S_Queue* queue){
    return !queue->size;
}
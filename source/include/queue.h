#ifndef __QUEUE__
#define __QUEUE__


typedef struct queue S_Queue;

S_Queue* create_queue();
void enqueue(S_Queue* queue,void* data);
void* dequeue(S_Queue* queue);
int isEmpty(S_Queue* queue);


#endif
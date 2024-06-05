#ifndef __LINKED_LIST__
#define __LINKED_LIST__

typedef struct linked_list S_LinkedList;

S_LinkedList* create_list(int sizeItem);
void push(S_LinkedList* list, void* element);
void pop(S_LinkedList* list, void* element);
void getItem(S_LinkedList* list, int index, void* element);
void setItem(S_LinkedList* list, int index, void* element);
int getSize(S_LinkedList* list);

#endif
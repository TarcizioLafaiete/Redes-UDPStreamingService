#include "../include/vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

typedef struct cVector{
    void* data;
    int size;
    size_t elementSize;
} S_cVector;

void expandVector(S_cVector* vector){

    vector->size++;
    void* newData = malloc(vector->size * vector->elementSize);

    if(vector->data != NULL){
        memcpy(newData,vector->data,vector->size*vector->elementSize);
        free(vector->data);
    }
    vector->data = newData;
}

void reduceVector(S_cVector* vector,void* element){

    if(vector->size == 1){
        memcpy(element,vector->data,vector->elementSize);
        free(vector->data);
        return;
    }
    else if(vector->size == 0){
        return;
    }

    void* newData = malloc((vector->size - 1) * vector->elementSize);
    memcpy(newData,vector->data + vector->elementSize,vector->size*vector->elementSize);
    memcpy(element,vector->data,vector->elementSize);
    free(vector->data);
    vector->data = newData;
    vector->size--;
}

S_cVector* create_vector(int elementSize){
    S_cVector* vector = (S_cVector*)malloc(sizeof(S_cVector));
    vector->data = NULL;
    vector->size = 0;
    vector->elementSize = elementSize;
    return vector;
}

void getElementVector(S_cVector* vector, int index,void* element){
    if(index < 0 || index >= vector->size){
        return;
    }
    memcpy(element,vector->data + index*vector->elementSize,vector->elementSize);
}

void setElementVector(S_cVector* vector, int index, void* element){
    if(index < 0 || index >= vector->size){
        return;
    }
    memcpy(vector->data + index*vector->elementSize,element,vector->elementSize);
}


void push(S_cVector* vector, void* element){
    expandVector(vector);
    memcpy(vector->data + (vector->size-1)*vector->elementSize,element,vector->elementSize);
}

void pop(S_cVector* vector,void* element){
    reduceVector(vector,element);
}

int getSize(S_cVector* vector){
    return vector->size;
}
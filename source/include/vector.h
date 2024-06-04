#ifndef __VECTOR__
#define __VECTOR__

typedef struct cVector S_cVector;

S_cVector* create_vector(int elementSize);
void getElementVector(S_cVector* vector, int index,void* element);
void setElementVector(S_cVector* vector,int index,void* element);
void push(S_cVector* vector, void* element);
void pop(S_cVector* vector,void* element);
int getSize(S_cVector* vector);


#endif
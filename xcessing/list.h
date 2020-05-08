#ifndef _XCESSING_LIST_H
#define _XCESSING_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _list_t {
    void* data;
    int length;
    int element_size;
    int capacity;
}  list_t;
#define List list_t*

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define newList(a)          (newListOfElementSize(sizeof(a)))
#define listAddValue(a,b,c) {b x = c;listAdd(a,&x);}
#define listSetValue(a,b,c) {b x = c;listSet(a,&x);}
#define listGetValue(a,b,c) (*(b*)listGet(a,c))
#define printList(a,b,c)    {for (int i = 0; i < a->length; i++){printf(c,listGetValue(a,b,i));};printf("\n");}

List newListOfElementSize(int element_size){
    List a = malloc(sizeof(list_t));
    a->data = malloc(element_size*8);
    a->capacity = 8;
    a->element_size = element_size;
    a->length = 0;
    return a;
}

void destroyList(List a){
    free(a->data);
    free(a);
}

void listAdd(List a, void* x){
    if (a->length >= a->capacity){
        a->capacity = a->capacity+MIN(4,a->capacity/2);
        a->data = realloc(a->data, a->element_size*(a->capacity));
    }
    memcpy(a->data + a->length * a->element_size, x, a->element_size);
    a->length ++;
}

void* listGet(List a, int i){
    return a->data + i * a->element_size;
}

void listSet(List a, int i, void* x){
    memcpy(a->data + i * a->element_size, x, a->element_size);
}

void listRemove(List a, int i){
    memcpy(a->data + i * a->element_size, a->data + (i+1) * a->element_size, (a->length-i) * a->element_size);
    a->length --;
}

List listSlice(List a, int i, int j){
    List b = newList(a->element_size);
    free(b->data);
    b->length = j-i;
    b->capacity = j-i;
    b->data = malloc(b->element_size * b->capacity);
    memcpy(b->data, a->data+i*a->element_size, (j-i)*a->element_size);
    return b;
}

void listAddAll(List a, List b){
    if (a->length + b->length > a->capacity){
        a->capacity = a->length + b->length;
        a->data = realloc(a->data, a->element_size*(a->capacity));
    }
    memcpy(a->data + a->length * a->element_size, b->data, b->length * b->element_size);
    a->length = a->length + b->length;
}

void listClear(List a){
    free(a->data);
    a->data = malloc((a->element_size)*8);
    a->length = 0;
    a->capacity = 8;
}

void listReverse(List a){
    for (int i = 0; i < a->length/2; i++){
        char* _a = (char*) a->data + i * a->element_size;
        char* b  = (char*) a->data + (a->length-i-1) * a->element_size;
        char* p;
        char* q;
        char* const sentry = (char*)_a + a->element_size;

        for ( p = _a, q = b; p < sentry; ++p, ++q ) {
            char t = *p;
            *p = *q;
            *q = t;
        }
    }
}

#endif
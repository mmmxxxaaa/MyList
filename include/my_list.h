#ifndef MY_LIST_H_
#define MY_LIST_H_

#include <stdio.h>
#include "list_error_types.h"

#define FREE_AND_NULL(ptr)      \
    do {                        \
        free(ptr);              \
        (ptr) = NULL;           \
    } while(0)

typedef struct {
    int data;
    int next;
    int prev;
} ElementInList;

typedef struct {
    ElementInList* array;
    int free;
    int capacity;
} List;

const int kPoison                 = 525252;
const int KMaxLengthOfFilename    = 256;
const int kMaxSystemCommandLength = 512;

ListErrorType ListCtorWithSpecifiedCapacity(List* ptr_list_struct, int capacity);
ListErrorType ListDtor(List* ptr_list_struct);

ListErrorType ListInsertAfter (List* list, int target_index, int value);
ListErrorType ListDelete(List* list, int index);

ListErrorType ListDump(List* ptr_list_struct, const char* filename);
ListErrorType InitListLog(const char* filename);
ListErrorType CloseListLog(const char* filename);


#endif //MY_LIST_H_

#ifndef MY_LIST_H_
#define MY_LIST_H_

#include <stdio.h>
#include "list_error_types.h"


#define FREE_AND_NULL(ptr)      \
    do {                        \
        free(ptr);              \
        (ptr) = NULL;           \
    } while(0)

typedef int DataType;

typedef struct {
    DataType data;
    int next;
    int prev;
} ElementInList;

typedef struct {
    ElementInList* array;
    int free;
    int capacity;
    int size;
} List;

const DataType kPoison                      = 525252;
const int      kMaxLengthOfFilename         = 256;
const int      kMaxSystemCommandLength      = 512;
const int      kFictiveElementIndex         = 0;
const int      kCapacityIncreaseCoefficient = 2;

int IsElementFree(List* list, int index);

ListErrorType ListCtorWithSpecifiedCapacity(List* ptr_list_struct, int capacity);
ListErrorType ListDtor(List* ptr_list_struct);
ListErrorType ListRealloc(List* list, int new_capacity);

ListErrorType ListInsertAfter          (List* list, int target_index, int value);
ListErrorType ListDeleteAt             (List* list, int index);
ListErrorType ListInsertBeforeHead     (List* list, int value); //для программиста же неочевидно, что надо писать ListInsertAfter(&list, 0, value)
ListErrorType ListInsertAfterTail      (List* list, int value);
ListErrorType ListInsertTheFirstElement(List* list, int value); //для программиста же неочевидно, что надо писать ListInsertAfter(&list, 0, value)
DataType      GetIndexOfHead(List* list);
DataType      GetIndexOfTail(List* list);


ListErrorType ListDump(List* ptr_list_struct, const char* filename);
ListErrorType InitListLog(const char* filename);
ListErrorType CloseListLog(const char* filename);
//FIXME в нормальном случае стрелочки должны быть двойными и черными (см тетрадь короче)
const char* VerifyResultToString(VerifyResult result);
VerifyResult DetectCycle(List* list);
VerifyResult VerifyList(List* list);

#endif //MY_LIST_H_

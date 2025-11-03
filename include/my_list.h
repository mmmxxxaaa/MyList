#ifndef MY_LIST_H_
#define MY_LIST_H_

#include <stdio.h>
#include <time.h>
#include "list_error_types.h"


#define FREE_AND_NULL(ptr)      \
    do {                        \
        free(ptr);              \
        (ptr) = NULL;           \
    } while(0)

typedef int DataType;

typedef struct {
    DataType data;
    ssize_t next;
    ssize_t prev;
} ElementInList;

typedef struct {
    ElementInList* array;
    ssize_t free;
    ssize_t capacity;
    ssize_t size;
} List;
//ХУЙНЯ хранить наибольший индекс USED и при реаллокации вниз освобождать все те индексы, которые правее used
        //тогда DeleteAt будет O(n), поэтому лучше находить этот max_used_index в функции реаллокации вниз
const DataType kPoison                      = 525252;
const int      kMaxLengthOfFilename         = 256;
const int      kMaxSystemCommandLength      = 512;
const ssize_t   kFictiveElementIndex        = 0;
const int      kCapacityIncreaseCoefficient = 2;

int IsElementFree(List* list, ssize_t index);

ListErrorType ListCtorWithSpecifiedCapacity(List* ptr_list_struct, ssize_t capacity);
ListErrorType ListDtor                     (List* ptr_list_struct);

ssize_t       FindMaxUsedIndexInList (List* list);
ListErrorType ListReallocUp          (List* list, ssize_t new_capacity);
ListErrorType ListReallocDown        (List* list, ssize_t new_capacity);
ListErrorType ListReallocDownAsPossibleAsYouCan(List* list);
ListErrorType ListShrinkToFit        (List* list);

ListErrorType ListInsertAfter     (List* list, ssize_t target_index, DataType value);
ListErrorType ListDeleteAt        (List* list, ssize_t index);
ListErrorType ListInsertBeforeHead(List* list, DataType value); //для программиста же неочевидно, что надо писать ListInsertAfter(&list, 0, value)
ListErrorType ListInsertAfterTail (List* list, DataType value);
ListErrorType ListLinearize       (List* list);
//FIXME next  prev говорят индексы ИСПОЛЬЗОВАТЬ эти функции в реализации других функций
//FIXME линеаризация: она не реаллоцирует память, она просто работает с индексами
//FIXME     выделить еще один массив, туда всё из старого напихать подряд, освободить старое, заполняю все, что надо

ListErrorType ListInsertTheFirstElement(List* list, DataType value); //для программиста же неочевидно, что надо писать ListInsertAfter(&list, 0, value)
ssize_t       GetIndexOfHead(List* list);
ssize_t       GetIndexOfTail(List* list);

//====================DUMP=========================================
ListErrorType ListDump(List* ptr_list_struct, const char* filename);
//---------------HTM-----------------------------------------------
ListErrorType ListDumpToHtm(List* list, FILE* htm_file, const char* folder_name);
void WriteDumpHeader(FILE* htm_file, time_t now);
void WriteListInfo(FILE* htm_file, List* list);
void WriteElementsInTable(FILE* htm_file, List* list);
const char* GetElementStatus(List* list, int index);
//----------------DOT-----------------------------------------------
ListErrorType GenerateGraphVisualization(List* list, FILE* htm_file, const char* folder_name, time_t now);
ListErrorType GenerateDotFile(List* list, const char* filename);
void CreateDotNodes(List* list, FILE* dot_file);
void CreateInvisibleElementConnections(List* list, FILE* dot_file);
void CreateCommonElementConnections(List* list, FILE* dot_file);
void CreateFreeElementConnections(List* list, FILE* dot_file);
//==================================================================


ListErrorType InitListLog(const char* filename);
ListErrorType CloseListLog(const char* filename);

const char* VerifyResultToString(VerifyResult result);
VerifyResult DetectCycle(List* list);
VerifyResult VerifyList(List* list);

#endif //MY_LIST_H_

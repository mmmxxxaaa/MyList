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

//====================DUMP=======================================
ListErrorType ListDump(List* ptr_list_struct, const char* filename);
//---------------HTM-----------------------
ListErrorType ListDumpToHtm(List* list, FILE* htm_file, const char* folder_name);
void WriteDumpHeader(FILE* htm_file, time_t now);
void WriteListInfo(FILE* htm_file, List* list);
void WriteElementsInTable(FILE* htm_file, List* list);
const char* GetElementStatus(List* list, int index);
//----------------DOT-------------------------------
ListErrorType GenerateGraphVisualization(List* list, FILE* htm_file, const char* folder_name, time_t now);
ListErrorType GenerateDotFile(List* list, const char* filename);
void CreateDotNodes(List* list, FILE* dot_file);
void CreateInvisibleElementConnections(List* list, FILE* dot_file);
void CreateCommonElementConnections(List* list, FILE* dot_file);
void CreateFreeElementConnections(List* list, FILE* dor_file);
//==================================================================


ListErrorType InitListLog(const char* filename);
ListErrorType CloseListLog(const char* filename);
//FIXME в нормальном случае стрелочки должны быть двойными и черными (см тетрадь короче)
const char* VerifyResultToString(VerifyResult result);
VerifyResult DetectCycle(List* list);
VerifyResult VerifyList(List* list);

#endif //MY_LIST_H_

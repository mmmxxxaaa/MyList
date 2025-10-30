#include "list_tests.h"

#include <assert.h>
#include <stdio.h>

#include "my_list.h"

//Вставка первого элемента в пустой список
void TestFirstInsert()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 5);

    const char* folder_name = "log_TestFirstInsert";
    InitListLog(folder_name);

    ListDump(&list, folder_name);
    ListErrorType result = ListInsertTheFirstElement(&list, 100);
    ListDump(&list, folder_name);

    CloseListLog(folder_name);
    ListDtor(&list);
}

//Вставка нескольких элементов и удаление среднего
void TestMultipleInsertionsAndDeleteMiddle()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 7);

    const char* folder_name = "log_TestMultipleInsertionsAndDeleteMiddle";
    InitListLog(folder_name);

    ListInsertTheFirstElement(&list, 10);
    ListDump(&list, folder_name);

    ListInsertAfter(&list, 1, 20);
    ListDump(&list, folder_name);

    ListInsertAfter(&list, 2, 30);
    ListDump(&list, folder_name);

    ListInsertAfter(&list, 3, 40);
    ListDump(&list, folder_name);

    ListDeleteAt(&list, 2);
    ListDump(&list, folder_name);

    ListInsertAfter(&list, 4, 50);
    ListDump(&list, folder_name);

    CloseListLog(folder_name);
    ListDtor(&list);
}

// Удаление головы
void TestDeleteHead()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 10);

    const char* folder_name = "log_TestDeleteHead";
    InitListLog(folder_name);

    ListInsertTheFirstElement(&list, 10);
    ListInsertAfter(&list, 1, 20);
    ListInsertAfter(&list, 2, 30);

    ListDump(&list, folder_name);
    ListErrorType result = ListDeleteAt(&list, 1);
    ListDump(&list, folder_name);
    CloseListLog(folder_name);

    ListDtor(&list);
}

// Удаление хвоста
void TestDeleteTail()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 10);

    const char* folder_name = "log_TestDeleteTail";
    InitListLog(folder_name);

    ListInsertTheFirstElement(&list, 10);
    ListInsertAfter(&list, 1, 20);
    ListInsertAfter(&list, 2, 30);

    ListDump(&list, folder_name);
    ListErrorType result = ListDeleteAt(&list, 3);
    ListDump(&list, folder_name);

    CloseListLog(folder_name);
    ListDtor(&list);
}

// Удаление последнего оставшегося элемента
void TestDeleteLastRemainingElement()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 5);

    const char* folder_name = "log_TestDeleteLastRemainingElement"; //FIXME везде так
    InitListLog(folder_name);

    ListInsertTheFirstElement(&list, 100);
    ListDump(&list, folder_name);

    ListErrorType result = ListDeleteAt(&list, 1);
    ListDump(&list, folder_name);

    CloseListLog(folder_name);
    ListDtor(&list);
}

// Тест реаллокации
void TestReallocation()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 3);
    const char* folder_name = "log_TestReallocation";
    InitListLog(folder_name);

    // Заполняем список
    ListInsertTheFirstElement(&list, 10);
    ListInsertAfter(&list, 1, 20);
    ListDump(&list, folder_name);

    ListErrorType result = ListInsertAfter(&list, 2, 30);
    ListDump(&list, folder_name);

    CloseListLog(folder_name);
    ListDtor(&list);
}

// Вставка после удаления (повторное использование памяти)
void TestReuseAfterDelete()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 5);

    const char* folder_name = "log_TestReuseAfterDelete";
    InitListLog(folder_name);

    ListInsertTheFirstElement(&list, 10);
    ListInsertAfter(&list, 1, 20);
    ListDump(&list, folder_name);

    int first_element_index = 1;

    ListDeleteAt(&list, first_element_index);
    ListDump(&list, folder_name);

    // Вставляем новый элемент на место удаленного
    ListInsertAfter(&list, 0, 30);
    ListDump(&list, folder_name);

    CloseListLog(folder_name);
    ListDtor(&list);
}

void AllTests()
{
    TestFirstInsert();
    TestMultipleInsertionsAndDeleteMiddle();
    TestDeleteHead();
    TestDeleteTail();
    TestDeleteLastRemainingElement();
    TestReuseAfterDelete();
    TestReallocation();
}

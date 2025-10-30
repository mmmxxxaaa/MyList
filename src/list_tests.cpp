#include "list_tests.h"

#include <assert.h>
#include <stdio.h>

#include "my_list.h"
// #include <string.h>

// Тест 1: Вставка первого элемента в пустой список
void TestFirstInsert()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 5);

    // Вставляем первый элемент после poison
    ListDump(&list, "log_TestFirstInsert.htm");
    ListErrorType result = ListInsertTheFirstElement(&list, 100);
    ListDump(&list, "log_TestFirstInsert.htm");

    ListDtor(&list);
}

// Тест 2: Вставка нескольких элементов и удаление среднего
void TestMultipleInsertionsAndDeleteMiddle()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 7);
    InitListLog("log_TestMulInsertionsAndDeleteMiddle.htm");

    ListInsertTheFirstElement(&list, 10);
    ListDump(&list, "log_TestMulInsertionsAndDeleteMiddle.htm");

    ListInsertAfter(&list, 1, 20);
    ListDump(&list, "log_TestMulInsertionsAndDeleteMiddle.htm");

    ListInsertAfter(&list, 2, 30);
    ListDump(&list, "log_TestMulInsertionsAndDeleteMiddle.htm");

    ListInsertAfter(&list, 3, 40);
    ListDump(&list, "log_TestMulInsertionsAndDeleteMiddle.htm");

    ListDeleteAt(&list, 2);
    ListDump(&list, "log_TestMulInsertionsAndDeleteMiddle.htm");

    ListInsertAfter(&list, 4, 50);
    ListDump(&list, "log_TestMulInsertionsAndDeleteMiddle.htm");

    CloseListLog("log_TestMulInsertionsAndDeleteMiddle.htm");
    ListDtor(&list);
}


// Тест 3: Удаление головы
void TestDeleteHead()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 10);

    ListInsertTheFirstElement(&list, 10);
    ListInsertAfter(&list, 1, 20);
    ListInsertAfter(&list, 2, 30);

    ListDump(&list, "log_TestDeleteHead.htm");
    ListErrorType result = ListDeleteAt(&list, 1);
    ListDump(&list, "log_TestDeleteHead.htm");

    ListDtor(&list);
}

// Тест 4: Удаление хвоста
void TestDeleteTail()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 10);

    ListInsertTheFirstElement(&list, 10);
    ListInsertAfter(&list, 1, 20);
    ListInsertAfter(&list, 2, 30);

    ListDump(&list, "log_TestDeleteTail.htm");
    ListErrorType result = ListDeleteAt(&list, 3);
    ListDump(&list, "log_TestDeleteTail.htm");

    ListDtor(&list);
}

// Тест 5: Удаление последнего оставшегося элемента
void TestDeleteLastRemainingElement()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 5);

    // Вставляем один элемент
    ListInsertTheFirstElement(&list, 100);
    ListDump(&list, "log_TestDeleteLastRemainingElement.htm");

    // УДАЛЯЕМ ПОСЛЕДНИЙ ЭЛЕМЕНТ
    ListErrorType result = ListDeleteAt(&list, 1);
    ListDump(&list, "log_TestDeleteLastRemainingElement.htm");

    ListDtor(&list);
}

// //FIXME реаллокация
// void TestOverflow() {
//     printf("Test 9: List Overflow\n");
//
//     List list = {};
//     ListCtorWithSpecifiedCapacity(&list, 3); // Маленькая емкость
//
//     // Заполняем список
//     ListInsertAfter(&list, 0, 10); // Используем 1
//     ListInsertAfter(&list, 1, 20); // Используем 2
//
//     // Попытка вставить третий элемент (должна завершиться ошибкой)
//     ListErrorType result = ListInsertAfter(&list, 2, 30);
//     assert(result == LIST_NO_FREE_SPACE);
//
//     ListDtor(&list);
//     printf("✓ PASSED\n\n");
// }

// Тест Вставка после удаления (повторное использование памяти)
void TestReuseAfterDelete()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 5);

    ListInsertTheFirstElement(&list, 10);
    ListInsertAfter(&list, 1, 20);
    ListDump(&list, "log_TestReuseAfterDelete.htm");

    int first_element_index = 1;

    // Удаляем первый элемент
    ListDeleteAt(&list, first_element_index);
    ListDump(&list, "log_TestReuseAfterDelete.htm");

    // Вставляем новый элемент - должен использовать освободившуюся память
    ListInsertAfter(&list, 0, 30);
    ListDump(&list, "log_TestReuseAfterDelete.htm");

    ListDtor(&list);
}

void AllTests()
{
    TestFirstInsert();
    TestMultipleInsertionsAndDeleteMiddle();
    TestDeleteHead();
    TestDeleteTail();
    TestDeleteLastRemainingElement();     // Самый важный тест!
    TestReuseAfterDelete();
}

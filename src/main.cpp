#include <stdio.h>

#include "my_list.h"
int main()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 10);

    // список: 10 -> 20 -> 30 -> 40
    ListInsertAfter(&list, 0, 10);
    ListInsertAfter(&list, 1, 20);
    ListInsertAfter(&list, 2, 30);
    ListInsertAfter(&list, 3, 40);

    // дамп до удаления
    ListDump(&list, "before_delete.txt");

    // удаляем элемент с индексом 2 (значение 20)
    ListDelete(&list, 2);

    // дамп после удаления
    ListDump(&list, "after_delete.txt");

    ListInsertAfter(&list, 4, 50);
    ListDump(&list, "after_new_insert.txt");

    ListDtor(&list);
    return 0;

}

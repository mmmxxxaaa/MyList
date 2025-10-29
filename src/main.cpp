#include <stdio.h>

#include "my_list.h"
int main()
{
    List list = {};
    ListCtorWithSpecifiedCapacity(&list, 7);
    InitListLog("list_log.htm");

    ListInsertAfter(&list, 0, 10);
    ListInsertAfter(&list, 1, 20);
    ListInsertAfter(&list, 2, 30);
    ListInsertAfter(&list, 3, 40);

    ListDump(&list, "list_log.htm");

    ListDelete(&list, 2);

    ListDump(&list, "list_log.htm");

    ListInsertAfter(&list, 4, 50);
    ListDump(&list, "list_log.htm");

    CloseListLog("list_log.htm");
    ListDtor(&list);
    return 0;

}

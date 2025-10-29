#include "my_list.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include <string.h>

static int IsElementFree(List* list, int index)
{
    assert(list);

    if (index < 0 || index >= list->capacity)
        return 0;

    int free_idx = list->free;
    while (free_idx != -1)
    {
        if (free_idx == index)
            return 1;

        free_idx = list->array[free_idx].next;
    }
    return 0;
}

ListErrorType ListCtorWithSpecifiedCapacity(List* ptr_list_struct, int capacity)
{
    assert(ptr_list_struct);

    if (capacity <= 0)
        return LIST_WRONG_SPECIFIED_CAPACITY;

    ptr_list_struct->array = (ElementInList*) calloc(capacity, sizeof(ElementInList));
    if (ptr_list_struct->array == NULL)
        return LIST_ALLOCATION_FAILED;
    ptr_list_struct->capacity = capacity;

    // инициализация poisonа на нулевом индексе
    ptr_list_struct->array[0].data = kPoison;
    ptr_list_struct->array[0].next = 0;
    ptr_list_struct->array[0].prev = 0;

    for (int i = 1; i < capacity; i++)
    {
        ptr_list_struct->array[i].data = kPoison;
        ptr_list_struct->array[i].next = (i == capacity - 1) ? -1 : i + 1;
        ptr_list_struct->array[i].prev = -1;
    }

    ptr_list_struct->free = 1;

    return LIST_ERROR_NO;
}

ListErrorType ListDtor(List* ptr_list_struct)
{
    assert(ptr_list_struct);

    FREE_AND_NULL(ptr_list_struct->array);

    return LIST_ERROR_NO;
}

ListErrorType ListInsertAfter(List* list, int target_index, int value)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    if (target_index < 0 || target_index >= list->capacity)
        return LIST_INVALID_INDEX;

    if (IsElementFree(list, target_index) && target_index != 0) // FIXME цикл есть или нет?
        return LIST_INVALID_INDEX;

    if (list->free == -1)
        return LIST_NO_FREE_SPACE;

    int new_index = list->free;
    list->free = list->array[new_index].next;

    list->array[new_index].data = value;

    list->array[new_index].prev = target_index;
    list->array[new_index].next = list->array[target_index].next;

    list->array[target_index].next = new_index;
    list->array[list->array[new_index].next].prev = new_index;

    return LIST_ERROR_NO;
}

ListErrorType ListDelete(List* list, int index)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    if (index < 1 || index >= list->capacity)
        return LIST_INVALID_INDEX;

    if (IsElementFree(list, index))  // FIXME цикл есть или нет? нельзя удалить уже свободный элемент
        return LIST_INVALID_INDEX;

    ElementInList* element = &list->array[index];

    list->array[element->prev].next = element->next;
    list->array[element->next].prev = element->prev;

    element->data = kPoison;
    element->prev = -1;
    element->next = list->free;
    list->free = index;

    return LIST_ERROR_NO;
}

ListErrorType ListDump(List* list, const char* filename)
{
    assert(filename);
    static int n_of_pictures = 0;

    FILE* htm_file = fopen(filename, "a");
    if (!htm_file)
        return LIST_ERROR_OPENING_FILE;

    time_t now = time(NULL);
    fprintf(htm_file, "<div style='border:2px solid #ccc; margin:10px; padding:15px; background:#f9f9f9;'>\n"); //создает красивый контейнер для одного дампа
    fprintf(htm_file, "<h2 style='color:#333;'>List Dump at %s</h2>\n", ctime(&now)); //время дампа

    // Базовая инфа
    fprintf(htm_file, "<div style='margin-bottom:15px;'>\n"); //margin -- внешний отступ, padding -- внутренний
    fprintf(htm_file, "<p><b>Capacity:</b> %d</p>\n", list->capacity);
    fprintf(htm_file, "<p><b>Free head:</b> %d</p>\n", list->free);
    fprintf(htm_file, "<p><b>Head index:</b> %d</p>\n", list->array[0].next); //мб еще size добавить
    fprintf(htm_file, "<p><b>Tail index:</b> %d</p>\n", list->array[0].prev);
    fprintf(htm_file, "</div>\n");

    char temp_dot[KMaxLengthOfFilename], temp_svg[KMaxLengthOfFilename];
    snprintf(temp_dot, sizeof(temp_dot), "temp_%d%ld.dot", n_of_pictures++, now);
    snprintf(temp_svg, sizeof(temp_svg), "temp_%d%ld.svg", n_of_pictures++, now);

    FILE* dot_file = fopen(temp_dot, "w");
    if (!dot_file)
    {
        fclose(htm_file);
        return LIST_ERROR_OPENING_FILE;
    }

//==============================РАБОТА С DOT========================================================
    fprintf(dot_file, "digraph DoublyLinkedList {\n");
    fprintf(dot_file, "    rankdir=LR;\n");
    fprintf(dot_file, "    node [shape=Mrecord, color = black];\n\n");

    for (int i = 0; i < list->capacity; i++)
    {
        ElementInList* element = &list->array[i];
        const char* color = "lightgreen";
        const char* label = "FREE";
        int is_free = IsElementFree(list, i);

        if (i == list->array[0].prev && i == list->array[0].next)
        {
            color = "lightpurple";
            label = "HEAD/TAIL";
        }
        else if (i == list->array[0].next)
        {
            color = "lightblue";
            label = "HEAD";
        }
        else if (i == list->array[0].prev)
        {
            color = "lightyellow";
            label = "TAIL";
        }
        else if (i == 0)
        {
            color = "lightcoral";
            label = "POISON";
        }
        else if (!is_free)
        {
            color = "white";
            label = "USED";
        }

        fprintf(dot_file, "    element%d [label=\"{%s|{idx: %d|data: %d|next: %d|prev: %d}}\", style=filled, fillcolor=%s, color=black];\n", //Внешние фигурные скобки создают основную таблицу символ | между элементами разделяет строки таблицы; Внутренние фигурные скобки создают вложенные таблицы/ячейки Символ | внутри внутренних скобок разделяет столбцы внутри строки
                i, label, i, element->data, element->next, element->prev, color);
    }

    fprintf(dot_file, "\n");
    for (int i = 0; i < list->capacity - 1; i++)
        fprintf(dot_file, "    element%d -> element%d [weight=100000, style=invis, color=white];\n", i, i+1);

    for (int i = 0; i < list->capacity; i++)
    {
        ElementInList* element = &list->array[i];
        if (IsElementFree(list, i) || i == 0)
            continue;

        if (element->next != -1)
            fprintf(dot_file, "    element%d -> element%d [color=blue, label=\"next\", constraint=false];\n", i, element->next);

        if (element->prev != -1)
            fprintf(dot_file, "    element%d -> element%d [color=red, label=\"prev\", style=dashed, constraint=false];\n", i, element->prev);
    }

    fprintf(dot_file, "\n");
    int free_idx = list->free;
    while (free_idx != -1 && free_idx < list->capacity)
    {
        ElementInList* element = &list->array[free_idx];
        if (element->next != -1)
            fprintf(dot_file, "    element%d -> element%d [color=gray, label=\"free\", constraint=false];\n", free_idx, element->next);
        free_idx = element->next;
    }

    fprintf(dot_file, "\n");
    fprintf(dot_file, "    head_ptr [shape=plaintext, label=\"head\"];\n");
    fprintf(dot_file, "    tail_ptr [shape=plaintext, label=\"tail\"];\n");
    fprintf(dot_file, "    free_ptr [shape=plaintext, label=\"free\"];\n");

    if (list->array[0].prev != -1)
        fprintf(dot_file, "    head_ptr -> element%d [color=darkblue];\n", list->array[0].next);

    if (list->array[0].next != -1)
        fprintf(dot_file, "    tail_ptr -> element%d [color=darkgreen];\n", list->array[0].prev);

    if (list->free != -1)
        fprintf(dot_file, "    free_ptr -> element%d [color=black];\n", list->free);

    fprintf(dot_file, "}\n");
    fclose(dot_file);
//==================================================================================================

    // создаем картинки
    char command[kMaxSystemCommandLength];
    snprintf(command, sizeof(command), "dot -Tsvg %s -o %s", temp_dot, temp_svg);
    int result = system(command);

    if (result == 0)
    {
        fprintf(htm_file, "<div style='text-align:center;'>\n");
        fprintf(htm_file, "<img src='%s' style='max-width:100%%; border:1px solid #ddd;'>\n", temp_svg);
        fprintf(htm_file, "</div>\n");
    }
    else
        fprintf(htm_file, "<p style='color:red;'>Error generating SVG graph</p>\n");

    // таблица элементов
    fprintf(htm_file, "<table border='1' style='border-collapse:collapse; width:100%%; margin-top:15px;'>\n");
    fprintf(htm_file, "<tr><th>Index</th><th>Data</th><th>Next</th><th>Prev</th><th>Status</th></tr>\n");

    for (int i = 0; i < list->capacity; i++) {
        ElementInList* element = &list->array[i];
        const char* status = "FREE";

        if (i == 0) status = "POISON";
        else if (i == list->array[0].next && i == list->array[0].prev)
            status = "HEAD/TAIL";
        else if (i == list->array[0].next)
            status = "HEAD";
        else if (i == list->array[0].prev)
            status = "TAIL";
        else if (!IsElementFree(list, i))
            status = "USED";

        fprintf(htm_file, "<tr><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%s</td></tr>\n",
                i, element->data, element->next, element->prev, status);
    }

    fprintf(htm_file, "</table>\n");
    fprintf(htm_file, "</div>\n\n");

    fclose(htm_file);

    remove(temp_dot);
    // remove(temp_svg);

    printf("Данные добавлены в лог-файл: %s\n", filename);
    return LIST_ERROR_NO;
}

ListErrorType InitListLog(const char* filename)
{
    FILE* htm_file = fopen(filename, "w");
    if (!htm_file)
        return LIST_ERROR_OPENING_FILE;

    fprintf(htm_file, "<!DOCTYPE htm>\n"
                      "<htm>\n"
                      "<head>\n"
                      "<title>List Dump Log</title>\n" //то, что отображается во вкладке браузера
                      "<style>\n"
                      "body { font-family: Arial, sans-serif; margin: 20px; }\n"
                      "table { border-collapse: collapse; width: 100%%; }\n" //объединяет границы ячеек
                      "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n" //td - ячейка, th - заголовок таблицы
                      "th { background-color: #f2f2f2; }\n"
                      "</style>\n"
                      "</head>\n"
                      "<body>\n"
                      "<h1>List Dump Log</h1>\n");
    fclose(htm_file);

    return LIST_ERROR_NO;
}

ListErrorType CloseListLog(const char* filename)
{
    FILE* htm_file = fopen(filename, "a");
    if (!htm_file)
        return LIST_ERROR_OPENING_FILE;

    fprintf(htm_file, "</body>\n");
    fprintf(htm_file, "</htm>\n");
    fclose(htm_file);

    return LIST_ERROR_NO;
}



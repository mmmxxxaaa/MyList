#include "my_list.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

int IsElementFree(List* list, ssize_t index)
{
    assert(list);

    if (index < 0 || index >= list->capacity)
        return 0;

    ssize_t free_idx = list->free;
    while (free_idx != 0)
    {
        if (free_idx == index)
            return 1;

        free_idx = list->array[free_idx].next;
    }
    return 0;
}

ListErrorType ListCtorWithSpecifiedCapacity(List* ptr_list_struct, ssize_t capacity)
{
    assert(ptr_list_struct);

    if (capacity <= 0)
        return LIST_WRONG_SPECIFIED_CAPACITY;

    ptr_list_struct->array = (ElementInList*) calloc(capacity, sizeof(ElementInList));
    if (ptr_list_struct->array == NULL)
        return LIST_ALLOCATION_FAILED;
    ptr_list_struct->capacity = capacity;

    // инициализация poisonа на нулевом индексе
    ptr_list_struct->array[kFictiveElementIndex].data = kPoison;
    ptr_list_struct->array[kFictiveElementIndex].next = kFictiveElementIndex;
    ptr_list_struct->array[kFictiveElementIndex].prev = kFictiveElementIndex;

    for (ssize_t i = 1; i < capacity; i++)
    {
        ptr_list_struct->array[i].data = kPoison;
        ptr_list_struct->array[i].next = (i == capacity - 1) ? kFictiveElementIndex : i + 1;
        ptr_list_struct->array[i].prev = -1;
    }

    ptr_list_struct->size           = 0;
    ptr_list_struct->free           = 1;

    return LIST_ERROR_NO;
}

ssize_t FindMaxUsedIndex(List* list)
{
    if (list == NULL || list->size == 0)
        return 0; // только фиктивный элемент

    ssize_t max_index = 0; // начинаем с фиктивного элемента
    ssize_t current = list->array[kFictiveElementIndex].next;

    // идем по цепочке next всех используемых элементов
    while (current != kFictiveElementIndex)
    {
        if (current > max_index)
            max_index = current;
        current = list->array[current].next;
    }

    return max_index;
}

ListErrorType ListReallocUp(List* list, ssize_t new_capacity)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    if (new_capacity <= list->capacity)
        return LIST_WRONG_SPECIFIED_CAPACITY;

    ElementInList* new_array = (ElementInList*)realloc(list->array, new_capacity * sizeof(ElementInList));
    if (new_array == NULL)
        return LIST_REALLOCATION_FAILED;

    list->array = new_array;

    ssize_t old_capacity = list->capacity; //инициализируем новые элементы
    for (ssize_t i = old_capacity; i < new_capacity; i++)
    {
        list->array[i].data = kPoison;
        list->array[i].next = (i == new_capacity - 1) ? kFictiveElementIndex : i + 1;
        list->array[i].prev = -1;
    }

    if (list->free == kFictiveElementIndex)
    {
        list->free = old_capacity; // если свободных не было, начинаем с первого нового элемента
    }
    else
    {
        // находим последний элемент в текущем списке свободных
        ssize_t last_free = list->free;
        while (list->array[last_free].next != kFictiveElementIndex) //FIXME новое поле, которое хранит индекс последнего free, и тут к нему обращаться и не будет цикла
            last_free = list->array[last_free].next;
        // связываем последний свободный с первым новым
        list->array[last_free].next = old_capacity;
    }

    list->capacity = new_capacity;
    return LIST_ERROR_NO;
}

ListErrorType ListReallocDown(List* list, ssize_t new_capacity)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    if (new_capacity >= list->capacity)
        return LIST_WRONG_SPECIFIED_CAPACITY;

    ssize_t max_used_index = FindMaxUsedIndex(list);
    if (new_capacity <= max_used_index)
        return LIST_WRONG_SPECIFIED_CAPACITY;

    ElementInList* new_array = (ElementInList*) calloc(new_capacity, sizeof(ElementInList));
    if (new_array == NULL)
        return LIST_ALLOCATION_FAILED;

    for (ssize_t i = 0; i < new_capacity; i++)
        new_array[i] = list->array[i];

    // если последний элемент был использован, его next должен указывать на фиктивный
    if (new_array[new_capacity - 1].prev != -1)
        new_array[new_capacity - 1].next = kFictiveElementIndex;

//FIXME
    ssize_t first_free = -1;
    ssize_t last_free  = -1;

    for (ssize_t i = 1; i < new_capacity; i++)
    {
        if (new_array[i].prev == -1) // элемент свободен
        {
            if (first_free == -1)
            {
                first_free = i;
                last_free = i;
            }
            else
            {
                new_array[last_free].next = i;
                last_free = i;
            }
        }
    }

    if (last_free != -1)
        new_array[last_free].next = kFictiveElementIndex;

    list->free = (first_free == -1) ? kFictiveElementIndex : first_free;

    free(list->array);
    list->array = new_array;
    list->capacity = new_capacity;

    return LIST_ERROR_NO;
}

ListErrorType ListReallocDownAsPossibleAsYouCan(List* list)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    ssize_t max_used_index = FindMaxUsedIndex(list);
    ssize_t new_capacity   = max_used_index + 1;

    ElementInList* new_array = (ElementInList*) calloc(new_capacity, sizeof(ElementInList));
    if (new_array == NULL)
        return LIST_ALLOCATION_FAILED;

    for (ssize_t i = 0; i < new_capacity; i++)
        new_array[i] = list->array[i];

    // если последний элемент был использован, его next должен указывать на фиктивный (тут всегда так будет)
    // if (new_array[new_capacity - 1].prev != -1)
        new_array[new_capacity - 1].next = kFictiveElementIndex;

//FIXME
    ssize_t first_free = -1;
    ssize_t last_free  = -1;

    for (ssize_t i = 1; i < new_capacity; i++)
    {
        if (new_array[i].prev == -1) // элемент свободен
        {
            if (first_free == -1)
            {
                first_free = i;
                last_free = i;
            }
            else
            {
                new_array[last_free].next = i;
                last_free = i;
            }
        }
    }

    if (last_free != -1)
        new_array[last_free].next = kFictiveElementIndex;

    list->free = (first_free == -1) ? kFictiveElementIndex : first_free;

    free(list->array);
    list->array = new_array;
    list->capacity = new_capacity;

    return LIST_ERROR_NO;
}

ListErrorType ListShrinkToFit(List* list)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    ListErrorType lin_result = ListLinearize(list);
    if (lin_result != LIST_ERROR_NO)
        return lin_result;

    ssize_t new_capacity = list->size + 2; //если нужно даже без одного свободного, то +2 меняем на +1 и усе

    if (new_capacity >= list->capacity)
        return LIST_ERROR_NO;

    return ListReallocDown(list, new_capacity);
}

ListErrorType ListDtor(List* ptr_list_struct)
{
    assert(ptr_list_struct);

    ptr_list_struct->capacity       = 0;
    ptr_list_struct->size           = 0;
    ptr_list_struct->free           = 0;

    FREE_AND_NULL(ptr_list_struct->array);

    return LIST_ERROR_NO;
}

ListErrorType ListInsertAfter(List* list, ssize_t target_index, DataType value)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    if (target_index < 0 || target_index >= list->capacity)
        return LIST_INVALID_INDEX;

    // if (IsElementFree(list, target_index) && target_index != 0) // FIXME цикл есть или нет?
    //     return LIST_INVALID_INDEX;

    if (list->free == 0)
    {
        ListErrorType realloc_result = ListReallocUp(list, list->capacity * kCapacityIncreaseCoefficient);
        if (realloc_result != LIST_ERROR_NO)
            return realloc_result;
    }

    ssize_t new_index = list->free;
    list->free = list->array[new_index].next;

    list->array[new_index].data = value;

    list->array[new_index].prev = target_index;
    list->array[new_index].next = list->array[target_index].next;

    list->array[target_index].next = new_index;
    list->array[list->array[new_index].next].prev = new_index;

    list->size += 1;

    return LIST_ERROR_NO;
}

ListErrorType ListInsertBeforeHead(List* list, DataType value)
{
    return ListInsertAfter(list, kFictiveElementIndex, value);
}

ListErrorType ListInsertAfterTail(List* list, DataType value)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    ssize_t tail_index = list->array[kFictiveElementIndex].prev;

    return ListInsertAfter(list, tail_index, value);
}

ListErrorType ListInsertTheFirstElement(List* list, DataType value)
{
    return ListInsertAfter(list, kFictiveElementIndex, value);
}

ListErrorType ListDeleteAt(List* list, ssize_t index)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    if (index < 1 || index >= list->capacity)
        return LIST_INVALID_INDEX;

    // if (IsElementFree(list, index))  // нужно ли проверять? нельзя удалить уже свободный элемент
    //     return LIST_INVALID_INDEX;

    ElementInList* element = &list->array[index];

    list->array[element->prev].next = element->next;
    list->array[element->next].prev = element->prev;

    element->data = kPoison;
    element->prev = -1;
    element->next = list->free;

    list->free  = index;
    list->size -= 1;

    return LIST_ERROR_NO;
}

ListErrorType ListLinearize(List* list)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    if (list->size == 1)
        return LIST_ERROR_NO;

    ElementInList* temp_array = (ElementInList*) calloc(list->capacity, sizeof(ElementInList));
    if (temp_array == NULL)
        return LIST_ALLOCATION_FAILED;

    temp_array[kFictiveElementIndex]      =  list->array[kFictiveElementIndex];
    temp_array[kFictiveElementIndex].next = (list->size > 0) ? 1 : 0;  // голова будет на индексе 1
    temp_array[kFictiveElementIndex].prev = (list->size > 0) ? list->size : 0; // хвост будет на последнем занятом индексе

    ssize_t current_index = list->array[kFictiveElementIndex].next;
    ssize_t new_index     = 1;

    while (current_index != 0 && new_index <= list->size) //FIXME
    {
        temp_array[new_index] = list->array[current_index];

        temp_array[new_index].prev = (new_index == 1)          ? kFictiveElementIndex : (new_index - 1);
        temp_array[new_index].next = (new_index == list->size) ? kFictiveElementIndex : (new_index + 1);

        current_index = list->array[current_index].next;
        new_index++;
    }

    for (ssize_t i = list->size + 1; i < list->capacity; i++)
    {
        temp_array[i].data = kPoison;
        temp_array[i].next = (i == list->capacity - 1) ? 0 : i + 1; //FIXME глянуть все нолики в проге, не должны ли они быть kFictiveElementIndex
        temp_array[i].prev = -1;
    }

    if (list->size + 1 < list->capacity)
        list->free = list->size + 1;
    else
        list->free = 0;

    free(list->array);
    list->array = temp_array;

    return LIST_ERROR_NO;
}

ssize_t GetIndexOfHead(List* list)
{
    assert(list);

    return list->array[kFictiveElementIndex].next;
}

ssize_t GetIndexOfTail(List* list)
{
    assert(list);

    return list->array[kFictiveElementIndex].prev;
}

ListErrorType ListDump(List* list, const char* filename)
{
    assert(list);
    assert(filename);

    char command[kMaxSystemCommandLength] = {};
    snprintf(command, sizeof(command), "mkdir -p %s", kGeneralFolderNameForLogs);
    system(command);

    char folder_name[kMaxLengthOfFilename] = {};
    snprintf(folder_name, sizeof(folder_name), "%s/%s_dump", kGeneralFolderNameForLogs, filename);

    // char command[kMaxSystemCommandLength] = {};
    snprintf(command, sizeof(command), "mkdir -p %s", folder_name);
    system(command);

    char htm_filename[kMaxLengthOfFilename] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s/%s.htm", kGeneralFolderNameForLogs, filename);

    FILE* htm_file = fopen(htm_filename, "a");
    if (!htm_file)
        return LIST_ERROR_OPENING_FILE;

    ListErrorType result = ListDumpToHtm(list, htm_file, folder_name);

    fclose(htm_file);

    return result;
}

ListErrorType ListDumpToHtm(List* list, FILE* htm_file, const char* folder_name)
{
    assert(list);
    assert(htm_file);
    assert(folder_name);

    time_t now = time(NULL);

    WriteDumpHeader(htm_file, now);

    WriteListInfo(htm_file, list);

    GenerateGraphVisualization(list, htm_file, folder_name, now);

    WriteElementsInTable(htm_file, list);

    fprintf(htm_file, "</div>\n\n");

    return LIST_ERROR_NO;
}

void WriteDumpHeader(FILE* htm_file, time_t now)
{
    assert(htm_file);

    fprintf(htm_file, "<div style='border:2px solid #ccc; margin:10px; padding:15px; background:#f9f9f9;'>\n"); //создает красивый контейнер для одного дампа
    fprintf(htm_file, "<h2 style='color:#333;'>List Dump at %s</h2>\n", ctime(&now)); //время дампа
}

void WriteListInfo(FILE* htm_file, List* list)
{
    assert(htm_file);
    assert(list);

    // Базовая инфа
    fprintf(htm_file, "<div style='margin-bottom:15px;'>\n"); //margin -- внешний отступ, padding -- внутренний
    fprintf(htm_file, "<p><b>Capacity:</b> %ld</p>\n", list->capacity);
    fprintf(htm_file, "<p><b>Size:</b> %ld</p>\n", list->size);
    fprintf(htm_file, "<p><b>Free head:</b> %ld</p>\n", list->free);
    fprintf(htm_file, "<p><b>Head index:</b> %ld</p>\n", list->array[kFictiveElementIndex].next);
    fprintf(htm_file, "<p><b>Tail index:</b> %ld</p>\n", list->array[kFictiveElementIndex].prev);

    VerifyResult verify_result          = VerifyList(list);
    const char* verify_result_in_string = VerifyResultToString(verify_result);
    const char* verify_color            = (verify_result == VERIFY_SUCCESS) ? "green" : "red";
    fprintf(htm_file, "<p><b>Verify result:</b> <span style='color:%s; font_weight: bold;'>%s</span></p>\n",
            verify_color, verify_result_in_string);

    fprintf(htm_file, "</div>\n");
}

void WriteElementsInTable(FILE* htm_file, List* list)
{
    assert(htm_file);
    assert(list);

    // таблица элементов //FIXME
    fprintf(htm_file, "<table border='1' style='border-collapse:collapse; width:100%%; margin-top:15px;'>\n"); //collapse -- убирает двойные линии в ячйеках
    fprintf(htm_file, "<tr><th>Index</th><th>Data</th><th>Next</th><th>Prev</th><th>Status</th></tr>\n");

    for (int i = 0; i < list->capacity; i++)
    {
        ElementInList* element = &list->array[i];
        const char* status = GetElementStatus(list, i);

        fprintf(htm_file, "<tr><td>%d</td><td>%d</td><td>%ld</td><td>%ld</td><td>%s</td></tr>\n",
                i, element->data, element->next, element->prev, status);
    }

    fprintf(htm_file, "</table>\n");
}

const char* GetElementStatus(List* list, int index)
{
    assert(list);

    if (index == 0)
        return "FICTIVE";
    else if (index == list->array[kFictiveElementIndex].next && index == list->array[kFictiveElementIndex].prev)
        return  "HEAD/TAIL";
    else if (index == list->array[kFictiveElementIndex].next)
        return "HEAD";
    else if (index == list->array[kFictiveElementIndex].prev)
        return "TAIL";
    else if (!IsElementFree(list, index))
        return "USED";
    else
        return "FREE";
}

//==========================================DOT=====================================================
ListErrorType GenerateGraphVisualization(List* list, FILE* htm_file, const char* folder_name, time_t now)
{
    assert(list);
    assert(htm_file);
    assert(folder_name);

    static int n_of_pictures = 0;
    char temp_dot[kMaxLengthOfFilename] = {};
    char temp_svg[kMaxLengthOfFilename] = {};

    snprintf(temp_dot, sizeof(temp_dot), "%s/temp_%d%ld.dot",folder_name, n_of_pictures, now);
    snprintf(temp_svg, sizeof(temp_svg), "%s/temp_%d%ld.svg",folder_name, n_of_pictures, now);
    n_of_pictures++;

    ListErrorType dot_result = GenerateDotFile(list, temp_dot);
    if (dot_result != LIST_ERROR_NO)
        return dot_result;

    char command[kMaxSystemCommandLength] = {};
    snprintf(command, sizeof(command), "dot -Tsvg %s -o %s", temp_dot, temp_svg);
    int result = system(command);

    if (result == 0)
    {
        fprintf(htm_file, "<div style='text-align:center;'>\n");
        fprintf(htm_file, "<img src='%s' style='max-width:100%%; border:1px solid #ddd;'>\n", temp_svg);
        fprintf(htm_file, "</div>\n");
    }
    else
    {
        fprintf(htm_file, "<p style='color:red;'>Error generating SVG graph</p>\n");
    }

    remove(temp_dot);
    // remove(temp_svg);

    return LIST_ERROR_NO;
}

ListErrorType GenerateDotFile(List* list, const char* filename)
{
    assert(list);
    assert(filename);

    FILE* dot_file = fopen(filename, "w");
    if (!dot_file)
        return LIST_ERROR_OPENING_FILE;

    fprintf(dot_file, "digraph DoublyLinkedList {\n");
    fprintf(dot_file, "    rankdir=LR;\n");
    fprintf(dot_file, "    node [shape=Mrecord, color = black];\n\n");

    CreateDotNodes(list, dot_file);

    CreateInvisibleElementConnections(list, dot_file);

    CreateCommonElementConnections(list, dot_file);

    CreateFreeElementConnections(list, dot_file);

    fprintf(dot_file, "    free_ptr [shape=plaintext, label=\"free\"];\n");

    if (list->free != 0)
        fprintf(dot_file, "    free_ptr -> element%ld [color=black];\n", list->free);

    fprintf(dot_file, "}\n");
    fclose(dot_file);

    return LIST_ERROR_NO;
}

void CreateDotNodes(List* list, FILE* dot_file)
{
    assert(list);
    assert(dot_file);

    for (int i = 0; i < list->capacity; i++)
    {
        ElementInList* element = &list->array[i];
        const char* color = "lightgreen";
        const char* label = "FREE";
        int is_free = IsElementFree(list, i);

        if (i == kFictiveElementIndex)
        {
            color = "lightcoral";
            label = "FICTIVE";
        }
        else if (i == list->array[kFictiveElementIndex].prev && i == list->array[kFictiveElementIndex].next)
        {
            color = "orange";
            label = "HEAD/TAIL";
        }
        else if (i == list->array[kFictiveElementIndex].next)
        {
            color = "lightblue";
            label = "HEAD";
        }
        else if (i == list->array[kFictiveElementIndex].prev)
        {
            color = "lightyellow";
            label = "TAIL";
        }
        else if (!is_free)
        {
            color = "white";
            label = "USED";
        }

        fprintf(dot_file, "    element%d [label=\"{%s|{idx: %d|data: %d|next: %ld|prev: %ld}}\", style=filled, fillcolor=%s, color=black];\n", //Внешние фигурные скобки создают основную таблицу символ | между элементами разделяет строки таблицы; Внутренние фигурные скобки создают вложенные таблицы/ячейки Символ | внутри внутренних скобок разделяет столбцы внутри строки
            i, label, i, element->data, element->next, element->prev, color);
        }

    fprintf(dot_file, "\n");
}

void CreateInvisibleElementConnections(List* list, FILE* dot_file)
{
    assert(list);
    assert(dot_file);

    for (int i = 0; i < list->capacity - 1; i++)
        fprintf(dot_file, "    element%d -> element%d [weight=100000, style=invis, color=white];\n", i, i+1);
}

void CreateCommonElementConnections(List* list, FILE* dot_file)
{
    assert(list);
    assert(dot_file);

    //проверяем связи и рисуем их
    for (ssize_t i = 0; i < list->capacity; i++)
    {
        ElementInList* element = &list->array[i];

        if (IsElementFree(list, i))
            continue;

        ssize_t next = element->next;
        ssize_t prev = element->prev;

        if (next >= 0 && next < list->capacity)
        {
            if (list->array[next].prev == i) //чекаем двустороннюю связь //FIXME
                fprintf(dot_file, "    element%ld -> element%ld [color=black, constraint=false, arrowhead=normal, arrowtail=normal, dir=both];\n", i, next);
            else
            {
                fprintf(dot_file, "    element%ld -> element%ld [color=blue, label=\"next\", constraint=false];\n", i, next);

                fprintf(dot_file, "    error_prev_%ld [shape=ellipse, style=filled, fillcolor=orange, label=\"Prev Error: element%ld->next=%ld\\nbut element%ld->prev=%ld\"];\n",
                        i, i, next, next, list->array[next].prev);
                fprintf(dot_file, "    error_prev_%ld -> element%ld [color=red, style=dashed];\n", i, next);
            }
        }

        if (prev >= 0 && prev < list->capacity && prev != -1)
        {
            // для prev связи проверяем только неправильные случаи, так как правильные уже нарисованы и будет дублирнование
            if (list->array[prev].next != i)
            {
                fprintf(dot_file, "    element%ld -> element%ld [color=red, label=\"prev\", constraint=false];\n", i, prev);

                fprintf(dot_file, "    error_next_%ld [shape=ellipse, style=filled, fillcolor=yellow, label=\"Next Error: element%ld->prev=%ld\\nbut element%ld->next=%ld\"];\n",
                        i, i, prev, prev, list->array[prev].next);
                fprintf(dot_file, "    error_next_%ld -> element%ld [color=blue, style=dashed];\n", i, prev);
            }
        }
    }
}

void CreateFreeElementConnections(List* list, FILE* dot_file)
{
    assert(list);
    assert(dot_file);

    fprintf(dot_file, "\n");
    ssize_t free_idx = list->free;
    while (free_idx != 0 && free_idx < list->capacity)
    {
        ElementInList* element = &list->array[free_idx];
        if (element->next != 0)
            fprintf(dot_file, "    element%ld -> element%ld [color=green, label=\"free\", constraint=false];\n", free_idx, element->next);
        free_idx = element->next;
    }
}

ListErrorType InitListLog(const char* filename)
{
    assert(filename);

    char htm_filename[kMaxLengthOfFilename] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s/%s.htm", kGeneralFolderNameForLogs, filename); //FIXME вынести logs в отдельную строковую константу

    FILE* htm_file = fopen(htm_filename, "w");
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
    assert(filename);

    char htm_filename[kMaxLengthOfFilename] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s/%s.htm", kGeneralFolderNameForLogs, filename);

    FILE* htm_file = fopen(htm_filename, "a");
    if (!htm_file)
        return LIST_ERROR_OPENING_FILE;

    fprintf(htm_file, "</body>\n");
    fprintf(htm_file, "</htm>\n");
    fclose(htm_file);

    return LIST_ERROR_NO;
}

const char* VerifyResultToString(VerifyResult result)
{
    switch (result)
    {
        case VERIFY_SUCCESS:                  return "Success";
        case VERIFY_FAKE_ELEMENT_NEXT_ERROR:  return "Fake element next pointer error";
        case VERIFY_FAKE_ELEMENT_PREV_ERROR:  return "Fake element prev pointer error";
        case VERIFY_TAIL_NEXT_ERROR:          return "Tail next pointer error";
        case VERIFY_HEAD_PREV_ERROR:          return "Head prev pointer error";
        case VERIFY_CAPACITY_EXCEEDED:        return "Capacity exceeded during traversal";
        case VERIFY_INVALID_INDEX:            return "Invalid node index";
        case VERIFY_BIDIRECTIONAL_LINK_ERROR: return "Bidirectional link broken";
        case VERIFY_COUNT_MISMATCH:           return "Element count mismatch";
        case VERIFY_EMPTY_LIST_LINKS_ERROR:   return "Empty list links error";
        case VERIFY_CYCLE_DETECTED:           return "Detected cycle in list";
        default:                              return "Unknown error";
    }
}

VerifyResult DetectCycle(List* list)
{
    if (list == NULL || list->size == 0)
        return VERIFY_SUCCESS;

    ssize_t current_by_next = kFictiveElementIndex; // идем с фиктивного элемента
    ssize_t current_by_prev = kFictiveElementIndex;
    ssize_t steps = 0;

    // идем size + 1 шагов
    while (steps <= list->size)
    {
        current_by_next = list->array[current_by_next].next;
        current_by_prev = list->array[current_by_prev].prev;

        if (current_by_next < 0 || current_by_next >= list->capacity ||
            current_by_prev < 0 || current_by_prev >= list->capacity)
            return VERIFY_INVALID_INDEX;

        steps++;

        // слишком рано встретили фиктивный элемент, значит, ошибка
        if ((current_by_next == kFictiveElementIndex || current_by_prev == kFictiveElementIndex) && steps <= list->size)
            return VERIFY_CYCLE_DETECTED;

        // если встретили фиктивный элемент ровно на шаге size + 1, то все ок
        if (current_by_next == kFictiveElementIndex && current_by_prev == kFictiveElementIndex && steps == list->size + 1)
            break;
    }

    // НАДО ПРОВЕРИТЬ, ЧТО МЫ ВЫШЛИ ИМЕННО ПО БРЕЙКУ, А НЕ ПРОСТО!!!!!
    if (current_by_next != kFictiveElementIndex || current_by_prev != kFictiveElementIndex)
        return VERIFY_CYCLE_DETECTED;

    return VERIFY_SUCCESS;
}

VerifyResult VerifyList(List* list)
{
    if (list == NULL)
        return VERIFY_INVALID_INDEX;

    VerifyResult cycle_check = DetectCycle(list);
    if (cycle_check != VERIFY_SUCCESS)
        return cycle_check;

    if (list->size > 0)
    {
        ssize_t head_index = list->array[kFictiveElementIndex].next;
        ssize_t tail_index = list->array[kFictiveElementIndex].prev;

        if (list->array[tail_index].next != kFictiveElementIndex)
            return VERIFY_TAIL_NEXT_ERROR;
        if (list->array[head_index].prev != kFictiveElementIndex)
            return VERIFY_HEAD_PREV_ERROR;

        ssize_t current = head_index; //начинаем обходить с головы
        ssize_t count = 0;

        // чекаем узлы
        while (count < list->size)
        {
            ssize_t next = list->array[current].next;
            if (next < 0 || next >= list->capacity)
                return VERIFY_INVALID_INDEX;

            // чекаем двустороннюю связь //FIXME создать функцию которая бежит до конца массива по next, пока не встретит ошибку. Если встретит ее, то обрабатывает и бежит дальше. Возвращает индекс элемента, где что-то не так. В верификаторе если возвращаемый индекс != 0, то в верификаторе возвращаю ошибку, а в дампе вывожу
            if (list->array[next].prev != current)
                return VERIFY_BIDIRECTIONAL_LINK_ERROR;

            current = next;
            count++;
        }

        if (current != kFictiveElementIndex)
            return VERIFY_CYCLE_DETECTED;
    }
    else
    {
        if (list->array[kFictiveElementIndex].next != kFictiveElementIndex ||
            list->array[kFictiveElementIndex].prev != kFictiveElementIndex)
            return VERIFY_EMPTY_LIST_LINKS_ERROR;
    }

    return VERIFY_SUCCESS;
}


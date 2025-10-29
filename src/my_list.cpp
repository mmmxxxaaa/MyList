#include "my_list.h"

#include <assert.h>
#include <stdlib.h>

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
    ptr_list_struct->array[0].next = 0; //FIXME
    ptr_list_struct->array[0].prev = 0;

    // инициализация остальных элементов как свободных
    for (int i = 1; i < capacity; i++)
    {
        ptr_list_struct->array[i].data = kPoison;
        ptr_list_struct->array[i].next = (i == capacity - 1) ? -1 : i + 1;
        ptr_list_struct->array[i].prev = -1;
    }

    // ptr_list_struct->head = -1;  // когда список пуст, HEAD = -1
    // ptr_list_struct->tail = -1;  // когда список пуст, TAIL = -1


    ptr_list_struct->free = 1;   // свободные элементы начинаются с индекса 1

    return LIST_ERROR_NO;
}

ListErrorType ListDtor(List* ptr_list_struct)
{
    assert(ptr_list_struct);

    FREE_AND_NULL(ptr_list_struct->array);

    return LIST_ERROR_NO;
}

ListErrorType ListInsertAfter(List* list, int target_index, int value) //FIXME должен возвращать индекс (физический/логический?)
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

    // случай когда вставляем после нулевого poisonа (начала списка)
//     if (target_index == 0)
//     {
//         if (list->head == -1)         // вставка в пустой список
//         {
//             list->array[new_index].prev = -1;
//             list->array[new_index].next = -1;
//             list->array[0].prev         = new_index; //head
//             list->array[0].next         = new_index; //tail
//         }
//         else        // вставка в начало непустого списка
//         {
//             list->array[new_index].prev = -1;
//             list->array[new_index].next = list->head;
//             list->array[list->array[0].prev].prev = new_index;
//             list->array[0].prev = new_index;
//         }
//     }
//     else
//     {
//         // обычная вставка после существующего элемента
//         list->array[new_index].prev = target_index;
//         list->array[new_index].next = list->array[target_index].next;
//
//         list->array[target_index].next = new_index;
//
//         // если был следующий элемент после target, обновляем его prev
//         if (list->array[new_index].next != -1)
//             list->array[list->array[new_index].next].prev = new_index;
//
//         // если вставляем после хвоста, обновляем tail
//         if (list->tail == target_index)
//             list->tail = new_index;
//     }

    return LIST_ERROR_NO;
}

ListErrorType ListDelete(List* list, int index)
{
    if (list == NULL)
        return LIST_NULL_POINTER;

    if (index < 1 || index >= list->capacity)  // нельзя удалить нулевой poison или несуществующий
        return LIST_INVALID_INDEX;

    if (IsElementFree(list, index))  // FIXME цикл есть или нет? нельзя удалить уже свободный элемент
        return LIST_INVALID_INDEX;

    ElementInList* element = &list->array[index];

    list->array[element->prev].next = element->next;
    list->array[element->next].prev = element->prev;

//
//     // сохр связи для перестроения
//     int prev_index = element->prev;
//     int next_index = element->next;
//
//     // перестраиваем связи соседних элементов
//     if (prev_index != -1)
//         list->array[prev_index].next = next_index;
//     else
//         list->head = next_index;      // если у элемента нет предыд, значит он голова
//
//     if (next_index != -1)
//         list->array[next_index].prev = prev_index;
//     else
//         list->tail = prev_index;      // если у элемента нет след, значит он хвост

    // возвращаем элемент в список свободных
    element->data = kPoison;
    element->prev = -1;
    element->next = list->free;
    list->free = index;

    return LIST_ERROR_NO;
}

ListErrorType ListDump(List* list, const char* filename)
{
    assert(filename);

    FILE* file = fopen(filename, "w");
    if (!file)
        return LIST_ERROR_OPENING_FILE;

    fprintf(file, "digraph DoublyLinkedList {\n");
    fprintf(file, "    rankdir=LR;\n");
    fprintf(file, "    node [shape=Mrecord, color = black];\n\n");

    for (int i = 0; i < list->capacity; i++)
    {
        ElementInList* element = &list->array[i];

        const char* color = "lightgreen";
        const char* label = "FREE";

        int is_free = IsElementFree(list, i); //FIXME

        if (i == list->array[0].prev && i == list->array[0].next) //когда эл-т одновременно и хвост, и голова
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

        fprintf(file, "    element%d [label=\"{%s|idx: %d|data: %d|next: %d|prev: %d}\", style=filled, fillcolor=%s, color=black];\n",
                i, label, i, element->data, element->next, element->prev, color);
    }

    fprintf(file, "\n");

    // создаем невидимые связи для выстраивания в один ряд
    for (int i = 0; i < list->capacity - 1; i++)
        fprintf(file, "    element%d -> element%d [weight=100000, style=invis, color=white];\n", i, i+1);

    // создаем связи для используемых элементов
    for (int i = 0; i < list->capacity; i++)
    {
        ElementInList* element = &list->array[i];

        if (IsElementFree(list, i) || i == 0)  // пропускаем свободные и нулевой пойзон
            continue;

        // связь next (синяя стрелка)
        if (element->next != -1)
            fprintf(file, "    element%d -> element%d [color=blue, label=\"next\", constraint=false];\n", i, element->next); //FIXME без constraint=false ебануто кривой порядок у узлов, так как дот стремится построить с минимальным количеством пересечений стрелочек

        // связь prev (красная стрелка)
        if (element->prev != -1)
            fprintf(file, "    element%d -> element%d [color=red, label=\"prev\", style=dashed, constraint=false];\n", i, element->prev); //FIXME разобраться и установить соответствие, чему равен array[0].prev и array[0].next (TAIL и HEAD?)
    }

    fprintf(file, "\n");

    // связи для свободных элементов (если они связаны)
    int free_idx = list->free;
    while (free_idx != -1 && free_idx < list->capacity)
    {
        ElementInList* element = &list->array[free_idx];
        if (element->next != -1)
            fprintf(file, "    element%d -> element%d [color=gray, label=\"free\", constraint=false];\n", free_idx, element->next);

        free_idx = element->next;
    }

    // спец указатели //FIXME нужно ли в моем случае
    fprintf(file, "\n");
    fprintf(file, "    head_ptr [shape=plaintext, label=\"head\"];\n");
    fprintf(file, "    tail_ptr [shape=plaintext, label=\"tail\"];\n");
    fprintf(file, "    free_ptr [shape=plaintext, label=\"free\"];\n");

    if (list->array[0].prev != -1)
        fprintf(file, "    head_ptr -> element%d [color=darkblue];\n", list->array[0].next);

    if (list->array[0].next != -1)
        fprintf(file, "    tail_ptr -> element%d [color=darkgreen];\n", list->array[0].prev);

    if (list->free != -1)
        fprintf(file, "    free_ptr -> element%d [color=black];\n", list->free);

    fprintf(file, "}\n");
    fclose(file);

    printf("Данные сохранены в %s\n", filename);
    printf("Для визуализации выполните: dot -Tsvg %s -o graph.svg\n", filename);

    return LIST_ERROR_NO;
}

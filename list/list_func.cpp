#ifndef LIST_FUNC
#define LIST_FUNC
#include "Array.h"

size_t* get_head(list_t* list)
{
    return &(list->array[0].next);
}

ssize_t* get_tail(list_t* list)
{
    return &(list->array[0].prev);
}

size_t* get_next(list_t* list, size_t index)
{
    return &(list->array[index].next);
}

ssize_t* get_prev(list_t* list, size_t index)
{    
    return &(list->array[index].prev);
}   

arr_value* get_elem(list_t* list, size_t index)
{
    return &(list->array[index].elem);
}


#endif
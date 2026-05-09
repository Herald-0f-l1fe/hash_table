// #define DEBUG

#include "Array.h"


ARR_errors list_creator(list_t* list)
{
    ON_DEBUG(printf("START CREATING LIST\n");)

    if (list == nullptr) 
        return no_valid_pointer_to_list;
    
    ON_DEBUG(printf("GOOD LIST POINTER\n");)

    list->capacity = LIST_SIZE;
    list->size = 0;

    list->array = (block_t*) calloc (list->capacity, sizeof(block_t));
    list->array->elem = CANARY;
    
    if (!list->array)
        return no_valid_pointer_to_list;
    
    list->free = 1;

    for (size_t i = 1; i < LIST_SIZE; i++)
    {
        list->array[i].elem = POISON;
        list->array[i].next = list->free + i;
        list->array[i].prev = -1;
    }
    list->array[LIST_SIZE - 1].next = 0; 

    return OK;
}

ARR_errors verificator(list_t* list)
{
    if (list == nullptr)
        {
        printf("NULLPOINTER TO LIST\n");
        return no_valid_pointer_to_list;
        }
    
    if (*get_elem(list, 0) != CANARY)
        {
            printf("CANARA IS DIED\n");
            list->errors |= canary_is_died;
        }
    
    if (list->size > list->capacity)
        {
            printf("SIZE IS MORE CAPACITY\n");
        list->errors |= size_more_capacity;
        }
    
    if (list->free == 0)
        {
            printf("NO FREE BLOCKS\n");
            list->errors |= no_free_block;
        }

    if (list->errors != 0)
        return ERROR;
    
    return OK;
}

ARR_errors insert_after(list_t* list, size_t index, arr_value new_elem)
{
    if (verificator(list) != OK)
        return ERROR;

    // else if (index >= list->capacity)
    //     return no_valid_index;    
    
    // else if (*get_prev(list, index) == -1)
    //     return insert_after_free;
    

    size_t new_free = *get_next(list, list->free);    


    if (new_free == 0)
    {
        list_extension(list);
        new_free = *get_next(list, list->free);
    }

    *get_prev(list, list->free) = (ssize_t)index;
    *get_next(list, list->free) = *get_next(list, index);
    *get_elem(list, list->free) = new_elem;


    *get_next(list, index) = list->free;

    list->free = new_free;
    *get_tail(list) = (ssize_t)*get_next(list, (size_t)*get_tail(list));

    list->size ++;
    return OK;

    
}

ARR_errors delete_after(list_t* list, size_t index)
{
    if (verificator(list) != OK)
        return ERROR;

    else if (index == (size_t)*get_tail(list))
    {
        list->errors |= delete_after_tail;
        return delete_after_tail;
    }

    else if(*get_prev(list, index) == -1)
    {
        list->errors |= delete_after_free;
        return delete_after_free;
    }
    
    size_t delete_index = *get_next(list, index);

    *get_prev(list, delete_index) = -1;
    *get_prev(list, *get_next(list, delete_index)) = (ssize_t)index;  
    *get_next(list, index) = *get_next(list, delete_index);
    


    *get_next(list, delete_index) = list->free;
    list->free = delete_index;
    list->size--;

    return OK;
}


ARR_errors list_destructor(list_t* list)
{
    free(list->array);
    return OK;
}

ARR_errors list_extension(list_t* list)
{
    if (list == nullptr)
        return no_valid_pointer_to_list;
    
    list->capacity *= 2;

    list->array = (block_t*) realloc(list->array, sizeof(block_t)*(list->capacity));


    for (size_t i = list->size + 1; i < list->capacity; i++)
    {
        list->array[i].next = i + 1;
        list->array[i].elem = POISON;
        list->array[i].prev = -1;
    }
    *get_next(list, list->capacity - 1) = 0;
    
    list->free = list->size + 1;

    return OK;
}

// ARR_errors delete_before(list_t* list, size_t index)
// {
//     if (verificator(list) != OK)
//         return ERROR;

//     if (*get_head(list) == index)
//     {
//         list->errors |= delete_before_head;
//         return delete_before_head;
//     }   

//     else if (*get_prev(list, index) == -1)
//     {
//         list->errors |= delete_before_free;
//         return delete_before_free;
//     } 

    
// }

void List_DUMP(list_t* list, size_t file_number)
{
    printf(BOLD_MAGENTA);
    
    printf("pointer to List is %p", list);



    printf(RESET);

    create_html(file_number);
}

void create_html(size_t file_number)
{
    FILE* fp = fopen("dump.html", "w");

    fprintf(fp,"<!DOCTYPE html>\n"
                       "<html>\n"
                       "<head>\n"
                       "    <title>List Dump</title>\n"
                       "</head>\n"
                       "<pre>\n"
                       "<body>\n");
    
                       
    for (size_t i = 0; i < file_number; i++)
    {
        char path[70] = {};
        sprintf(path, "/mnt/c/Users/boris/projects/Array/photo/graph%lu.png", i);    
        fprintf(fp, "<img src=\" %s \">\n\n\n", path);
    }

    fprintf(fp, "</body>\n"
                "</pre>"
                "</html>\n");
    fclose(fp);
}
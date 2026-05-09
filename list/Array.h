#ifndef ARRAY
#define ARRAY
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#include "../../common/DEBUG.h"
#include "../../common/COLORS.h"
typedef char *  arr_value;

#define CANARY (char*)1505007 
#define POISON (char*)0xDEADBEEF 
#define LIST_SIZE 10

enum ARR_errors 
{  
    OK                       =    0,
    ERROR                    =    1,
    no_valid_pointer_to_list =    2,
    error_in_pointer         =    4,
    no_free_block            =    8,
    delete_after_tail        =   16,    
    error_in_creating_graph  =   32,  
    no_valid_index           =   64,
    insert_after_free        =  128,
    delete_after_free        =  256,
    canary_is_died           =  512,
    size_more_capacity       = 1024,
    delete_before_head       = 2048,
    delete_before_free       = 4096,
};



#define PRP(point)                              \
    if (point == nullptr)                       \
        return error_in_pointer;

    
#define func_check(func)                        \
                                                \
    if (func != OK)                             \
        {                                       \
            List_DUMP(&list, graph_namber);     \
            return ERROR;                       \
        }                                       \
    create_graph(&graph_namber, list);  

struct block_t
{
    size_t next;
    arr_value elem;
    ssize_t prev;
};

struct list_t
{
    block_t* array;
    size_t free;
    size_t size;
    size_t capacity;
    size_t errors;
};





ARR_errors list_creator(list_t* arr);
ARR_errors verificator(list_t* array);
ARR_errors insert_after(list_t* arr, size_t index, arr_value new_elem);
ARR_errors delete_after(list_t* arr, size_t index);
//ARR_errors add_to_begin(list_t* arr, arr_value new_elem);
ARR_errors list_destructor(list_t* arr);
ARR_errors list_extension(list_t* arr);
void create_html(size_t file_number);
void List_DUMP(list_t* list, size_t file_number);

FILE* open_file();
ARR_errors create_file(FILE* fp, list_t arr);
char* block_creater(block_t node, size_t node_number, char* block);
char* edge_create(block_t node, size_t index, char* edge);
ARR_errors create_graph(size_t* file_number, list_t arr);
char* system_graph(size_t file_number, char* system_cmd);
ARR_errors set_graph(FILE* fp, list_t arr);

ssize_t* get_prev(list_t* arr, size_t index);
ssize_t* get_tail(list_t* arr);
size_t* get_next(list_t* arr, size_t index);
size_t* get_head(list_t* arr);
arr_value* get_elem(list_t* arr, size_t index);

#endif
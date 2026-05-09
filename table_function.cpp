#include "hash_table.h"


ARR_errors hash_table_init(hash_table_t* table, size_t capacity, hash_func_t hash)
{
    if (!table) return ERROR;

    table->bucket_count = capacity;
    table->buckets = (list_t*)calloc(capacity, sizeof(list_t));
    if (!table->buckets) return ERROR;

    for (size_t i = 0; i < capacity; ++i) 
    {
        ARR_errors err = list_creator(&table->buckets[i]);
        if (err != OK) 
        {
            // откат уже созданных списков
            for (size_t j = 0; j < i; ++j)
                list_destructor(&table->buckets[j]);
            free(table->buckets);
            table->buckets = NULL;
            printf(RED"error in creating lists\n");
            return ERROR;
        }
    }

    table->buffer      = NULL;
    table->buffer_size = 0;
    table->owns_buffer = false;
    table->hash = hash;
    return OK;
}

ARR_errors hash_table_insert(hash_table_t* table, const char* word)
{
    if (!table || !word) return ERROR;

    // Игнорируем строки, состоящие только из дефисов/апострофов
    if (!has_alnum(word, strlen(word)))
        return ERROR;

    size_t idx = table->hash(word, table->bucket_count);
    list_t* bucket = &table->buckets[idx];

    // Поиск дубликата
    size_t cur = *get_head(bucket);
    while (cur != 0) {
        arr_value* elem = get_elem(bucket, cur);
        if (elem && *elem && strcmp(*elem, word) == 0)
            return OK;   // уже есть
        cur = *get_next(bucket, cur);
    }

    // Создаём копию
    // char* copy = strdup(word);
    // if (!copy) return ERROR;

    // Вставляем после фиктивного элемента (индекс 0) – в начало списка
    // return insert_after(bucket, 0, copy);
    return insert_after(bucket, 0, (char*) word);
}

arr_value hash_table_find(const hash_table_t* table, const char* word)
{
    if (!table || !word) return NULL;

    size_t idx = table->hash(word, table->bucket_count);
    list_t* bucket = &table->buckets[idx];

    size_t cur = *get_head(bucket);   // get_head требует не const
    while (cur != 0) {
        arr_value* elem = get_elem(bucket, cur);
        if (elem && *elem && strcmp(*elem, word) == 0)
            return *elem;
        cur = *get_next(bucket, cur);
    }
    return NULL;
}


void hash_table_destroy(hash_table_t* table)
{
    if (!table) return;

    // Освобождаем все списки корзин
    if (table->buckets) 
    {
        for (size_t i = 0; i < table->bucket_count; ++i)
            list_destructor(&table->buckets[i]);
        free(table->buckets);
    }

    // Если владеем буфером – освобождаем его
    if (table->owns_buffer && table->buffer) {
        free(table->buffer);
    }

    // Зануляем структуру
    table->buckets      = NULL;
    table->buffer       = NULL;
    table->bucket_count = 0;
    table->buffer_size  = 0;
    table->owns_buffer  = false;
}


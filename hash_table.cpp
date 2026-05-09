#include "hash_table.h"

#include <ctype.h>      // isalpha, isdigit, isalnum
#include <stdio.h>      // FILE, fopen, fclose, fprintf
#include <stdlib.h>     // calloc, free, size_t
#include <string.h>     // memmove, strlen, strcmp, strdup


extern size_t hash_djb2(const char* word, size_t mod);
extern size_t hash_const_0(const char* word, size_t mod);
extern size_t hash_const_1(const char* word, size_t mod);
extern size_t hash_first_char(const char* word, size_t mod);
extern size_t hash_sum_ascii(const char* word, size_t mod);
extern size_t hash_sum_rotate_left(const char* word, size_t mod);
extern size_t hash_sum_rotate_right(const char* word, size_t mod);


static char* compact_buffer(char* raw, size_t size, size_t* new_size);
static const char* input_file = "The War of the Worlds.txt";
static const size_t bucket_count = 1009;   // количество корзин

// Массив хэш‑функций и их названий для отчётов

struct hash_func 
{
    const char* name;
    hash_func_t hash;

};


static hash_func hash_functions[] = 
{
    {"djb2"             ,  hash_djb2             },
    {"const_0"          ,  hash_const_0          },
    {"const_1"          ,  hash_const_1          },
    {"first_char"       ,  hash_first_char       },
    {"sum_ascii"        ,  hash_sum_ascii        },
    {"sum_rotate_left"  ,  hash_sum_rotate_left  },
    {"sum_rotate_right" ,  hash_sum_rotate_right }
};

static const int num = sizeof(hash_functions) / sizeof(hash_functions[0]);


int main()
{
    for (int i = 0; i < num; i++)
    {
        printf("Хэш‑функция: %s\n", hash_functions[i].name);

        hash_table_t table;
        ARR_errors err = hash_table_init(&table, bucket_count, hash_functions[i].hash);
        if (err != OK)
        {
            printf("Ошибка инициализации таблицы.\n");
            continue;
        }
        printf("Таблица создана\n");

        // Загружаем текст
        err = hash_table_load_from_file_buffer(&table, input_file);
        if (err != OK)
        {
            printf("Ошибка загрузки файла '%s'.\n", input_file);
            hash_table_destroy(&table);
            continue;
        }
        printf("Хэш-таблица заполнена\n");
        // PNG‑график
        char hist_png[256];
        snprintf(hist_png, sizeof(hist_png), "hist_%s.png", hash_functions[i].name);
        hash_table_histogram_to_png(&table, hist_png);

        hash_table_destroy(&table);
        printf("\n");
    }

    printf("Все гистограммы построены.\n");
    return 0;
}


static int is_word_char(int c)
{
    return isalpha(c) || isdigit(c) || c == '-' || c == '\'';
}

int has_alnum(const char* str, size_t len)
{
    for (size_t i = 0; i < len; ++i)
        if (isalnum((unsigned char)str[i]))
            return 1;
    return 0;
}



static ARR_errors insert_buffer_word(hash_table_t* table, const char* word_ptr)
{
    size_t idx = table->hash(word_ptr, table->bucket_count);
    list_t* bucket = &table->buckets[idx];
    // Проверка на дубликат
    size_t cur = *get_head(bucket);
    while (cur != 0) 
    {
        arr_value* elem = get_elem(bucket, cur);

        if (elem && *elem && strcmp(*elem, word_ptr) == 0)
            return OK;
        cur = *get_next(bucket, cur);
        // printf("cur = %zu\n", cur);
    }
    return insert_after(bucket, 0, (char*)word_ptr);
}


ARR_errors hash_table_load_from_file_buffer(hash_table_t* table, const char* filename)
{
    if (!table || !filename) return ERROR;

    long int file_size = 0;
    char* raw_buffer = read_from_file_to_buffer(&file_size, filename);
    if (!raw_buffer || file_size <= 0) return ERROR;

    // Сжимаем буфер: оставляем только слова и по одному '\0' между ними
    size_t new_size = 0;

    char* compacted = compact_buffer(raw_buffer, (size_t)file_size, &new_size);
    // Запоминаем буфер в таблице, теперь она отвечает за его освобождение
    table->buffer      = compacted;
    table->buffer_size = new_size;
    table->owns_buffer = true;
    free(raw_buffer);
    // Проходим по сжатому буферу и вставляем каждое слово как указатель
    char* cur = compacted;
    char* end = compacted + new_size;
    ON_DEBUG(printf("before while\n");)
    while (cur < end) 
    {
        if (*cur == '\0')
        {
            ++cur;
            // printf(BLUE"?"RESET);
            continue;
        }
        // printf("before insert\n");
        ARR_errors err = insert_buffer_word(table, cur);
        if (err != OK) return err;
        cur = strchr(cur, '\0');   // переходим к следующему слову
        // ON_DEBUG(printf(RED"!"RESET);)
        // ON_DEBUG(printf("cur = %p\n", cur);)
        // ON_DEBUG(printf("end = %p\n", end);)
    }
    ON_DEBUG(printf("before return\n");)
    return OK;
}



void hash_table_compute_histogram(const hash_table_t* table, size_t* sizes)
{
    if (!table || !sizes) return;

    for (size_t i = 0; i < table->bucket_count; ++i) 
    {
        size_t count = 0;
        size_t cur = *get_head((list_t*)&table->buckets[i]);
        while (cur != 0) 
        {
            ++count;
            cur = *get_next((list_t*)&table->buckets[i], cur);
        }
        sizes[i] = count;
    }
}


void hash_table_plot_histogram(const hash_table_t* table)
{
    if (!table || table->bucket_count == 0) return;

    size_t* sizes = (size_t*)calloc(table->bucket_count, sizeof(size_t));
    if (!sizes) {
        fprintf(stderr, "Ошибка памяти при построении графика.\n");
        return;
    }
    hash_table_compute_histogram(table, sizes);

    // 2. Открываю канал к gnuplot (с persist, чтобы окно не закрылось)
    FILE* gp = popen("gnuplot -persist", "w");
    if (!gp) 
    {
        fprintf(stderr, "Не удалось запустить gnuplot.\n");
        free(sizes);
        return;
    }

    size_t max_val = 0;
    for (size_t i = 0; i < table->bucket_count; ++i)
        if (sizes[i] > max_val) max_val = sizes[i];

    // 4. Посылаем команды gnuplot
    fprintf(gp, "set title 'Hash Table Bucket Load'\n");
    fprintf(gp, "set xlabel 'Bucket index'\n");
    fprintf(gp, "set ylabel 'Elements'\n");
    fprintf(gp, "set xrange [0:%zu]\n", table->bucket_count - 1);
    fprintf(gp, "set yrange [0:%zu]\n", max_val + 1);
    fprintf(gp, "set boxwidth 0.8\n");
    fprintf(gp, "set style fill solid 0.3\n");
    fprintf(gp, "plot '-' using 1:2 with boxes notitle\n");

    // 5. Передаём данные: пары (bucket_index, count)
    for (size_t i = 0; i < table->bucket_count; ++i)
        fprintf(gp, "%zu %zu\n", i, sizes[i]);
    fprintf(gp, "e\n");   // конец данных

    fflush(gp);
    pclose(gp);   // после закрытия gnuplot останется с persist
    free(sizes);
}

void hash_table_histogram_to_png(const hash_table_t* table, const char* filename)
{
    if (!table || !filename || table->bucket_count == 0) return;

    // Собираем данные
    size_t* sizes = (size_t*)calloc(table->bucket_count, sizeof(size_t));
    if (!sizes) {
        fprintf(stderr, "Ошибка памяти при построении графика.\n");
        return;
    }
    hash_table_compute_histogram(table, sizes);

    // Открываем канал к gnuplot
    FILE* gp = popen("gnuplot", "w");
    if (!gp) {
        fprintf(stderr, "Не удалось запустить gnuplot.\n");
        free(sizes);
        return;
    }

    size_t max_val = 0;
    for (size_t i = 0; i < table->bucket_count; ++i)
        if (sizes[i] > max_val) max_val = sizes[i];

    // Настройка вывода в PNG
    fprintf(gp, "set terminal png size 800,600\n");
    fprintf(gp, "set output '%s'\n", filename);
    fprintf(gp, "set title 'Hash Table Bucket Load'\n");
    fprintf(gp, "set xlabel 'Bucket index'\n");
    fprintf(gp, "set ylabel 'Elements'\n");
    fprintf(gp, "set xrange [0:%zu]\n", table->bucket_count - 1);
    fprintf(gp, "set yrange [0:%zu]\n", max_val + 1);
    fprintf(gp, "set boxwidth 0.8\n");
    fprintf(gp, "set style fill solid 0.3\n");
    fprintf(gp, "plot '-' using 1:2 with boxes notitle\n");

    // Передаём данные
    for (size_t i = 0; i < table->bucket_count; ++i)
        fprintf(gp, "%zu %zu\n", i, sizes[i]);
    fprintf(gp, "e\n"); // Конец передачи

    fflush(gp);
    pclose(gp);
    free(sizes);

    printf("График сохранён: %s\n", filename);
}

static char* compact_buffer(char* raw, size_t size, size_t* new_size)
{
        char* clean = (char*)calloc(size, sizeof(char));
    if (!clean) return NULL;

    char* dst = clean;
    const char* src = raw;
    const char* end = raw + size;
    
    while (src < end)
    {
        // пропускаем разделители
        while (src < end && !is_word_char((unsigned char)*src))
            ++src;
        if (src >= end) break;

        // начало слова
        const char* word_start = src;
        while (src < end && is_word_char((unsigned char)*src))
            ++src;
        size_t word_len = src - word_start;

        // игнорируем слова без букв/цифр
        if (!has_alnum(word_start, word_len))
            continue;

        // копируем слово
        memcpy(dst, word_start, word_len);
        dst += word_len;
        *(dst++) = '\0';
        ON_DEBUG(printf("src = %p\n", src);)
        ON_DEBUG(printf("end = %p\n", end);)
    }
    ON_DEBUG(printf("exit while\n");)
    if ((dst - clean + 1) <= size)
        *dst = '\0';
    ON_DEBUG(printf("afte add \\0\n");)
    *new_size = dst - clean;
    ON_DEBUG(printf("size = %zu; new_size = %zu\n", size, *new_size);)
    return clean;
}
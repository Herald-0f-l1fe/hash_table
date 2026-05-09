#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "list/Array.h"     // list_t и операции
#include <stddef.h>         // size_t
#include <ctype.h>          // isalnum
#include <sys/stat.h>       //file.info
#include <stdbool.h>        // для bool (или замените на int)

typedef size_t (*hash_func_t)(const char* word, size_t mod);

struct hash_table_t
{
    list_t* buckets;
    size_t  bucket_count;
    char*   buffer;        
    size_t  buffer_size;
    hash_func_t hash;       // выбранная хэш-функция
    bool    owns_buffer;   // true, если нужно освободить буфер в destroy
};

enum HashFunctionType
{
    HASH_DJB2 = 0,
    HASH_CONST_0,
    HASH_CONST_1,
    HASH_FIRST_CHAR,
    HASH_SUM_ASCII,
    HASH_SUM_ROTATE_LEFT,
    HASH_SUM_ROTATE_RIGHT
};

/**
 * @brief Создать хэш-таблицу заданного размера.
 * @param table     Указатель на ещё не инициализированную структуру.
 * @param capacity  Количество бакетов (желательно простое число).
 * @param type      Тип хэш-функции для использования.
 * @return OK или код ошибки.
 */
ARR_errors hash_table_init(hash_table_t* table, size_t capacity,
                           hash_func_t hash);

/**
 * @brief Вставить слово в таблицу. Если слово уже есть, ничего не делает.
 * @return OK или код ошибки.
 */
ARR_errors hash_table_insert(hash_table_t* table, const char* word);

/**
 * @brief Найти слово в таблице.
 * @return Указатель на хранящуюся строку (arr_value) или nullptr, если слово не найдено.
 */
arr_value hash_table_find(const hash_table_t* table, const char* word);

/**
 * @brief Удалить слово из таблицы.
 * @return OK, если удалено, или ERROR, если не найдено, или код ошибки.
 */
ARR_errors hash_table_remove(hash_table_t* table, const char* word);

/**
 * @brief Уничтожить хэш-таблицу и освободить всю память.
 */
void hash_table_destroy(hash_table_t* table);

/**
 * @brief Заполняет массив sizes количеством элементов в каждой корзине.
 * @param table Указатель на хэш-таблицу.
 * @param sizes Массив длиной не менее bucket_count для сохранения размеров.
 */
void hash_table_compute_histogram(const hash_table_t* table, size_t* sizes);

/**
 * @brief Выводит гистограмму распределения элементов по корзинам в поток stream.
 * @param table Указатель на хэш-таблицу.
 * @param stream Файловый поток для вывода (например, stdout).
 */
void hash_table_print_histogram(const hash_table_t* table, FILE* stream);

/**
 * @brief Извлекает все слова из файла и добавляет их в хэш-таблицу.
 * @param table   Указатель на инициализированную таблицу.
 * @param filename Имя текстового файла для чтения.
 * @return OK при успешной обработке, ERROR при ошибке ввода-вывода или нехватке памяти.
 */
ARR_errors hash_table_load_from_file(hash_table_t* table, const char* filename);

/**
 * @brief Извлекает все слова из переданной строки и добавляет их в таблицу.
 * @param table Указатель на таблицу.
 * @param text  Текст, завершающийся нулём. Может быть большого размера.
 * @return OK или ERROR.
 */
ARR_errors hash_table_load_from_text(hash_table_t* table, const char* text);

/**
 * @brief Извлекает весь текст из buffer
 * @param size Размер текста в char.
 * @param file_name Название файла для открытия.
 * @return Указатель на буффер с текстом.
 */
extern char* read_from_file_to_buffer(long int* size, const char* file_name );

/**
 * @brief Находит количество символов в файле.
 * @param file_name Название файла для открытия.
 * @return Количество символов в файле.
 */
extern long int finding_file_size(const char* file_name);
/**
 * @brief Сжимает буфер, удаляя все символы, не являющиеся частью слова, и оставляя по одному '\0' между словами.
 * @param buffer Указатель на буфер с текстом.
 * @param size Размер буфера в байтах.
 * @return Ошибку при ошибке или ок.
 */
ARR_errors hash_table_load_from_file_buffer(hash_table_t* table, const char* filename);

/**
 * @brief Алоцирует память в heap-buffer и переписывает size символов туда  
 * @param size Количество символов для записи.
 * @param file_pointer Файл открытый на запись, куда будут записываться данные.
 * @return Указатель на буффер с данными.
 */
extern char* filling_the_buffer_with_text(long int size, FILE* file_pointer);

/**
 * @brief Записывает размер корзин table в список sizes  
 * @param table Количество символов для записи.
 * @param sizes Список куда будут записываться размеры корзин.
 */
void hash_table_compute_histogram(const hash_table_t* table, size_t* sizes);

/**
 * @brief Открывает gnuplot и строит гистограмму распределения элементов по корзинам.  
 * @param table Указатель на хэш-таблицу.
 * @return Указатель на буффер с данными.
 */
void hash_table_plot_histogram(const hash_table_t* table);

/**
 * @brief Проверяет, содержит ли строка хотя бы один алфавитно-цифровой символ.
 * @param str Указатель на строку для проверки.
 * @param len Длина строки.
 * @return 1, если строка содержит алфавитно-цифровой символ, иначе 0.
 */
int has_alnum(const char* str, size_t len);

/**
 * @brief Строит гистограмму распределения элементов по корзинам и сохраняет её в PNG-файл.
 * @param table Указатель на хэш-таблицу.
 * @param filename Имя файла для сохранения PNG-графика.
 */
void hash_table_histogram_to_png(const hash_table_t* table, const char* filename);
#endif
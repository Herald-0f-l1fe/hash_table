#include "hash_table.h"


size_t hash_djb2(const char* word, size_t mod)
{
    size_t hash = 5381;
    int c;
    while ((c = (unsigned char)*word++))
        hash = ((hash << 5) + hash) + c;   // hash * 33 + c
    return hash % mod;
}

/* Константа 0 */
size_t hash_const_0(const char* word, size_t mod)
{
    (void)word;
    return 0;
}

/* Константа 1 */
size_t hash_const_1(const char* word, size_t mod)
{
    (void)word;
    return 1;
}

/* ASCII-код первого символа */
size_t hash_first_char(const char* word, size_t mod)
{
    if (*word == '\0')
        return 0;
    return (unsigned char)word[0] % mod;
}

/* Сумма ASCII-кодов всех символов */
size_t hash_sum_ascii(const char* word, size_t mod)
{
    size_t sum = 0;
    while (*word)
        sum += (unsigned char)*word++;
    return sum % mod;
}

/* Сумма со сдвигом влево (на 1 бит после каждого символа) */
size_t hash_sum_rotate_left(const char* word, size_t mod)
{
    size_t hash = 0;
    while (*word) {
        hash = (hash << 1) + (unsigned char)*word;
        ++word;
    }
    return hash % mod;
}

/* Сумма со сдвигом вправо (циклический сдвиг? сделаем логический сдвиг вправо) */
size_t hash_sum_rotate_right(const char* word, size_t mod)
{
    size_t hash = 0;
    size_t len = strlen(word);
    for (size_t i = 0; i < len; ++i) {
        // Сдвиг вправо на 1 бит, старший бит заполняется 0 (логический)
        hash = (hash >> 1) + ((size_t)(unsigned char)word[i] << (sizeof(size_t)*8 - 1));
    }
    return hash % mod;
}



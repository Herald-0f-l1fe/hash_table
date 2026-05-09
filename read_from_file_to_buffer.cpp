#include "hash_table.h"

char* read_from_file_to_buffer(long int* size, const char* file_name)
{
    FILE* file_pointer = fopen(file_name, "r");
    if (file_pointer == nullptr) // NULL
        printf("File %s didn't open\n", file_name);

    *size = finding_file_size(file_name);

    char* buffer = filling_the_buffer_with_text(*size, file_pointer);

    fclose(file_pointer);
    return buffer;
}

int line_counter(char* buffer)
{
    int cnt = 0;
    for (char* buffer1 = strchr(buffer, '\n'); buffer1 != NULL; buffer1 = strchr(buffer1 + 1, '\n'))
    {
        if (*(buffer1 + 1) != '\n')
            cnt++;
    }

    return cnt;
}

long int finding_file_size(const char* file_name)
{
    struct stat file_info = {};
    if (stat(file_name, &file_info) == 0) 
    {
        printf("Size of file %ld\n", file_info.st_size);
    }
    else
    {
        printf("smth went wrong, erasing all data on hard drive...\n");
    }
    return file_info.st_size;
}

char* filling_the_buffer_with_text(long int size, FILE* file_pointer)
{
    char* buffer = (char*) calloc(size + 2, sizeof(char));
    if (buffer == nullptr)
        printf("Memory for the buffer was not allocated.\n");
    
    size_t buffer_size = fread(buffer, sizeof(char), size, file_pointer);

    buffer[buffer_size++] = '\0';
    buffer[buffer_size++] = '\0';

    return buffer;
}

#include "Array.h"

FILE* open_file()
{
    FILE* fp = fopen("graph.dot", "w");
    assert(fp != 0);
    return fp;
}

ARR_errors create_file(FILE* fp, list_t arr)
{
    assert(fp);
    fprintf(fp, "digraph G {\n"
                "rankdir=LR\n"
                "splines=ortho\n"
                "node [style=filled, fillcolor=red, fontcolor=white, shape=octagon]\n"
            );

    char block[200] = {};
    char edge[200] = {};
    
    for (size_t i = 0; i < arr.capacity; i++)
    {
        edge_create(arr.array[i], i, edge);
        block_creater(arr.array[i], i, block);

        fprintf(fp, block);
        fprintf(fp, edge);
    }
    set_graph(fp, arr);
    fprintf(fp, "}\n");
    fclose(fp); 
    return OK;
}

char* block_creater(block_t node, size_t node_number, char* block)
{
    sprintf(block, "%lu [shape=Mrecord, label =\"index = %lu | elem = %s | next = %lu | prev = %ld\", style=filled, fontcolor=\"black\"", node_number, node_number, node.elem, node.next, node.prev);
    if (node.prev == -1)
        strcat(block, ", fillcolor=green");
    else 
        strcat(block, ", fillcolor=white");
    strcat(block, "];\n");
    return block;
}

char* edge_create(block_t node, size_t index, char* edge)
{
    char prev_edge[50] = {};

    sprintf(prev_edge, "%lu->%ld[color=\"green\"]\n", index, node.prev);
    sprintf(edge, "%lu->%lu[color=\"blue\"];\n", index, node.next);
    if (node.prev != -1)
        strcat(edge, prev_edge);

    return edge;
}

ARR_errors create_graph(size_t* file_number, list_t arr)
{
    FILE* fp = open_file();
    create_file(fp, arr);

    char system_cmd[200] = {}; 
    system_graph(*file_number, system_cmd);

    int prove = system(system_cmd);  
    if (prove != 0)
        return error_in_creating_graph;

    (*file_number)++;

    
    return OK;
}

char* system_graph(size_t file_number, char* system_cmd)
{
    char cmd[100] = {};
    sprintf(system_cmd, "mkdir -p /mnt/c/Users/boris/projects/Array/photo\n");

    sprintf(cmd, "dot -T png graph.dot -o /mnt/c/Users/boris/projects/Array/photo/graph%lu.png\n", file_number);
    strcat(system_cmd, cmd);
    return system_cmd;
}

ARR_errors set_graph(FILE* fp, list_t arr)
{
    for (size_t i = 0; i < arr.capacity - 1; i++)
    {
        fprintf(fp, "%lu->%lu[color=\"lightcyan\" weight=5000]\n", i, i + 1);
    }
    return OK;
}
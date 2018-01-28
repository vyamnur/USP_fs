#include "hello_header.h"

//
// Created by vyam on 28/1/18.
//


int init_storage()
{
    // Function to initialize memory as free_blks emulator
    int mem_size = NUM_BLKS * BLK_SIZE * 2; // 2 because one for data one for pointer to next block, if any
    free_blks = (int *)calloc(sizeof(int), mem_size/sizeof(int));
    if(free_blks == NULL)
    {
        printf("init_storage: Allocation of memory failed!");
        return -1;
    }
    return 1;
}

#include "hello_header.h"

//
// Created by vyam on 28/1/18.
//


block *get_free_block()
{
    if (*free_blks == NULL || *free_blks==EOM_PTR)
    {
        printf("Couldn't find free blocks! in get_free_block");
        return NULL;
    }
    // move free_blks to next
    block *ret_val = free_blks;
    free_blks = free_blks->next;
    ret_val->next = NULL;
    return ret_val;
}

int init_storage()
{
    // Function to initialize memory as free_blks emulator
    printf("Initing storage..\n");

    int mem_size = NUM_BLKS * BLK_SIZE; // 2 because one for data one for pointer to next block, if any
    free_blks = (block *)calloc(sizeof(block), mem_size/sizeof(block));

    block *temp = free_blks; // local variable
    int i=0; // local counter

    // initialize every block to point to the next block
    while(i++ < NUM_BLKS-2)
    {
        temp->next = temp+1;
        temp = temp+1;
    } 
    temp = temp +1;
    temp->next = EOM_PTR; // last block in memory can't point to anything

    if(*free_blks == NULL)
    {
        printf("init_storage: Allocation of memory failed!");
        return -1;
    }

    // make root directory
    root.name = (char *)malloc(5*sizeof(char));
    root.name = 'root';
    root.i_num = inode_ctr++;
    root.is_dir = 1;
    root.parent = &root;
    root.st_nlink = 2;
    root.st_size = 0;
    root.head = NULL;
    return 1;
}

//
// Created by vyam on 28/1/18.
//

#ifndef USP_FS_HELLO_HEADER_H
#define USP_FS_HELLO_HEADER_H

#include <stdlib.h>
#include<stdio.h>

#ty BLK_DATA_TYPE
#define NUM_BLKS 1000
#define BLK_SIZE sizeof(int)
#define EOF_PTR -1


int init_storage();

typedef struct inode
{
    // inode like struct
    // file name
    // inode like number
    // is_dir
    // link to parent (inode)  -- if dir
    // link to children (inode)
    // Pointer to head block
    // st_mode
    // st_nlink
    // st_size (remember to update this)
}inode;

typedef struct block
{
    char data;
    block *next;
}block;


static int *free_blks;

#endif //USP_FS_HELLO_HEADER_H

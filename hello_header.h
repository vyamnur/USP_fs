//
// Created by vyam on 28/1/18.
//

#ifndef USP_FS_HELLO_HEADER_H
#define USP_FS_HELLO_HEADER_H

#include <stdlib.h>
#include <stdio.h>

typedef char BLK_DATA_TYPE;

#define NUM_BLKS 1000
#define BLK_SIZE sizeof(block)
#define EOM_PTR -1
#define MAX_PATH_LEN 252

int init_storage();


typedef struct block
{
    BLK_DATA_TYPE data[1024];
    struct block *next;
}block;


typedef struct inode
{
    char *name;
    int i_num;
    int is_dir=0;

    struct inode *parent;
    struct inode *children;

    int st_size;
    int st_nlink;
    block *head;


}inode;

inode root;
static block *free_blks;
static int inode_ctr=0;

#endif //USP_FS_HELLO_HEADER_H

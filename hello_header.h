//
// Created by vyam on 28/1/18.
//

#ifndef USP_FS_HELLO_HEADER_H
#define USP_FS_HELLO_HEADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef char BLK_DATA_TYPE;

#define NUM_BLKS 1000
#define BLK_SIZE sizeof(block)
#define EOM_PTR -1
#define MAX_PATH_LEN 252
#define MAX_CHILDREN 10
#define MAX_LEVEL 10

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
    int is_dir;

    struct inode *parent;
    struct inode *children[MAX_CHILDREN];

    int st_size;
    int st_nlink;
    block *head;


}inode;

/*------------------------------------ METHOD PROTOTYPES -------------------------------------*/
int init_storage();
block *get_free_block();


struct inode *root;
static block *free_blks;
static int inode_ctr=0;

struct inode *resolve_path(char *path, int is_dir);
struct inode *child_exists(struct inode *parent, char *child);
struct inode *createChild(struct inode *parent, char *child, int is_dir);

#endif //USP_FS_HELLO_HEADER_H

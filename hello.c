/*
 ******************************** Credits *****************************************

  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>
  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

 **********************************************************************************

	linked list approach to block management
 	One block for data, one block to point to next block of data
 	50% overhead !!

 Build and execute:
    gcc -c hello_funs.c && gcc -Wall hello_funs.o hello.c `pkg-config fuse --cflags --libs` -o hello
    mkdir blah
    ./hello -f blah

 To unmount:
    sudo umount -l blah


 */


#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "hello_header.h"

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";


extern struct inode *root;

static int hello_mkdir(const char *path_name, mode_t mode)
{
    /*inode new_dir;

    char path[MAX_PATH_LEN];
    strncpy(path, path_name, MAX_PATH_LEN);

    if (*path_name == NULL)
    {
        printf(stderr, "error: path = NULL during mkdir\n");
        return -1;
    }
    new_dir.i_num = inode_ctr++;
    new_dir.is_dir = 1;
    // new_dir.parent = parse path to get parent
    new_dir.st_nlink =2;
    return 1;

    */
    printf("root: %s\n", root->name);
    char *path = path_name;
    struct inode *temp = resolve_path(path, 1);
    printf("New dir name: %s\n", temp->name);
    return 0;
}



static int hello_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    // file access modes, is wrong have  to fix
    // change all this
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else if (strcmp(path, hello_path) == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(hello_str);
    } else
        res = -ENOENT;

    return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{

    (void) offset;
    (void) fi;

    // this code has to be changed
    /*
    if (strcmp(path, "/") != 0)
        return -ENOENT;
    */

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    if(strcmp(path, "/") == 0 && root->st_nlink > 2) {
        for(int i = 0; i < root->st_nlink - 2; i++){
            //struct inode *temp = root->children[0];
            //printf("Children: %d\n",temp);
            filler(buf, root->children[i]->name, NULL, 0);
        }
    }


    return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) == O_RDWR || (fi->flags & 3) == 0)
        return -EACCES;

    return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;

    len = strlen(hello_str);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, hello_str + offset, size);
    } else
        size = 0;

    return size;
}


static int hello_write(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
    // does not support negative offset!
    if(offset<0)
    {
        return -1;
    }
    // locate the file
    inode *fil = resolve_path(path,0); // 0 becasue we dont want to create a dir
    
    if(fil == NULL)
    {
        printf("Could not resolve path, in hello_write");
        return -1;
    }
    
    //new file, allocate memory and write
    if(fil->head == NULL)
    {
        fil->head = get_free_block();
        if(fil->head == NULL)
        {
            //printf("Didn't get free block! in hello_write");
            return -1;
        }
    }
    
    // seek to offset
    int write_blk_offset = 0; //offset within a block
    block *write_block = fil->head;
    while(offset>0)
    {
        if(offset>sizeof(write_block->data))
        {
            offset -= sizeof(write_block->data);
            
            if(write_block->next == NULL)
            {
                write_block->next = get_free_block();
                if(write_block->next == NULL)
                {
                    return -1;
                }
            }
            write_block = write_block->next;
        }
        else
        {
            write_blk_offset = offset;
            offset = 0;
        }
    }
    
    //start writing from buff
    int blk_ctr;
    while(buf!=NULL)
    {
        blk_ctr=0;
        while(blk_ctr<sizeof(write_block->data))
        {
            if(buf == NULL)
            {
                write_block->data[write_blk_offset+blk_ctr] = '\0';
                return 1; // done writing, return
            }
            write_block->data[write_blk_offset+blk_ctr] = buf[0];
            buf = buf+1;
            blk_ctr++;
        }
        // run out of blck go to next block
        if(write_block->next == NULL)
        {
            write_block->next = get_free_block();
            if(write_block->next == NULL)
            {
                return -1;
            }
        }
        write_blk_offset = 0;
        write_block = write_block->next;
        
    }
    return 1; // never reached
}


static struct fuse_operations hello_oper = {
        .getattr	= hello_getattr,
        .readdir	= hello_readdir,
        .open		= hello_open,
        .read		= hello_read,
        .write      = hello_write,
        .mkdir      = hello_mkdir,
};

int main(int argc, char *argv[])
{
    if(init_storage()==-1) // initialises storage and creates a root directory
    {
        return -1;
    }

    return fuse_main(argc, argv, &hello_oper, NULL);
}

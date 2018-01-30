/*
 ******************************** Credits *****************************************

  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>
  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
  gcc -Wall `pkg-config fuse --cflags --libs` hello.c -o hello

 **********************************************************************************

	linked list approach to block management
 	One block for data, one block to point to next block of data
 	50% overhead !!

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



static int hello_mkdir(const char *path_name, mode_t mode)
{
    inode new_dir;

    char path[MAX_PATH_LEN];
    strncpy(path, path_name, MAX_PATH_LEN);

    if (path == NULL)
    {
        printf(stderr, "error: path = NULL during mkdir\n");
        return -1;
    }
    new_dir.i_num = inode_ctr++;
    new_dir.is_dir = 1;
    // new_dir.parent = parse path to get parent
    new_dir.st_nlink =2;


}



static int hello_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    // file access modes, is wrong have  to fix
    /* change all this
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else if (strcmp(path, hello_path) == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(hello_str);
    } else
        res = -ENOENT;
     */
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
    filler(buf, hello_path + 1, NULL, 0);
    //filler(buf,"viky",NULL,0); // why NULL 0?

    return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
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

static struct fuse_operations hello_oper = {
        .getattr	= hello_getattr,
        .readdir	= hello_readdir,
        .open		= hello_open,
        .read		= hello_read,
};

int main(int argc, char *argv[])
{
    if(init_storage()==-1) // initialises storage and creates a root directory
    {
        return -1;
    }

    return fuse_main(argc, argv, &hello_oper, NULL);
}

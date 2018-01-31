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
#include <zconf.h>
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

static int hello_getattr(const char *path, struct stat *stbuf) {
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    // file access modes, is wrong have  to fix
    // change all this
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;

    } else if ((strcmp(path, "..") != 0) && (strcmp(path, ".") != 0)) {

        char *string, *found;
        char *dir_array[MAX_LEVEL];
        string = strdup(path);
        int i = -2;

        while ((found = strsep(&string, "/")) != NULL) {
            i++;
            printf("%s", found);
            dir_array[i] = found;
        }

        printf("hdsfjdsf: %s\n", dir_array[0]);

        if (i > -1) {
            struct inode *temp = root;
            temp = child_exists(temp, dir_array[0]);
            if (temp == NULL) {
                printf("Blah\n");
                res = -ENOENT;
            } else {
                printf("jhkjhkjhdfsdfsdf\n");
                stbuf->st_mode = (temp->is_dir == 1)?(S_IFDIR| 0755):(S_IFREG | 0444);
                stbuf->st_nlink = temp->st_nlink;
                stbuf->st_size = 0;
                stbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
                stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
                stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
                stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
            }
        }
    }
    else
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

static int hello_open(const char* path, struct fuse_file_info* fi)
{

    printf("Opened a file! %s\n",path);

    /*char *a = strdup(path);
    inode *i = createChild(root, a+1, 0);
    */
    //printf("Open done: %s\n", i->name);

    return 0;

}

static int hello_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    printf("Created a file %s\n",path);

    return hello_open(path,fi);
}


static int hello_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{

    printf("beginning write!\n");
    char *hj = strdup(path);
    // does not support negative offset!
    if(offset<0)
    {
        return 0;
    }
    // locate the file
    //inode *fil = resolve_path(path,0); // 0 becasue we dont want to create a dir

    inode *fil = createChild(root, hj+1 ,0); // 0 becasue we dont want to create a dir

    if(fil == NULL)
    {
        printf("Could not resolve path, in hello_write");
        return 0;
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
                    return 0;
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
                return 0; // done writing, return
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
                return 0;
            }
        }
        write_blk_offset = 0;
        write_block = write_block->next;
        
    }
    return 0; // never reached
}


static struct fuse_operations hello_oper = {
        .getattr	= hello_getattr,
        .readdir	= hello_readdir,
        .open		= hello_open,
        .read		= hello_read,
        .write      = hello_write,
        .mkdir      = hello_mkdir,
        .create     = hello_create,
};

int main(int argc, char *argv[])
{
    if(init_storage()==-1) // initialises storage and creates a root directory
    {
        return -1;
    }

    return fuse_main(argc, argv, &hello_oper, NULL);
}

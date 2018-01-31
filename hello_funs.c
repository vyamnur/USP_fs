#include "hello_header.h"

//
// Created by vyam on 28/1/18.
//


block *get_free_block()
{
    if (free_blks == NULL || free_blks==EOM_PTR)
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

    printf("1...\n");

    // initialize every block to point to the next block
    while(i++ < NUM_BLKS-2)
    {
        temp->next = temp+1;
        temp = temp+1;
    }
    temp = temp +1;
    temp->next = EOM_PTR; // last block in memory can't point to anything

    printf("2...\n");

    if(free_blks == NULL)
    {
        printf("init_storage: Allocation of memory failed!");
        return -1;
    }

    root = (inode *)malloc(sizeof(inode));

    // make root directory

    root->name = "/";
    root->i_num = inode_ctr++;
    root->is_dir = 1;
    root->parent = NULL;
    root->st_nlink = 2;
    root->st_size = 0;
    root->head = NULL;

    printf("Init done!\n");
    return 1;
}

struct inode *resolve_path(char *path, int is_dir) {

    char *string,*found;
    char *dir_array[MAX_LEVEL];
    string = strdup(path);

    printf("1..");

    //Split path by '/' and store in an array
    int i = -2;

    while( (found = strsep(&string,"/")) != NULL ) {
        i++;
        dir_array[i] = found;
    }
    printf("dir_array: %s\n", dir_array[0]);
    if(i == 0) {
        return createChild(root, dir_array[0], is_dir);
    }

    printf("i: %d\n", i);

    printf("2.. Added all to array\n");

    //Get to the deepest directory in the path
    struct inode *temp = root;
    int j;

    for(j = 0; j < i - 1; j++) {
        if((temp = child_exists(temp, dir_array[j])) == NULL)
            printf("Directory %s does not exist\n", dir_array[j]);
    }

    printf("Traversed to last array\n");

    if((temp = child_exists(temp, dir_array[j]) == NULL)){
        printf("Checking last entry: IF\n");
        return createChild(temp, dir_array[j], is_dir);
    }
    else {
        printf("Checking last entry: ELSE\n");
        return temp;
    }
}

struct inode *child_exists(struct inode *parent, char *child) {
    printf("Checking if child exists\n");
    if(!parent){
        printf("Parent is null");
        return NULL;
    }
    if(parent->st_nlink < 3 || parent->is_dir != 1) {
        printf("child_exists: Not a directory or directory empty\n");
        return NULL;
    }

    struct inode* temp = parent->children[0];
    for(int i = 1; i < parent->st_nlink - 2; i++){
        if(strcmp(temp->name, child) == 0)
            return temp;
    }

    return NULL;
}

struct inode *createChild(struct inode *parent, char *child, int is_dir) {
    printf("create_child called parent: %s child: %s\n", parent->name, child);
    struct inode *temp = (struct inode *)malloc(sizeof(struct inode));
    //printf("1..\n");
    temp->name = child;
    temp->parent = parent;

    printf("%s\n", temp->name);

    temp->i_num = inode_ctr++;
    temp->is_dir = is_dir;

    temp->st_nlink = (is_dir == 1)?2:1;

    temp->st_size = 0;
    temp->head = NULL;

    parent->children[parent->st_nlink - 2] = temp;
    parent->st_nlink += 1;

    return temp;
}

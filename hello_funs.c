#include "hello_header.h"

//
// Created by vyam on 28/1/18.
//


block *get_free_block()
{
    if (free_blks == NULL)
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

    int mem_size = NUM_BLKS * BLK_SIZE;
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
    temp->next = NULL; // last block in memory can't point to anything

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

    printf("1..path: %s\n", path);


    /*Split path by '/' and store in an array
    Testing required*/
    int i = -2; //-2 because strsep returns two garbage values before actual inodes
    while( (found = strsep(&string,"/")) != NULL ) {
        i++;
        //printf("%s", found);
        dir_array[i] = found;
    }
    for(int k = 0; k <= i; k++) {
        printf("Path %d: %s\n", k, dir_array[k]);
    }
    printf("2.. Added all to array\n");

    //printf("dir_array: %s\n", dir_array[0]);
    //printf("i: %d\n", i);
    if(i == 0) {
        inode *t = child_exists(root, dir_array[0]);
        if(t != NULL) return t;//For getattr
        else if(is_dir != 2) return createChild(root, dir_array[0], is_dir);
        else return NULL;
    }


    //Get to the deepest directory in the path. Assumes child_exists works
    struct inode *temp = root;
    int j;

    for(j = 0; j < i; j++) {
        if((temp = child_exists(temp, dir_array[j])) == NULL) {
            printf("Directory %s does not exist\n", dir_array[j]);
            return NULL;
        }
    }

    printf("j: %d d: %s\n", j, dir_array[1]);

    printf("Traversed to last directory\n");

    struct inode *temp1 = temp;
    if((temp = child_exists(temp, dir_array[j])) == NULL){
        printf("Checking last entry: IF\n");
        if(is_dir != 2) return createChild(temp1, dir_array[j], is_dir);
        return NULL;
    }
    else {
        printf("Checking last entry: ELSE temp: %s\n", temp->name);
        return temp;
    }
}

struct inode *child_exists(struct inode *parent, char *child) {
    printf("Checking if child exists\n");
    if(!parent){
        printf("Parent is null\n");
        return NULL;
    }
    if(parent->st_nlink < 3 || parent->is_dir != 1) {
        printf("child_exists: Not a directory or directory empty: %s\n", parent->name);
        return NULL;
    }

    struct inode* temp = parent->children[0];
    printf("temp in ex: %s\n", temp->name);
    printf("Checking for child: %s in parent %s\n", child, parent->name);

    for(int i = 0; i <= parent->st_nlink - 2; i++){
        if(strcmp(strdup(temp->name), strdup(child)) == 0){
            printf("Child found\n");
            return temp;
        }
    }
    printf("Child not found\n");
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

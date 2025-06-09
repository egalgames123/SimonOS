#ifndef FS_H
#define FS_H

#define MAX_FILES 32
#define MAX_FILENAME_LEN 32
#define MAX_FILE_CONTENT 256

typedef struct {
    char name[MAX_FILENAME_LEN];
    char content[MAX_FILE_CONTENT];
    int used;
} File;

void fs_init();
int fs_create(const char* name);
int fs_delete(const char* name);
const char* fs_read(const char* name);
int fs_write(const char* name, const char* content);
void fs_list();

#endif

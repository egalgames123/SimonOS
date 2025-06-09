#include "fs.h"
#include "kernel.h"  // für print()

static File files[MAX_FILES];

void fs_init() {
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].used = 0;
    }
}

int fs_create(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strncmp(files[i].name, name, MAX_FILENAME_LEN) == 0) {
            return 0;  // bereits vorhanden
        }
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            files[i].used = 1;
            for (int j = 0; j < MAX_FILENAME_LEN - 1 && name[j]; j++)
                files[i].name[j] = name[j];
            files[i].name[MAX_FILENAME_LEN - 1] = '\0';
            files[i].content[0] = 0;
            return 1;
        }
    }
    return 0;  // kein Platz
}

int fs_delete(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strncmp(files[i].name, name, MAX_FILENAME_LEN) == 0) {
            files[i].used = 0;
            return 1;
        }
    }
    return 0;
}

const char* fs_read(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strncmp(files[i].name, name, MAX_FILENAME_LEN) == 0) {
            return files[i].content;
        }
    }
    return 0;
}

int fs_write(const char* name, const char* content) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strncmp(files[i].name, name, MAX_FILENAME_LEN) == 0) {
            for (int j = 0; j < MAX_FILE_CONTENT - 1 && content[j]; j++)
                files[i].content[j] = content[j];
            files[i].content[MAX_FILE_CONTENT - 1] = '\0';
            return 1;
        }
    }
    return 0;
}

void fs_list() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            print(files[i].name);
            print("\n");
        }
    }
}

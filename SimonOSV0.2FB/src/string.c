#include "string.h"

int strncmp(const char* s1, const char* s2, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0')
            return (unsigned char)s1[i] - (unsigned char)s2[i];
    }
    return 0;
}

char* strncpy(char* dest, const char* src, unsigned int n) {
    unsigned int i = 0;
    for (; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

unsigned int strlen(const char* str) {
    unsigned int len = 0;
    while (str[len] != '\0') len++;
    return len;
}


char* strncat(char* dest, const char* src, unsigned int n) {
    unsigned int dest_len = 0;
    while (dest[dest_len] != '\0') {
        dest_len++;
    }

    unsigned int i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[dest_len + i] = src[i];
    }
    dest[dest_len + i] = '\0';

    return dest;
}
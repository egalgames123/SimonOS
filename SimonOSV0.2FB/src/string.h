#ifndef STRING_H
#define STRING_H

int strncmp(const char* s1, const char* s2, unsigned int n);
char* strncpy(char* dest, const char* src, unsigned int n);
char* strncat(char* dest, const char* src, unsigned int n);
unsigned int strlen(const char* str);

#endif

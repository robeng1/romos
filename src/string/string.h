

#ifndef STRING_H
#define STRING_H
#include <stdbool.h>
#include <common/dll.h>

int strlen(const char *s);
int strnlen(const char *ptr, int max);
bool isdigit(char c);
int tonumericdigit(char c);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int count);
int strncmp(const char *str1, const char *str2, int n);
int istrncmp(const char *s1, const char *s2, int n);
int strnlen_terminator(const char *str, int max, char terminator);
char tolower(char s1);
int strlen(const char *s);

char *strncpy(char *destString, const char *sourceString, int maxLength);

int strcmp(const char *dst, char *src);

void strcat(void *dest, const void *src);

char *strstr(const char *in, const char *str);

void itoa(char *buf, unsigned long int n, int base);

int atoi(char *string);

int isspace(char c);

int isprint(char c);

char *strdup(const char *src);

char *strsep(char **stringp, const char *delim);

list_t *str_split(const char *str, const char *delim, unsigned int *numtokens);

void sprintf(char *buf, const char *fmt, ...);

#endif
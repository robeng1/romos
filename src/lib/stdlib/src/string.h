#ifndef STDLIB_STRING_H
#define STDLIB_STRING_H

#include <stdbool.h>
#include <stdarg.h>

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
char *strsep(char **stringp, const char *delim);
void vsprintf(char *str, void (*putchar)(char), const char *format, va_list arg);
char *strtok(char *str, const char *delimiters);
void sprintf(char *buf, const char *fmt, ...);

#endif
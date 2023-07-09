#include "string.h"
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <common/printf.h>
#include <mm/memory.h>

#define TOLOWER(x) ((x) | 0x20) // Macro to convert a character to lowercase

int strlen(const char *s)
{
  const char *sc;

  for (sc = s; *sc != '\0'; ++sc)
    /* nothing */; // Loop through the string until the null terminator is found
  return sc - s;   // Return the difference between the current pointer and the start pointer to calculate the length
}

char tolower(char s1)
{
  if (s1 >= 65 && s1 <= 90) // Check if the character is uppercase
  {
    s1 += 32; // Convert the character to lowercase by adding 32 to its ASCII value
  }

  return s1; // Return the converted character
}

int strnlen(const char *ptr, int max)
{
  int i = 0;
  for (i = 0; i < max; i++)
  {
    if (ptr[i] == 0) // Check if the current character is null
      break;         // If so, break out of the loop
  }

  return i; // Return the length
}

int strnlen_terminator(const char *str, int max, char terminator)
{
  int i = 0;
  for (i = 0; i < max; i++)
  {
    if (str[i] == '\0' || str[i] == terminator) // Check if the current character is null or matches the terminator character
      break;                                    // If so, break out of the loop
  }

  return i; // Return the length
}

int istrncmp(const char *s1, const char *s2, int n)
{
  unsigned char u1, u2;
  while (n-- > 0)
  {
    u1 = (unsigned char)*s1++;
    u2 = (unsigned char)*s2++;
    if (u1 != u2 && tolower(u1) != tolower(u2)) // Check if the characters are not equal, considering case-insensitivity
      return u1 - u2;                           // Return the difference between the ASCII values
    if (u1 == '\0')
      return 0; // If the first string has ended, return 0 indicating equality
  }

  return 0; // Return 0 indicating equality
}

int strncmp(const char *str1, const char *str2, int n)
{
  unsigned char u1, u2;

  while (n-- > 0)
  {
    u1 = (unsigned char)*str1++;
    u2 = (unsigned char)*str2++;
    if (u1 != u2)     // Check if the characters are not equal
      return u1 - u2; // Return the difference between the ASCII values
    if (u1 == '\0')
      return 0; // If the first string has ended, return 0 indicating equality
  }

  return 0; // Return 0 indicating equality
}

char *strcpy(char *dest, const char *src)
{
  char *res = dest;
  while (*src != 0)
  {
    *dest = *src; // Copy the character from source to destination
    src += 1;     // Move the source pointer forward
    dest += 1;    // Move the destination pointer forward
  }

  *dest = 0x00; // Null terminate the destination string

  return res; // Return the pointer to the start of the destination string
}

char *strncpy(char *dest, const char *src, int count)
{
  int i = 0;
  for (i = 0; i < count - 1; i++)
  {
    if (src[i] == 0x00) // Check if the current character of source is null
      break;            // If so, break out of the loop

    dest[i] = src[i]; // Copy the character from source to destination
  }

  dest[i] = 0x00; // Null terminate the destination string
  return dest;    // Return the pointer to the start of the destination string
}

bool isdigit(char c)
{
  return (c >= '0') && (c <= '9');
}

int tonumericdigit(char c)
{
  return c - 48; // Convert the character digit to its corresponding numeric value
}

int strcmp(const char *dst, char *src)
{
  int i = 0;

  while ((dst[i] == src[i]))
  {
    if (src[i++] == 0)
      return 0;
  }

  return 1;
}

char *strstr(const char *in, const char *str)
{
  char c;
  uint32_t len;

  c = *str++;
  if (!c)
    return (char *)in;

  len = strlen(str);
  do
  {
    char sc;

    do
    {
      sc = *in++;
      if (!sc)
        return (char *)0;
    } while (sc != c);
  } while (strncmp(in, str, len) != 0);

  return (char *)(in - 1);
}

void strcat(void *dest, const void *src)
{
  char *end = (char *)dest + strlen(dest);
  memcpy((char *)end, (char *)src, strlen((char *)src));
  end = end + strlen((char *)src);
  *end = '\0';
}

void itoa(char *buf, unsigned long int n, int base)
{
  unsigned long int tmp;
  int i, j;

  tmp = n;
  i = 0;

  do
  {
    tmp = n % base;
    buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
  } while (n /= base);
  buf[i--] = 0;

  for (j = 0; j < i; j++, i--)
  {
    tmp = buf[j];
    buf[j] = buf[i];
    buf[i] = tmp;
  }
}

int atoi(char *string)
{
  int result = 0;
  unsigned int digit;
  int sign;

  while (isspace(*string))
  {
    string += 1;
  }

  /*
   * Check for a sign.
   */

  if (*string == '-')
  {
    sign = 1;
    string += 1;
  }
  else
  {
    sign = 0;
    if (*string == '+')
    {
      string += 1;
    }
  }

  for (;; string += 1)
  {
    digit = *string - '0';
    if (digit > 9)
    {
      break;
    }
    result = (10 * result) + digit;
  }

  if (sign)
  {
    return -result;
  }
  return result;
}

int isspace(char c)
{
  return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

int isprint(char c)
{
  return ((c >= ' ' && c <= '~') ? 1 : 0);
}

char *strdup(const char *src)
{
  int len = strlen(src) + 1;
  char *dst = kernel_zalloc(len);
  memcpy(dst, src, len);
  return dst;
}

char *strsep(char **stringp, const char *delim)
{
  char *s;
  const char *spanp;
  int c, sc;
  char *tok;
  if ((s = *stringp) == NULL)
    return (NULL);
  for (tok = s;;)
  {
    c = *s++;
    spanp = delim;
    do
    {
      if ((sc = *spanp++) == c)
      {
        if (c == 0)
          s = NULL;
        else
          s[-1] = 0;
        *stringp = s;
        return (tok);
      }
    } while (sc != 0);
  }
}

/*
   Split a string into list of strings
   */
list_t *str_split(const char *str, const char *delim, unsigned int *numtokens)
{
  list_t *ret_list = list_create();
  char *s = strdup(str);
  char *token, *rest = s;
  while ((token = strsep(&rest, delim)) != NULL)
  {
    if (!strcmp(token, "."))
      continue;
    if (!strcmp(token, ".."))
    {
      if (list_size(ret_list) > 0)
        list_pop(ret_list);
      continue;
    }
    list_push(ret_list, strdup(token));
    if (numtokens)
      (*numtokens)++;
  }
  kernel_free(s);
  return ret_list;
}

void sprintf(char *buf, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, NULL, fmt, ap);
  va_end(ap);
}

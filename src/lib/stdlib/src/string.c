#include "string.h"
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <memory.h>

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
char *sp = 0;
char *strtok(char *str, const char *delimiters)
{
  int i = 0;
  int len = strlen(delimiters);
  if (!str && !sp)
    return 0;

  if (str && !sp)
  {
    sp = str;
  }

  char *p_start = sp;
  while (1)
  {
    for (i = 0; i < len; i++)
    {
      if (*p_start == delimiters[i])
      {
        p_start++;
        break;
      }
    }

    if (i == len)
    {
      sp = p_start;
      break;
    }
  }

  if (*sp == '\0')
  {
    sp = 0;
    return sp;
  }

  // Find end of substring
  while (*sp != '\0')
  {
    for (i = 0; i < len; i++)
    {
      if (*sp == delimiters[i])
      {
        *sp = '\0';
        break;
      }
    }

    sp++;
    if (i < len)
      break;
  }

  return p_start;
}

int is_format_letter(char c)
{
  return c == 'c' || c == 'd' || c == 'i' || c == 'e' || c == 'E' || c == 'f' || c == 'g' || c == 'G' || c == 'o' || c == 's' || c == 'u' || c == 'x' || c == 'X' || c == 'p' || c == 'n';
}

void vsprintf_helper(char *str, void (*putchar)(char), const char *format, uint32_t *pos, va_list arg)
{
  char c;
  int sign, ival, sys;
  char buf[512];
  char width_str[10];
  uint32_t uval;
  uint32_t size = 8;
  uint32_t i;
  int size_override = 0;
  memset(buf, 0, 512);

  while ((c = *format++) != 0)
  {
    sign = 0;

    if (c == '%')
    {
      c = *format++;
      switch (c)
      {
      // Handle calls like printf("%08x", 0xaa);
      case '0':
        size_override = 1;
        // Get the number between 0 and (x/d/p...)
        i = 0;
        c = *format;
        while (!is_format_letter(c))
        {
          width_str[i++] = c;
          format++;
          c = *format;
        }
        width_str[i] = 0;
        format++;
        // Convert to a number
        size = atoi(width_str);
      case 'd':
      case 'u':
      case 'x':
      case 'p':
        if (c == 'd' || c == 'u')
          sys = 10;
        else
          sys = 16;

        uval = ival = va_arg(arg, int);
        if (c == 'd' && ival < 0)
        {
          sign = 1;
          uval = -ival;
        }
        itoa(buf, uval, sys);
        uint32_t len = strlen(buf);
        // If use did not specify width, then just use len = width
        if (!size_override)
          size = len;
        if ((c == 'x' || c == 'p' || c == 'd') && len < size)
        {
          for (i = 0; i < len; i++)
          {
            buf[size - 1 - i] = buf[len - 1 - i];
          }
          for (i = 0; i < size - len; i++)
          {
            buf[i] = '0';
          }
        }
        if (c == 'd' && sign)
        {
          if (str)
          {
            *(str + *pos) = '-';
            *pos = *pos + 1;
          }
          else
            (*putchar)('-');
        }
        if (str)
        {
          strcpy(str + *pos, buf);
          *pos = *pos + strlen(buf);
        }
        else
        {
          char *t = buf;
          while (*t)
          {
            putchar(*t);
            t++;
          }
        }
        break;
      case 'c':
        if (str)
        {
          *(str + *pos) = (char)va_arg(arg, int);
          *pos = *pos + 1;
        }
        else
        {
          (*putchar)((char)va_arg(arg, int));
        }
        break;
      case 's':
        if (str)
        {
          char *t = (char *)va_arg(arg, int);
          strcpy(str + (*pos), t);
          *pos = *pos + strlen(t);
        }
        else
        {
          char *t = (char *)va_arg(arg, int);
          while (*t)
          {
            putchar(*t);
            t++;
          }
        }
        break;
      default:
        break;
      }
      continue;
    }
    if (str)
    {
      *(str + *pos) = c;
      *pos = *pos + 1;
    }
    else
    {
      (*putchar)(c);
    }
  }
}

void vsprintf(char *str, void (*putchar)(char), const char *format, va_list arg)
{
  uint32_t pos = 0;
  vsprintf_helper(str, putchar, format, &pos, arg);
}

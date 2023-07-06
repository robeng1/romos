#include "string.h" // Include header file for string-related functionality

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

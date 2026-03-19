
#ifndef MINSTR_H
#define MINSTR_H

#include <stdint.h>

static char *strstr(const char *haystack, const char *needle) {
  if (!*needle)
    return (char *)haystack;
  for (; *haystack; haystack++) {
    const char *h = haystack;
    const char *n = needle;
    while (*h && *n && *h == *n) {
      h++;
      n++;
    }
    if (!*n)
      return (char *)haystack;
  }
  return 0;
}

static void *memset(void *s, int c, unsigned long n) {
  unsigned char *p = (unsigned char *)s;
  while (n--)
    *p++ = (unsigned char)c;
  return s;
}

static void *memcpy(void *dest, const void *src, unsigned long n) {
  unsigned char *d = (unsigned char *)dest;
  const unsigned char *s = (const unsigned char *)src;
  while (n--)
    *d++ = *s++;
  return dest;
}

static int memcmp(const void *s1, const void *s2, unsigned long n) {
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
  while (n--) {
    if (*p1 != *p2)
      return *p1 - *p2;
    p1++;
    p2++;
  }
  return 0;
}

static unsigned long strlen(const char *s) {
  const char *p = s;
  while (*p)
    p++;
  return (unsigned long)(p - s);
}

static int strcmp(const char *s1, const char *s2) {
  while (*s1 && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

static uintptr_t hex_str_to_u64(const char *s) {
  uintptr_t val = 0;
  while (*s) {
    if (*s >= '0' && *s <= '9')
      val = (val << 4) | (*s - '0');
    else if (*s >= 'a' && *s <= 'f')
      val = (val << 4) | (*s - 'a' + 10);
    else if (*s >= 'A' && *s <= 'F')
      val = (val << 4) | (*s - 'A' + 10);
    else
      break;
    s++;
  }
  return val;
}

#endif /* MINSTR_H */

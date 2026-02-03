#ifndef MINC_H
#define MINC_H

#include <stdint.h>
#include <sys/types.h>

/*
 * MINIMAL C LIBRARY IMPLEMENTATION
 *
 * Students must implement these basic C library functions from scratch.
 * No libc functions allowed - only pure C and the syscall interface.
 */

/* =============================================================================
 * STRING FUNCTIONS (IMPLEMENT THESE)
 * =============================================================================
 */

/* String length */
size_t strlen(const char *s);

/* String compare */
int strcmp(const char *s1, const char *s2);

/* String copy */
char *strcpy(char *dest, const char *src);

/* String concatenate */
char *strcat(char *dest, const char *src);

/* Find substring */
char *strstr(const char *haystack, const char *needle);

/* Find character */
char *strchr(const char *s, int c);

/* =============================================================================
 * MEMORY FUNCTIONS (IMPLEMENT THESE)
 * =============================================================================
 */

/* Memory copy */
void *memcpy(void *dest, const void *src, size_t n);

/* Memory set */
void *memset(void *s, int c, size_t n);

/* Memory compare */
int memcmp(const void *s1, const void *s2, size_t n);

/* =============================================================================
 * CONVERSION FUNCTIONS (IMPLEMENT THESE)
 * =============================================================================
 */

/* ASCII to integer */
int atoi(const char *str);

/* ASCII to long */
long atol(const char *str);

/* =============================================================================
 * OUTPUT HELPER FUNCTIONS (IMPLEMENT THESE)
 * =============================================================================
 */

/* Convert integer to string and write to fd */
void write_num(int fd, long num);

/* Convert integer to hex string and write to fd */
void write_hex(int fd, unsigned long num);

/* Parse hex string to bytes */
int parse_hex(const char *hex_str, uint8_t *bytes, size_t max_bytes);

#endif /* MINC_H */
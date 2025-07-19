#ifndef FXBF_HELPERS_H
#define FXBF_HELPERS_H

#include <stdint.h>

void wrapping_scrolling_putchar(const char c);
void wrapping_scrolling_puts(const char* s);
void wrapping_scrolling_printf(const char* fmt, ...);
char *read_entire_file(const char *path);
uint8_t get_ascii(uint8_t flags, uint8_t key);

extern const uint8_t keymap[];
extern const int keymap_len;

#endif
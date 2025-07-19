#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/keycodes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "helpers.h"

// 21x7 display
void wrapping_scrolling_putchar(const char c) {
	static int x = 0, y = 0;

	if (c != '\n') {
		const char s[] = { c, 0 };
		dtext(x, y, C_BLACK, s);

		x += 6;
	}

	if (x >= 124 || c == '\n') {
		x = 0;
		y += 8;

		if (y > 56) {
			uint8_t *vram = (uint8_t *)gint_vram;
			memmove(vram, vram + 128, 128 * 8);   // move 8 lines up
			memset(vram + 128 * 8, 0, 128);       // clear last line
			y=56;
		}
	}
}

void wrapping_scrolling_puts(const char* s) {
	while (*s) wrapping_scrolling_putchar(*(s++));
	dupdate();
}

void wrapping_scrolling_printf(const char* fmt, ...) {
	char str[512];
	va_list args;

	va_start(args, fmt);
	vsnprintf(str, 512, fmt, args);
	va_end(args);

	wrapping_scrolling_puts(str);
}

char *read_entire_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long size = ftell(f);
    if (size < 0) { fclose(f); return NULL; }
    rewind(f); // or fseek(f, 0, SEEK_SET);

    char *buf = malloc(size + 1);
    if (!buf) { fclose(f); return NULL; }

    size_t read_bytes = fread(buf, 1, size, f);
    fclose(f);

    if ((long)read_bytes != size) { free(buf); return NULL; }

    buf[size] = '\0'; // null-terminate for string use
    return buf;
}

uint8_t get_ascii(uint8_t flags, uint8_t key) {
    for (int i = 0; i < keymap_len; ++i) {
        const uint8_t* ent = &keymap[i*3];
        if (ent[1] == key && ent[0] == flags) return ent[2];
    }

    return 0;
}

// keymap shit

const uint8_t keymap[] = {
    // first val: first bit is shift, second is alpha
    // second val: casio keycode
    // third val: ascii char

    // numbers
    0b00, KEY_0, '0',
    0b00, KEY_1, '1',
    0b00, KEY_2, '2',
    0b00, KEY_3, '3',
    0b00, KEY_4, '4',
    0b00, KEY_5, '5',
    0b00, KEY_6, '6',
    0b00, KEY_7, '7',
    0b00, KEY_8, '8',
    0b00, KEY_9, '9',

    // special keys
    0b00, KEY_DOT, '.',
    0b00, KEY_COMMA, ',',
    0b00, KEY_LEFTPAR, '(',
    0b00, KEY_RIGHTPAR, ')',
    0b00, KEY_EXE, '\n',
    0b10, KEY_EXE, '\r',

    // shift symbols
    0b10, KEY_MUL, '{',
    0b10, KEY_DIV, '}',
    0b10, KEY_PLUS, '[',
    0b10, KEY_MINUS, ']',
    0b10, KEY_DOT, '=',

    // operators
    0b00, KEY_MUL, '*',
    0b00, KEY_DIV, '/',
    0b00, KEY_PLUS, '+',
    0b00, KEY_MINUS, '-',
    0b00, KEY_POWER, '^',

    // alphabet
    0b01, KEY_XOT, 'a',
    0b01, KEY_LOG, 'b',
    0b01, KEY_LN, 'c',
    0b01, KEY_SIN, 'd',
    0b01, KEY_COS, 'e',
    0b01, KEY_TAN, 'f',
    0b01, KEY_FRAC, 'g',
    0b01, KEY_FD, 'h',
    0b01, KEY_LEFTPAR, 'i',
    0b01, KEY_RIGHTPAR, 'j',
    0b01, KEY_COMMA, 'k',
    0b01, KEY_STORE, 'l',
    0b01, KEY_7, 'm',
    0b01, KEY_8, 'n',
    0b01, KEY_9, 'o',
    0b01, KEY_4, 'p',
    0b01, KEY_5, 'q',
    0b01, KEY_6, 'r',
    0b01, KEY_MUL, 's',
    0b01, KEY_DIV, 't',
    0b01, KEY_1, 'u',
    0b01, KEY_2, 'v',
    0b01, KEY_3, 'w',
    0b01, KEY_PLUS, 'x',
    0b01, KEY_MINUS, 'y',
    0b01, KEY_0, 'z',
    0b01, KEY_DOT, ' ',
};

const int keymap_len = sizeof(keymap) / 3;
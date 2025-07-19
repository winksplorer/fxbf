#include <gint/display.h>
#include <gint/keyboard.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gint/keycodes.h"
#include "helpers.h"

#define MEMORY_SIZE 30000
#define MAX_PROGRAM_SIZE 65536
#define MAX_NESTING 255

int main(void)
{
	dclear(C_WHITE);
	wrapping_scrolling_puts("starting fxbf\n");

	uint8_t* memory = calloc(MEMORY_SIZE, 1);
    uint16_t memptr = 0;
	wrapping_scrolling_puts("memory init\n");

    char* program = read_entire_file("program.bf");
	if (!program) {
		wrapping_scrolling_puts("could not read file\npress MENU to exit\n");
		dupdate();
        free(memory);
		for(;;) getkey();
	}

	wrapping_scrolling_puts("file read\n");

    size_t program_len = strlen(program);

    uint16_t jumps[MAX_PROGRAM_SIZE];
	memset(jumps, 0, sizeof(jumps[0]));
    uint16_t stack[MAX_NESTING];
    uint8_t depth = 0;

    for (size_t i = 0; i < program_len; ++i) {
        if (program[i] == '[') {
            if (depth >= MAX_NESTING) {
                wrapping_scrolling_printf("too much nesting (%d >= %d)\npress MENU to exit\n", depth, MAX_NESTING);
				dupdate();
                free(memory);
                free(program);
                for(;;) getkey();
            }
            stack[depth++] = i;
        } else if (program[i] == ']') {
            if (!depth) {
                wrapping_scrolling_puts("unmatched ]\npress MENU to exit\n");
				dupdate();
                free(memory);
                free(program);
				for(;;) getkey();
            }
            uint16_t open = stack[--depth];
            jumps[open] = i;
            jumps[i] = open;
        }
    }
	wrapping_scrolling_puts("jump table init\n");

	if (depth) {
        wrapping_scrolling_puts("unmatched [\npress MENU to exit\n");
		dupdate();
        free(memory);
        free(program);
		for(;;) getkey();
    }


	wrapping_scrolling_printf("running program.bf:\n\n");

    key_event_t key;
    uint8_t exit_flag = 0;
    for (size_t programptr = 0; programptr < program_len; ++programptr) {
        clearevents();
        if (exit_flag || keydown(KEY_EXIT)) break;

        switch (program[programptr]) {
            case '>': memptr++; break;
            case '<': memptr--; break;
            case '+': memory[memptr]++; break;
            case '-': memory[memptr]--; break;
            case '.':
				wrapping_scrolling_putchar(memory[memptr]);
				dupdate();
				break;
            case ',': 
				key = getkey();
                if (key.key == KEY_EXIT) {
                    exit_flag = 1;
                    break;
                }

				memory[memptr] = get_ascii(key.mod ? ((key.shift << 1) | key.alpha) : 0, key.key);
				break;
            case '[':
                if (!memory[memptr]) programptr = jumps[programptr];
                break;
            case ']':
                if (memory[memptr]) programptr = jumps[programptr];
                break;
        }
    }

	free(memory);
	free(program);

	wrapping_scrolling_printf("\n\nexecution finished\npress MENU to exit");

	// not an infinite loop of sadness, getkey exits when menu is pressed
	for(;;) getkey();
	return 1;
}
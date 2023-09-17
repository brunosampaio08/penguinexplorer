#ifndef PE_APPLICATION_H
#define PE_APPLICATION_H

#include <ncurses.h>

#define pELOG(fmt, ...) \
	fprintf(stderr, "[%s] [%s]: %d: " fmt "\n", \
			LOG_TAG, __func__, __LINE__, ##__VA_ARGS__);

struct window_desc {
	int height;
	int width;
	int starty;
	int startx;
	int pointer_y;
	int pointer_x;
	WINDOW *win;
};

enum tutorial_command_t
{
	PSTACK,
	COMMAND,
	PCODE,
	PTUTORIAL,
	EXIT_CMD,
};

void print_code(FILE* tutorial_file, FILE* code_file);

void print_to_window(char* message, struct window_desc *window);

void read_and_print_file(FILE *file, char* start_regex,\
		char* stop_regex, struct window_desc *window, int should_print_start, int should_print_stop,\
		int cflags);

void print_stack(struct window_desc* stack_window, FILE* stack_file);

int exec_tutorial_command(struct window_desc *tutorial_window, FILE* tutorial_file, char* command);

void print_tutorial(int tutorial_number, \
		struct window_desc *tutorial_window, struct window_desc *stack_window, struct window_desc *command_window);

void gdb_run(char *str);

#endif /* PE_APPLICATION_H */

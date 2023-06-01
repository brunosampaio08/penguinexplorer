#include <ncurses.h>

#include "shell.h"

struct window_desc {
	int height;
	int width;
	int starty;
	int startx;
	WINDOW *win;
};

int main(int argc, char **argv)
{
	struct window_desc prompt_window = {.height = 15, .width = 17, .starty = 0, .startx = 0};
	int rows, cols;
	
	struct shell_struct my_shell;
	
	initscr();
	raw();

	getmaxyx(stdscr, rows, cols);

	prompt_window.height = rows;
	prompt_window.width = cols/2;

	prompt_window.win = newwin(prompt_window.height, prompt_window.width, prompt_window.starty, prompt_window.startx);
	refresh();

	// making box border with default border styles
	box(prompt_window.win, 0, 0);
	mvwprintw(prompt_window.win, ++prompt_window.starty, ++prompt_window.startx, "prompt >");

	// refreshing the window
	wrefresh(prompt_window.win);

	//my_shell = start_shell();

	char unparsed_cmd[100];
	getstr(unparsed_cmd);
	printw(unparsed_cmd);
	do{
		run_shell(unparsed_cmd);
		getstr(unparsed_cmd);
	}while(!strcmp(unparsed_cmd, "exit"));

	getch();

	delete_shell(my_shell, 0);

	delwin(prompt_window.win);
	endwin();

	return 0;
}

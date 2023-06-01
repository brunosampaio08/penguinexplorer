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
	
	initscr();
	raw();

	getmaxyx(stdscr, rows, cols);

	prompt_window.height = rows;
	prompt_window.width = cols/2;

	prompt_window.win = newwin(prompt_window.height, prompt_window.width, prompt_window.starty, prompt_window.startx);
	refresh();

	// making box border with default border styles
	box(prompt_window.win, 0, 0);
	mvwprintw(prompt_window.win, ++prompt_window.starty, ++prompt_window.startx, "prompt > ");

	// refreshing the window
	wrefresh(prompt_window.win);

	char unparsed_cmd[100];
	wgetnstr(prompt_window.win, unparsed_cmd, 100);
	while(strcmp(unparsed_cmd, "exit"))
	{
		wmove(prompt_window.win, ++prompt_window.starty, 5);
		run_shell(unparsed_cmd);
		mvwprintw(prompt_window.win, ++prompt_window.starty, 1, "prompt > ");
		wgetnstr(prompt_window.win, unparsed_cmd, 100);
	}

	delwin(prompt_window.win);
	endwin();

	return 0;
}

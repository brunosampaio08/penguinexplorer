//TODO make a shell_delete and make shell_start better

#include <ncurses.h>

#include "shell.h"

struct window_desc {
	int height;
	int width;
	int starty;
	int startx;
	int pointer_y;
	int pointer_x;
	WINDOW *win;
};

int main(int argc, char **argv)
{
	struct window_desc prompt_window = {.height = 15, 
					.width = 17, 
					.starty = 0, 
					.startx = 0,
					.pointer_y = 1,
					.pointer_x = 1};
	int rows, cols;
	int return_value;

	FILE *output_file;

	char unparsed_cmd[100];
	char *path;
	char ch;

	// init screen
	initscr();
	// dont wait for newline
	cbreak();
	// turn on scrolling
	scrollok(prompt_window.win, TRUE);
	// recebei special keys like arrow and fX
	keypad(stdscr, TRUE);

	getmaxyx(stdscr, rows, cols);

	prompt_window.height = rows;
	prompt_window.width = cols/2;

	prompt_window.win = newwin(prompt_window.height, prompt_window.width, prompt_window.starty, prompt_window.startx);

	// making box border with default border styles
	box(prompt_window.win, 0, 0);
	// refreshing the window
	wrefresh(prompt_window.win);

	char buffer[1000];

	freopen("tmp/tmp_err", "a+", stderr);

	path = shell_start();

	do
	{
		mvwprintw(prompt_window.win, prompt_window.pointer_y++, prompt_window.startx+1, "prompt > ");
		wrefresh(prompt_window.win);
		wgetnstr(prompt_window.win, unparsed_cmd, 100);
		return_value = run_shell(unparsed_cmd);
		if(return_value == -1){
			mvwprintw(prompt_window.win, prompt_window.pointer_y++, prompt_window.startx+1, "Command failed. Are you sure it exists?");
			continue;
		}
		output_file = fopen(path, "rw");
		while((ch = fgetc(output_file)) != EOF)
		{	
			if(prompt_window.pointer_y == (prompt_window.height-2)){
				char tmp_ch;
				mvwprintw(prompt_window.win, prompt_window.height-1, prompt_window.startx, "Page End. Pres \"n\" to go to next page.");
				noecho();
				while((tmp_ch = wgetch(prompt_window.win)) != 'n');
				wclear(prompt_window.win);
				wrefresh(prompt_window.win);
				box(prompt_window.win, prompt_window.starty, prompt_window.startx);
				prompt_window.pointer_y = prompt_window.starty+1;
				prompt_window.pointer_x = prompt_window.startx+1;
				echo();
			}
			if(ch == '\n'){
				prompt_window.pointer_y++;
				prompt_window.pointer_x = 1;
				wmove(prompt_window.win, prompt_window.pointer_y, prompt_window.pointer_x);
				continue;
			}
			if(prompt_window.pointer_x >= (prompt_window.width-2)){
				prompt_window.pointer_y++;
				prompt_window.pointer_x = 1;
			}
			mvwaddch(prompt_window.win, prompt_window.pointer_y, prompt_window.pointer_x++, ch);
			wrefresh(prompt_window.win);
		}
		if(prompt_window.pointer_x != 1){
			prompt_window.pointer_y++;
			prompt_window.pointer_x = 1;
		}
		fclose(output_file);
	}while(strcmp(unparsed_cmd, "exit"));

	delwin(prompt_window.win);
	endwin();

	return 0;
}

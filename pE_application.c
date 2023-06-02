//TODO make shell_start better

#include <ncurses.h>
#include <regex.h>

#include "shell.h"

#define GDB_TMP "./tmp/gdb.tmp"

struct window_desc {
	int height;
	int width;
	int starty;
	int startx;
	int pointer_y;
	int pointer_x;
	WINDOW *win;
};

void read_and_print_gdbTmp(FILE *gdb_file, char* regex, struct window_desc window){
	regex_t fname_regex;
	char buffer[1000]; //no line should be 1000 chars
	char ch;
	int kp = 0;

	regcomp(&fname_regex, regex, REG_EXTENDED);

	while(fgets(buffer, 1000, gdb_file)){
		if(!regexec(&fname_regex, buffer, 0, NULL, 0)){ //check if it's our file. if it is, then next lines until an empty line should be our symbols
			mvwprintw(window.win, window.pointer_y, window.pointer_x, buffer);
			wrefresh(window.win);
			window.pointer_y++;
			while(kp || ((ch = fgetc(gdb_file)) != EOF))
			{	
				kp = 0;
				if(window.pointer_y == (window.height-2)){
					char tmp_ch;
					mvwprintw(window.win, window.height-1, window.startx, "Page End. Pres \"n\" to go to next page.");
					noecho();
					while((tmp_ch = wgetch(window.win)) != 'n');
					wclear(window.win);
					wrefresh(window.win);
					box(window.win, window.starty, window.startx);
					window.pointer_y = window.starty+1;
					window.pointer_x = window.startx+1;
					echo();
				}
				if(ch == '\n'){
					window.pointer_y++;
					window.pointer_x = 1;
					wmove(window.win, window.pointer_y, window.pointer_x);
					// if next is also a newline then stop
					if((ch = fgetc(gdb_file)) == '\n'){
						break;
					}
					kp = 1;
					continue;
				}
				if(window.pointer_x >= (window.width-2)){
					window.pointer_y++;
					window.pointer_x = 1;
				}
				mvwaddch(window.win, window.pointer_y, window.pointer_x++, ch);
				wrefresh(window.win);
			}
			wmove(window.win, window.pointer_y++, window.pointer_x);
		}
	}

}

int main(int argc, char **argv)
{
	struct window_desc prompt_window = {.height = 0, 
					.width = 0, 
					.starty = 0, 
					.startx = 0,
					.pointer_y = 1,
					.pointer_x = 1};

	struct window_desc memExam_window = {.height = 0,
					.width = 0, 
					.starty = 0, 
					.startx = 0,
					.pointer_y = 1,
					.pointer_x = 1};
	int rows, cols;
	int return_value;

	FILE *output_file;
	FILE *gdb_file;

	char unparsed_cmd[100];
	char *path;
	char ch;

	// init screen
	initscr();
	// dont wait for newline
	cbreak();
	// turn on scrolling
	scrollok(prompt_window.win, TRUE);
	// receive special keys like arrow and fX
	keypad(stdscr, TRUE);

	/* START PROMPT WINDOW */
	getmaxyx(stdscr, rows, cols);

	prompt_window.height = rows;
	prompt_window.width = cols/2;

	prompt_window.win = newwin(prompt_window.height, prompt_window.width, prompt_window.starty, prompt_window.startx);

	box(prompt_window.win, 0, 0);
	wrefresh(prompt_window.win);
	/* FINISHED PROMPT WINDOW */

	/* START MemoryExamination WINDOW */
	memExam_window.height = rows/2;
	memExam_window.width = cols/2;

	memExam_window.startx = cols/2;

	memExam_window.win = newwin(memExam_window.height, memExam_window.width, memExam_window.starty, memExam_window.startx);

	box(memExam_window.win, 0, 0);
	wrefresh(memExam_window.win);

	memExam_window.pointer_y = 1;
	memExam_window.pointer_x = 1;

	/* FINISHED MemoryExamination WINDOW */

	freopen("tmp/tmp_err", "a+", stderr);

	// print some stuff on memExam just for fun
	if (!(gdb_file = fopen(GDB_TMP, "r"))){
		perror("Could not open GDB_TMP");
		return -1;
	}

	read_and_print_gdbTmp(gdb_file, "File .*\.[ch]:", memExam_window);

	fclose(gdb_file);

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

	delete_shell(1);

	delwin(prompt_window.win);
	endwin();

	return 0;
}

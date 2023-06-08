//TODO make shell_start better

#include <ncurses.h>
#include <regex.h>

#include "shell.h"

#define GDB_TMP "./tmp/gdb.tmp"

#define LOG_TAG "pE_application"
#define DEBUG

#define pELOG(fmt, ...) \
	fprintf(stderr, "[%s] [%s]: " fmt "\n", \
			LOG_TAG, __func__, ##__VA_ARGS__);

struct window_desc {
	int height;
	int width;
	int starty;
	int startx;
	int pointer_y;
	int pointer_x;
	WINDOW *win;
};

void print_to_window(char* message, struct window_desc *window){
	size_t length;
	char ch;

	length = strlen(message);
	for(size_t i = 0; i < length; i++)
	{	
		ch = message[i];
		if((*window).pointer_y >= ((*window).height-2)){
			char tmp_ch;
			mvwprintw((*window).win, (*window).height-1, 0, "Page End. Pres \"n\" to go to next page.");
			noecho();
			while((tmp_ch = wgetch((*window).win)) != 'n');
			wclear((*window).win);
			wrefresh((*window).win);
			box((*window).win, 0, 0);
			(*window).pointer_y = (*window).starty+1;
			(*window).pointer_x = (*window).startx+1;
			echo();
		}
		if(ch == '\n'){
			(*window).pointer_y++;
			(*window).pointer_x = 1;
			wmove((*window).win, (*window).pointer_y, (*window).pointer_x);
			continue;
		}
		if((*window).pointer_x >= ((*window).width-2)){
			(*window).pointer_y++;
			(*window).pointer_x = 1;
		}
		mvwaddch((*window).win, (*window).pointer_y, (*window).pointer_x++, ch);
		wrefresh((*window).win);
	}
}

void read_and_print_file(FILE *file, char* start_regex,\
		char* stop_regex, struct window_desc *window, int should_print_start, int should_print_stop){
	regex_t fname_start_regex;
	regex_t fname_stop_regex;
	char buffer[1000]; //no line should be 1000 chars
	char ch;
	int kp = 0;

	regcomp(&fname_start_regex, start_regex, REG_EXTENDED);
	regcomp(&fname_stop_regex, stop_regex, REG_EXTENDED);

	pELOG("Started with start_regex: %s", start_regex);
	pELOG("Started with stop_regex: %s", stop_regex);

	int length;

	while(fgets(buffer, 1000, file)){
		if(!regexec(&fname_start_regex, buffer, 0, NULL, 0)){
			if(!should_print_start){
				fgets(buffer, 1000, file);
			}
			do
			{
				print_to_window(buffer, window);

			}while(fgets(buffer, 1000, file) && regexec(&fname_stop_regex, buffer, 0, NULL, 0));
			if(should_print_stop)
			{
				print_to_window(buffer, window);
			}
			wmove((*window).win, (*window).pointer_y++, (*window).pointer_x);
		}
	}

	clearerr(file);
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

	stderr = freopen("./tmp/tmp_err", "w", stderr);
	// set stderr buffering to line buffering instead of block
	setvbuf(stderr, NULL, _IOLBF, 0);

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

	// print some stuff on memExam just for fun
	if (!(gdb_file = fopen(GDB_TMP, "r"))){
		perror("Could not open GDB_TMP");
		return -1;
	}

	read_and_print_file(gdb_file, "File .*\\.[ch]:", "^\\s*$", &memExam_window, 1, 0);

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
		read_and_print_file(gdb_file, "\\+p unparsed_cmd", "\\+continue", &memExam_window, 0, 0);
		output_file = fopen(path, "rw");
		read_and_print_file(output_file, ".*", "^$", &prompt_window, 1, 0);
		fclose(output_file);
	}while(strcmp(unparsed_cmd, "exit"));

	delete_shell(0);

	fclose(gdb_file);

	delwin(prompt_window.win);
	endwin();

	return 0;
}

//TODO make shell_start better
//TODO make print_to_window print entire word (should i do this?)

#include <ncurses.h>
#include <regex.h>
#include <unistd.h>
#include <locale.h>

#include "shell.h"
#include "pE_application.h"

#define GDB_TMP "./tmp/gdb.tmp"
#define GDB_TMP_2 "./tmp/gdb.tmp2"
// TODO make this generic
#define TUTORIALS_PATH "./bin/tutorials/tutorial.1.txt"
#define STACK_PATH "./bin/tutorials/stack_file"
#define INTRO_PATH "./bin/tutorials/intro"

#define LOG_TAG "pE_application"
#define DEBUG

#define CMD_MAX_SIZE 1024
// this leaves space for tutorial.<n> size to range n = {1..99999999999999}
#define PATH_MAX_SIZE CMD_MAX_SIZE-24
#define TUTORIAL_MAX_NUM 15
#define BUFFER_MAX_SIZE 1024

void gdb_run(char *str) {}

void start_pE(struct window_desc *tutorial_window)
{
	// show some info about pE
	FILE* intro_file;
	char buffer[BUFFER_MAX_SIZE];

	char tmp_ch;

	intro_file = fopen(INTRO_PATH, "r");

	wclear((*tutorial_window).win);
	wrefresh((*tutorial_window).win);
	box((*tutorial_window).win, 0, 0);
	(*tutorial_window).pointer_y = 1;
	(*tutorial_window).pointer_x = 1;

	print_to_window("Welcome to penguinExplore! Do you wish to read the intro?(y/n) ", tutorial_window);
	wgetnstr((*tutorial_window).win, buffer, BUFFER_MAX_SIZE);

	if(buffer[0] != 'y')
		return;

	while(fgets(buffer, BUFFER_MAX_SIZE, intro_file))
	{
		wclear((*tutorial_window).win);
		wrefresh((*tutorial_window).win);
		box((*tutorial_window).win, 0, 0);
		(*tutorial_window).pointer_y = 1;
		(*tutorial_window).pointer_x = 1;

		print_to_window(buffer, tutorial_window);

		mvwprintw((*tutorial_window).win, (*tutorial_window).height-1, 0, "Press \"n\" continue.");
		noecho();
		while((tmp_ch = wgetch((*tutorial_window).win)) != 'n');
		echo();
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

	struct window_desc tutorial_window = {.height = 0,
					.width = 0, 
					.starty = 0, 
					.startx = 0,
					.pointer_y = 1,
					.pointer_x = 1};

	struct window_desc gdb_window = {.height = 0,
					.width = 0, 
					.starty = 0, 
					.startx = 0,
					.pointer_y = 1,
					.pointer_x = 1};

	int rows, cols;
	int return_value;

	FILE *output_file;
	FILE *gdb_file;

	char unparsed_cmd[CMD_MAX_SIZE];
	char tmp_str[CMD_MAX_SIZE];
	char tutorial_num[TUTORIAL_MAX_NUM];
	char pwd[PATH_MAX_SIZE];
	char *path;

	//struct stat st = {0};

	// create tmp files folder if it doesn't exist
	// no node to bother checking for existance, will only log an error
	mkdir("tmp", 0700);

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

	// TODO make this "-6" more generic using DEFINEs
	prompt_window.height = rows/3;
	prompt_window.width = cols/2;

	prompt_window.win = newwin(prompt_window.height, prompt_window.width, prompt_window.starty, prompt_window.startx);

	box(prompt_window.win, 0, 0);
	wrefresh(prompt_window.win);
	/* FINISHED PROMPT WINDOW */

	/* START TutorialWindow WINDOW */
	tutorial_window.height = rows/3;
	tutorial_window.width = cols/2;

	tutorial_window.startx = 0;
	tutorial_window.starty = rows/3;

	tutorial_window.win = newwin(tutorial_window.height, tutorial_window.width, tutorial_window.starty, tutorial_window.startx);

	box(tutorial_window.win, 0, 0);
	wrefresh(tutorial_window.win);

	tutorial_window.pointer_y = 1;
	tutorial_window.pointer_x = 1;

	/* FINISHED TutorialWindow WINDOW */

	/* START MemoryExamination WINDOW */
	memExam_window.height = rows;
	memExam_window.width = cols/2;

	memExam_window.startx = cols/2;

	memExam_window.win = newwin(memExam_window.height, memExam_window.width, memExam_window.starty, memExam_window.startx);

	box(memExam_window.win, 0, 0);
	wrefresh(memExam_window.win);

	memExam_window.pointer_y = 1;
	memExam_window.pointer_x = 1;

	/* FINISHED MemoryExamination WINDOW */

	/* START GDB WINDOW */
	gdb_window.height = (rows/3)+1;
	gdb_window.width = cols/2;

	gdb_window.startx = 0;
	gdb_window.starty = (2*rows)/3;

	gdb_window.win = newwin(gdb_window.height, gdb_window.width, gdb_window.starty, gdb_window.startx);

	box(gdb_window.win, 0, 0);
	wrefresh(gdb_window.win);

	gdb_window.pointer_y = 1;
	gdb_window.pointer_x = 1;

	/* FINISHED GDB WINDOW */

	start_pE(&tutorial_window);

	// print some stuff on memExam just for fun
	if (!(gdb_file = fopen(GDB_TMP, "r"))){
		perror("Could not open GDB_TMP");
		//return -1;
	}else{
		read_and_print_file(gdb_file, "File .*\\.[ch]:", "^\\s*$", &memExam_window, 1, 0, REG_EXTENDED);
	}

	path = shell_start();

	do
	{
		mvwprintw(prompt_window.win, prompt_window.pointer_y++, prompt_window.startx+1, "prompt > ");
		wrefresh(prompt_window.win);
		wgetnstr(prompt_window.win, unparsed_cmd, CMD_MAX_SIZE);

		// If command is some custom command or some problematic string,
		// check and treat it according
		if((int)strlen(unparsed_cmd) == 0){ // if it's an empty line, i.e. just \n
			// if it's the last line we need to clear the window
			// can't use print_to_window because not printing anything,
			// so hardcode it
			if(prompt_window.pointer_y >= (prompt_window.height-2)){
				char tmp_ch;
				mvwprintw(prompt_window.win, prompt_window.height-1, 0, "Page End. Press \"n\" to go to next page.");
				noecho();
				while((tmp_ch = wgetch(prompt_window.win)) != 'n');
				wclear(prompt_window.win);
				wrefresh(prompt_window.win);
				box(prompt_window.win, 0, 0);
				prompt_window.pointer_y = 1;
				prompt_window.pointer_x = 1;
				echo();
			}
			continue;
		}
		if(!strcmp(strtok(unparsed_cmd," "), "tutorial")){ // if it's a tutorial command
			if(!strcmp(strtok(NULL," "), "-n")){

				getcwd(pwd, PATH_MAX_SIZE);
				strcpy(tutorial_num, strtok(NULL," "));

				// set unparsed_cmd to binary path
				strcpy(unparsed_cmd, pwd);
				strcat(unparsed_cmd, "/bin/tutorials/tutorial.");
				strcat(unparsed_cmd, tutorial_num);

				// create gdb --batch command string and run it first
				// gdb --batch --cd=<PWD>/tutorials --command=<PWD>/tutorials/tutorial.<n>.gdb <PWD>/bin/tutorials/tutorial.<n>
				// gdb --batch --cd=
				strcpy(tmp_str, "gdb --batch --cd=");

				// gdb --batch --cd=<PWD>/tutorials --command=
				strcat(tmp_str, pwd);
				strcat(tmp_str, "/tutorials --command=");

				// gdb --batch --cd=<PWD>/tutorials --command=<PWD>/tutorials/tutorial.<n>.gdb <PWD>/bin/tutorials/tutorial.<n>
				strcat(tmp_str, pwd);
				strcat(tmp_str, "/tutorials/tutorial.");
				strcat(tmp_str, tutorial_num);
				strcat(tmp_str, ".gdb ");
				strcat(tmp_str, unparsed_cmd);

				pELOG("unparsed_cmd: %s", unparsed_cmd);
				pELOG("gdb_cmd: %s", tmp_str);

				return_value = run_shell(tmp_str);

				if(return_value) {
					pELOG("run_shell return: %d", return_value);
					continue;
				}

				return_value = run_shell(unparsed_cmd);

				// TODO make tutorial_number dynamic
				print_tutorial(1, &tutorial_window, &memExam_window, &gdb_window);
			}
			else{
				print_to_window("Run tutorial but couldn't get option!\n", &prompt_window);
				print_to_window("Tutorial usage is: tutorial -n <tutorial_num>\n", &prompt_window);
			}
			continue;
		}
		// if not, just run the command
		return_value = run_shell(unparsed_cmd);

		if(return_value == -1){
			mvwprintw(prompt_window.win, prompt_window.pointer_y++, prompt_window.startx+1, "Command failed. Are you sure it exists?");
			continue;
		}
		//read_and_print_file(gdb_file, "\\+p unparsed_cmd", "\\+continue", &memExam_window, 0, 0, REG_EXTENDED);
		output_file = fopen(path, "rw");
		read_and_print_file(output_file, ".*", "^$", &prompt_window, 1, 0, REG_EXTENDED);
		fclose(output_file);
	}while(strcmp(unparsed_cmd, "exit"));

	delete_shell(0);

	fclose(gdb_file);

	delwin(prompt_window.win);
	endwin();

	return 0;
}

void print_to_window(char* message, struct window_desc *window){
	size_t length;
	char ch;

	pELOG("Started!");

	length = strlen(message);
	for(size_t i = 0; i < length; i++)
	{	
		ch = message[i];
		if((*window).pointer_y >= ((*window).height-2)){
			char tmp_ch;
			mvwprintw((*window).win, (*window).height-1, 0, "Page End. Press \"n\" to go to next page.");
			noecho();
			while((tmp_ch = wgetch((*window).win)) != 'n');
			wclear((*window).win);
			wrefresh((*window).win);
			box((*window).win, 0, 0);
			(*window).pointer_y = 1;
			(*window).pointer_x = 1;
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

	pELOG("Leaving!");
}

void read_and_print_file(FILE *file, char* start_regex,\
		char* stop_regex, struct window_desc *window, int should_print_start, int should_print_stop,\
		int cflags){
	regex_t fname_start_regex;
	regex_t fname_stop_regex;
	char buffer[BUFFER_MAX_SIZE]; //no line should be BUFFER_MAX_SIZE chars

	regcomp(&fname_start_regex, start_regex, cflags);
	regcomp(&fname_stop_regex, stop_regex, cflags);

	pELOG("Started with start_regex: %s", start_regex);
	pELOG("Started with stop_regex: %s", stop_regex);

	while(fgets(buffer, BUFFER_MAX_SIZE, file)){
		if(!regexec(&fname_start_regex, buffer, 0, NULL, 0)){
			if(!should_print_start){
				fgets(buffer, BUFFER_MAX_SIZE, file);
			}
			do
			{
				print_to_window(buffer, window);

			}while(fgets(buffer, BUFFER_MAX_SIZE, file) && regexec(&fname_stop_regex, buffer, 0, NULL, 0));
			if(should_print_stop)
			{
				print_to_window(buffer, window);
			}
			wmove((*window).win, (*window).pointer_y++, (*window).pointer_x);
			break;
		}
	}

	// caller should clear err/EOF if desired
	// clearerr(file);
}

int write_stack_file(FILE* file, char* line)
{
	char stack_addr[BUFFER_MAX_SIZE];;
	char buffer[BUFFER_MAX_SIZE];

	int rvalue = 0;

	FILE* tmp;

	regex_t stack_addr_regex;

	pELOG("Started!");

	tmp = tmpfile();

	strcpy(stack_addr, line);

	for(int i = 0; i < strlen(stack_addr); i++)
	{
		if(stack_addr[i] == ':')
		{
			stack_addr[i] = '\0';
			break;
		}
	}

	pELOG("stack_addr: %s", stack_addr);

	regcomp(&stack_addr_regex, stack_addr, 0);

	while(fgets(buffer, BUFFER_MAX_SIZE, file)){
		//if(0)
		if(!regexec(&stack_addr_regex, buffer, 0, NULL, 0))
		{
			pELOG("Matched %s!", buffer);
			rvalue = 1;
			fputs(line, tmp);
		}else
		{
			pELOG("Not matched %s!", buffer);
			fputs(buffer, tmp);
		}
	}

	rewind(tmp);
	rewind(file);
	ftruncate(fileno(file), 0L);
	while(fgets(buffer, BUFFER_MAX_SIZE, tmp))
	{
		pELOG("Putting %s", buffer);
		fputs(buffer, file);
	}

	fclose(tmp);

	return rvalue;
}

void print_stack(struct window_desc* stack_window, FILE* stack_file)
{
	int is_last = 0;

	char buffer[BUFFER_MAX_SIZE];
	char* print_buffer;
	char stack_addr_buffer[BUFFER_MAX_SIZE];

	// this holds the stack
	// argument stack_file is gdb output
	FILE* stack_main_file;

	regex_t fname_start_regex;
	regex_t fname_stop_regex;
	regex_t stack_addr_regex;

	pELOG("Started!");

	wclear((*stack_window).win);
	box((*stack_window).win, 0, 0);
	wrefresh((*stack_window).win);

	(*stack_window).pointer_y = 1;
	(*stack_window).pointer_x = 1;

	stack_main_file = fopen(STACK_PATH, "a+");
	rewind(stack_main_file);

	regcomp(&fname_start_regex, "Begin_stack_change:", REG_EXTENDED);
	regcomp(&fname_stop_regex, "End_stack_change:", REG_EXTENDED);

	while(fgets(buffer, BUFFER_MAX_SIZE, stack_file)){
		if(!regexec(&fname_start_regex, buffer, 0, NULL, 0)){
			while(fgets(buffer, BUFFER_MAX_SIZE, stack_file) && regexec(&fname_stop_regex, buffer, 0, NULL, 0))
			{
				strcpy(stack_addr_buffer, buffer);
				if(!write_stack_file(stack_main_file, buffer))
				{
					pELOG("Putting %s", buffer);
					fputs(buffer, stack_main_file);
				}
				rewind(stack_main_file);
			}
			//wmove((*stack_window).win, (*stack_window).pointer_y++, (*stack_window).pointer_x);
			break;
		}
	}

	// only write until end of last stack, so stack can decrease in size

	for(int i = 0; i < strlen(stack_addr_buffer); i++)
	{
		if(stack_addr_buffer[i] == ':')
		{
			stack_addr_buffer[i] = '\0';
			break;
		}
	}

	pELOG("stack_addr: %s", stack_addr_buffer);

	regcomp(&stack_addr_regex, stack_addr_buffer, 0);

	is_last = 0;

	while(fgets(buffer, BUFFER_MAX_SIZE, stack_main_file))
	{
		if(is_last)
			break;
		if(!regexec(&stack_addr_regex, buffer, 0, NULL, 0))
			is_last = 1;

		// all lines should be equalsize so there's no problem in printing aligned to center string
		print_buffer = buffer;
		// 14 is always the size of the address
		print_buffer[14]='\0';
		print_to_window(print_buffer, stack_window);

		// TODO maybe align better the prints?
		wvline((*stack_window).win, ACS_VLINE, 1);

		print_buffer = &(buffer[15]);
		(*stack_window).pointer_x = (((*stack_window).width-strlen(print_buffer))/2);
		wmove((*stack_window).win, (*stack_window).pointer_y, (*stack_window).pointer_x);
		print_to_window(print_buffer, stack_window);

		wmove((*stack_window).win, (*stack_window).pointer_y++, (*stack_window).pointer_x);
		whline((*stack_window).win, ACS_HLINE, (*stack_window).width-2);
		wrefresh((*stack_window).win);
	}

	fclose(stack_main_file);

	pELOG("Leaving!");
}

int exec_tutorial_command(struct window_desc *tutorial_window, FILE* tutorial_file, char* buffer_command)
{
	char command[CMD_MAX_SIZE];
	char start_buffer[BUFFER_MAX_SIZE];
	char end_buffer[BUFFER_MAX_SIZE];

	char *tmp_str;

	long file_position;

	regex_t regex;

	pELOG("Started!");

	strcpy(start_buffer, "Start ");

	strcpy(end_buffer, "End ");

	wclear((*tutorial_window).win);
	wrefresh((*tutorial_window).win);
	box((*tutorial_window).win, 0, 0);

	(*tutorial_window).pointer_y = 1;
	(*tutorial_window).pointer_x = 1;

	// replace \n so we can compare with command
	buffer_command[strcspn(buffer_command, "\n")] = '\0';

	do{

		if((*tutorial_window).pointer_y >= ((*tutorial_window).height-2)){
			char tmp_ch;
			mvwprintw((*tutorial_window).win, (*tutorial_window).height-1, 0, "Page End. Press \"n\" to go to next page.");
			noecho();
			while((tmp_ch = wgetch((*tutorial_window).win)) != 'n');
			wclear((*tutorial_window).win);
			wrefresh((*tutorial_window).win);
			box((*tutorial_window).win, 0, 0);
			(*tutorial_window).pointer_y = 1;
			(*tutorial_window).pointer_x = 1;
			echo();
		}

		mvwprintw((*tutorial_window).win, (*tutorial_window).pointer_y, (*tutorial_window).pointer_x, "command > ");

		wgetnstr((*tutorial_window).win, command, CMD_MAX_SIZE);

		// an empty command could cause some troubles, so just do nothing
		if(strlen(command) == 0){
			(*tutorial_window).pointer_y++;
			continue;
		}
		// if it's just command, tutorial creator doesn't want us to check, so
		// first check for special commands, then go ahead
		if(!strcmp(buffer_command, "command")){
			// let's treat some special commands, like to reprint code or to leave tutorial
			if(!strcmp(command, "help")){
				(*tutorial_window).pointer_y++;
				print_to_window("Built-in commands for 'Command' window:\n", tutorial_window);
				print_to_window("help: Display this help!\n", tutorial_window);
				print_to_window("cancel: Go back to tutorial.\n", tutorial_window);
				print_to_window("exit: leave tutorial and go back to shell.\n", tutorial_window);
				print_to_window("pcode: show code again.\n", tutorial_window);
				return 0;
			}
			if(!strcmp(command, "cancel")){
				return 0;
			}
			if(!strcmp(command, "exit")){
				return EXIT_CMD;
			}
			else if(!strcmp(command, "pcode")){
				FILE* tfile;
				tfile = fopen(GDB_TMP_2, "r");
				print_code(NULL, tfile);
				return 0;
			}
			break;
		}
		// else, this came from a tutorial and developer wants us to check
		// if user typed correctly
		else if(strcmp(command, buffer_command)){
			(*tutorial_window).pointer_y++;
			print_to_window("Typed command do not match requested. Please try again.\n", tutorial_window);
		}else{
			break;
		}
	}while(true);

	(*tutorial_window).pointer_y++;
	(*tutorial_window).pointer_x = 1;
	wmove((*tutorial_window).win, (*tutorial_window).pointer_y, (*tutorial_window).pointer_x);

	pELOG("Checking for special chars!");

	if((tmp_str = strchr(command, '*')))
	{
		for(int i = strlen(tmp_str)+1; i > 0; i--)
		{
			*(tmp_str+i) = *(tmp_str+i-1);
		}
		*(tmp_str) = '\\';
	}
	if((tmp_str = strchr(command, ',')))
	{
		for(int i = strlen(tmp_str)+1; i > 0; i--)
		{
			*(tmp_str+i) = *(tmp_str+i-1);
		}
		*(tmp_str) = '\\';
	}

	pELOG("Finished checking for special chars!");

	strcat(start_buffer, command);
	strcat(end_buffer, command);

	file_position = ftell(tutorial_file);

	regcomp(&regex, start_buffer , 0);

	while(fgets(command, BUFFER_MAX_SIZE, tutorial_file)){
		if(!regexec(&regex, command, 0, NULL, 0))
		{
			fseek(tutorial_file, file_position, SEEK_SET);
			break;
		}
	}

	// TODO a better way to get commands than rewind (there might be repeated commands)
	rewind(tutorial_file);

	read_and_print_file(tutorial_file, start_buffer, end_buffer, tutorial_window, 0, 0, 0);

	fseek(tutorial_file, file_position, SEEK_SET);

	return 0;
}

void print_code(FILE* tutorial_file, FILE* code_file)
{
	FILE* screen_tmp;
	
	int rows, cols;

	char buffer[BUFFER_MAX_SIZE];

	char proclnk[BUFFER_MAX_SIZE];
	char tmp_file[BUFFER_MAX_SIZE];
	int tmp_fileno;
	ssize_t r;

	char tmp_ch;

	struct window_desc code_window = {.height = 0,
					.width = 0, 
					.starty = 0, 
					.startx = 0,
					.pointer_y = 1,
					.pointer_x = 1};

	struct window_desc comment_window = {.height = 0,
					.width = 0, 
					.starty = 0, 
					.startx = 0,
					.pointer_y = 1,
					.pointer_x = 1};

	screen_tmp = tmpfile();
	tmp_fileno = fileno(screen_tmp);
	sprintf(proclnk, "/proc/self/fd/%d", tmp_fileno);
	r = readlink(proclnk, tmp_file, BUFFER_MAX_SIZE);
	tmp_file[r] = '\0';

	scr_dump(tmp_file);

	getmaxyx(stdscr, rows, cols);

	/* START CODE WINDOW */

	code_window.height = rows-10;
	code_window.width = cols;

	code_window.win = newwin(code_window.height, code_window.width, code_window.starty, code_window.startx);

	box(code_window.win, 0, 0);
	wrefresh(code_window.win);

	code_window.pointer_y = 1;
	code_window.pointer_x = 1;
	/* FINISHED CODE WINDOW */

	/* START COMMENT WINDOW */
	comment_window.height = 10;
	comment_window.width = cols;

	comment_window.starty = rows-10;

	comment_window.win = newwin(comment_window.height, comment_window.width, comment_window.starty, comment_window.startx);

	box(comment_window.win, 0, 0);
	wrefresh(comment_window.win);

	comment_window.pointer_y = 1;
	comment_window.pointer_x = 1;

	/* FINISHED TutorialWindow WINDOW */

	read_and_print_file(code_file, "Start pcode", "End pcode", &code_window, 0, 0, 0);

	if(tutorial_file)
	{
		while(fgets(buffer, BUFFER_MAX_SIZE, tutorial_file) && strcmp(strtok(strdup(buffer), " \n"), "pcode"))
		{
			// clear window everytime we'll print new line of comment
			wclear(comment_window.win);
			wrefresh(comment_window.win);
			box(comment_window.win, 0, 0);
			comment_window.pointer_y = 1;
			comment_window.pointer_x = 1;

			print_to_window(buffer, &comment_window);

			mvwprintw(comment_window.win, comment_window.height-1, 0, "Press \"n\" to continue.");
			noecho();
			while((tmp_ch = wgetch(comment_window.win)) != 'n');
			echo();
		}
	}else{
		mvwprintw(comment_window.win, comment_window.height-1, 0, "Press \"n\" to continue.");
		noecho();
		while((tmp_ch = wgetch(comment_window.win)) != 'n');
		echo();
	}

	rewind(code_file);

	scr_restore(tmp_file);
	doupdate();

	fclose(screen_tmp);
}

int parse_buffer(char* buffer)
{
	char tmp_buffer[BUFFER_MAX_SIZE];
	char *token;

	strcpy(tmp_buffer, buffer);

	token = strtok(tmp_buffer, " \n");
	pELOG("token: %s", token);
	if(!strcmp(token, "pstack"))
		return PSTACK;
	else if(!strcmp(token, "command"))
		return COMMAND;
	else if (!strcmp(token, "pcode"))
		return PCODE;
	else if(!strcmp(token, "EXIT"))
		return EXIT_CMD;
	else
		return PTUTORIAL;

	return -1;
}

void print_tutorial(int tutorial_number, \
		struct window_desc *tutorial_window, struct window_desc *stack_window, struct window_desc *command_window)
{
	FILE *tutorials_txt;
	FILE *gdb_file;
	char buffer[BUFFER_MAX_SIZE];
	char tmp_ch;

	enum tutorial_command_t command;

	pELOG("Started!");

	tutorials_txt = fopen(TUTORIALS_PATH, "r");
	gdb_file = fopen(GDB_TMP_2, "r");

	wclear((*stack_window).win);
	box((*stack_window).win, 0, 0);
	wrefresh((*stack_window).win);

	(*stack_window).pointer_y = 1;
	(*stack_window).pointer_x = 1;

	// TODO fix this gambiarra
	FILE *aaa;
	aaa = fopen(STACK_PATH, "w+");
	fclose(aaa);

	while(fgets(buffer, BUFFER_MAX_SIZE, tutorials_txt))
	{
		pELOG("buffer: %s", buffer);

		command = parse_buffer(buffer);
		pELOG("command: %d", command);

		switch(command)
		{
			case PSTACK:
				print_stack(stack_window, gdb_file);
				break;

			case COMMAND:
				if(EXIT_CMD == exec_tutorial_command(command_window, gdb_file, buffer))
					goto EXIT;
				break;
			case PCODE:
				print_code(tutorials_txt, gdb_file);
				break;

			case PTUTORIAL:
				// clear window everytime we'll print new line of tutorial
				wclear((*tutorial_window).win);
				wrefresh((*tutorial_window).win);
				box((*tutorial_window).win, 0, 0);
				(*tutorial_window).pointer_y = 1;
				(*tutorial_window).pointer_x = 1;

				print_to_window(buffer, tutorial_window);

				mvwprintw((*tutorial_window).win, (*tutorial_window).height-1, 0, "Press \"n\" continue.");
				noecho();
				while((tmp_ch = wgetch((*tutorial_window).win)) != 'n')
				{
					pELOG("tmp_ch = %c", tmp_ch);
					// if we are waiting after tutorial was printed
					// user can type 'q' and then 'c' to type a command
					// if user types command "exit", exit from tutorial mode
					if(tmp_ch == 'q')
					{
						if((tmp_ch = wgetch((*tutorial_window).win)) == 'c')
						{
							echo();
							strcpy(buffer, "command");
							if(EXIT_CMD == exec_tutorial_command(command_window, gdb_file, buffer))
							{
								goto EXIT;
							}
							mvwprintw((*tutorial_window).win, (*tutorial_window).height-1, 0, "Press \"n\" continue.");
							noecho();
						}
					}
				}
				echo();
				break;
			default:
				pELOG("Error reading tutorial file. Exiting.");
				goto EXIT;
		}
	}

EXIT:
	fclose(tutorials_txt);
	fclose(gdb_file);

	pELOG("Leaving!");

}

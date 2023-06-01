#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

struct shell_struct {
	FILE *tmp_out;
	FILE *tmp_err;
	FILE *tmp_in;
	int err;
};

struct shell_struct start_shell();
int delete_shell(struct shell_struct my_shell, int keep_history);
int run_shell(char *unparsed_cmd);

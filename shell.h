#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

int run_shell(char *unparsed_cmd);
int delete_shell(int keep_history);
char *shell_start();

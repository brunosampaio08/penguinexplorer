/* Credit:
 * 	Bruno Sampaio Leite @brunosampaio08
 *	Talita Ludmila de Lima @ludmilalima
 */


#include "shell.h"
#include "pE_application.h"

#define TMP_DIR "./tmp"
#define TMP_IN TMP_DIR"/tmp_in"
#define TMP_OUT TMP_DIR"/tmp_out"
#define TMP_ERR TMP_DIR"/tmp_err"

char ***createCMDMatrix(int argc, char **argv, int *cmd_num, int *input_from, int *output_to){
	
	//Matriz de comandos para ser retornada
	char ***cmd;
	//Contador de posicoes da matriz
	int k = 0;

	//Numero de comandos e no minimo 1
	*cmd_num = 1;

	//Aloca memoria para a matriz
	cmd = malloc(sizeof(char **));

	//Matriz de comandos comeca apontando para todo o argv
	cmd[k] = &argv[0];
	k++;

	//Valor arbitrario para caso os IOs sejam o padrao
	*input_from = -1;
	*output_to = -1;

	//Para todas as posicoes de argv a partir do primeiro comando
	for(int i = 0; i < argc; i++){
		//Se achou um pipe
		if(strcmp(argv[i], "|") == 0){
			//Matriz de comandos recebe o proximo comando
			cmd[k] = &argv[i+1];
			//Pipe recebe NULL para que execvp pare
			argv[i] = NULL;
			k++;
			(*cmd_num)++;
		}else if(strcmp(argv[i], "<") == 0){
			//Achou um operador de input de arquivo
			char *filename;
			//filename recebe o nome do arquivo para ler
			filename = &argv[i+1][0];
			//Nome do arquivo e operador recebem NULL
			argv[i] = NULL;
			argv[i+1] = NULL;
			//Pula a posicao de argv que contem o nome do arquivo
			i++;
			//Abre um FD para ler o input
			*input_from = open(filename, O_RDONLY);
			if(*input_from == -1){
				perror("open()");
			}
		}else if(strcmp(argv[i], ">") == 0){
			//Achou um operador de output para arquivo truncado
			char *filename;
			filename = &argv[i+1][0];
			argv[i] = NULL;
			argv[i+1] = NULL;
			i++;
			//Abre um FD de escrita com permissoes de user e grupo
			*output_to = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 
					S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
			if(*output_to == -1){
				perror("open()");
			}
		}else if(strcmp(argv[i], ">>") == 0){
			//Achou um operador de output para arquivo concatenado
			char *filename;
			filename = &argv[i+1][0];
			argv[i] = NULL;
			argv[i+1] = NULL;
			i++;
			*output_to = open(filename, O_WRONLY | O_CREAT | O_APPEND,
				      	S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
			if(*output_to == -1){
				perror("open()");
			}
		}

	}
	//Se nao achou operadores de input ou output usa os arquivos padrao
	//Ncurses vai ler desses arquivos
	if(*input_from == -1){
		*input_from = STDIN_FILENO;
	}
	if(*output_to == -1){
		*output_to = open(TMP_OUT, O_WRONLY | O_CREAT | O_TRUNC, 
					S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
		if(*output_to == -1){
			perror("open()");
		}
	}

	//Retorna a matri de comandos
	return cmd;
}

int exeCMD(char ***cmd, int cmd_num, int input_from, int output_to){
	int fd[cmd_num-1][2]; //Cria um pipe pra cada comando-1;
	int i = 0;
	pid_t pid;

	for(i = 0; i < cmd_num; i++){
		if(i < cmd_num-1){//Do primeiro ao penultimo comando
			pipe(fd[i]);
			if(pipe < 0){
				perror("pipe()");
				exit(-1);
			}
		}

		pid = fork();
		
		if(pid == 0){//Se e o filho
			if(i < cmd_num-1){ //Do primeiro ao penultimo comando
				dup2(fd[i][1], STDOUT_FILENO);//Escreve no pipe
				close(fd[i][1]);
				close(fd[i][0]);
			}
			if(i > 0){//Do segundo ao ultimo comando
				dup2(fd[i-1][0], STDIN_FILENO);
				close(fd[i-1][0]);
				close(fd[i-1][1]);
			}
			
			if(i == 0 && input_from != 0){//Se e o primeiro filho
				//Escolhe de onde veio o input
				dup2(input_from, STDIN_FILENO);
			}
			if(i == cmd_num-1 && output_to != 1){
				//Escolhe pra onde vai o output
				dup2(output_to, STDOUT_FILENO);
			}

			execvp(cmd[i][0], cmd[i]);
			exit(-1);
		}else if(pid > 0){
			int status;
			if(i > 0){//Existe comando anterior
				//Fecha os pipes dos comandos anteriores
				close(fd[i-1][0]);
				close(fd[i-1][1]);
			}
			waitpid(-1, &status, 0);
			if(WIFEXITED(status)){
				if(WEXITSTATUS(status)){
					return -1;
				}
			}
		}
	}

	return 0;
}

char *readinput(FILE *file){
#define BATCH 50
	char *input = NULL;
	char tmp_input[BATCH];

	size_t tmp_len, input_len;
	tmp_len = 0;
	input_len = 0;

	do
	{
		if(!fgets(tmp_input, BATCH, file)){
			return NULL;	
		}
		tmp_len = strlen(tmp_input);
		input = realloc(input, tmp_len+input_len+1);
		strcpy(input+input_len, tmp_input);
		input_len += tmp_len;
	}while(tmp_len==BATCH-1 && tmp_input[BATCH-2]!='\n');

	return input;	
}

char **parse_cmd(char *cmd, int *arg_num)
{
	char **parsed_cmd = NULL;
	const char *delimiter = "\t \n";
	char *token;

	*arg_num = 0;
	token = strtok(cmd, delimiter);

	while(token != NULL)
	{
		parsed_cmd = realloc(parsed_cmd,((*arg_num)+1)*(sizeof(char *)));
		parsed_cmd[*arg_num] = strdup(token);
		token = strtok(NULL, delimiter);
		(*arg_num)++;
	}

	parsed_cmd = realloc(parsed_cmd,((*arg_num)+1)*(sizeof(char *)));
	parsed_cmd[(*arg_num)] = NULL;

	return parsed_cmd;
}

int run_shell(char *unparsed_cmd){
	char ***cmd;
	int cmd_num, input_from, output_to;
	
	char **parsed_cmd = NULL;
	int arg_num;

	int return_value = 0;

	struct stat st = {0};

	if (stat(TMP_DIR, &st) == -1) {
	    mkdir(TMP_DIR, 0700);
	}

	parsed_cmd = parse_cmd(unparsed_cmd, &arg_num);

	cmd = createCMDMatrix(arg_num, parsed_cmd, &cmd_num, &input_from, &output_to);
	return_value = exeCMD(cmd, cmd_num, input_from, output_to);

	// exec failed
	if(return_value == -1){
		return -1;
	}

	if(input_from != STDIN_FILENO)
	{
		close(input_from);
	}
	// output_fd should always be closed by caller
	close(output_to);

	return 0;
}

char *shell_start()
{
	return TMP_OUT;
}

int delete_shell(int keep_history){
	if(keep_history){
		if(access(TMP_OUT, F_OK) == 0){
			remove(TMP_OUT);
		}
		if(access(TMP_ERR, F_OK) == 0){
			remove(TMP_ERR);
		}
		if(access(TMP_IN, F_OK) == 0){
			remove(TMP_IN);
		}
		if(access(TMP_DIR, F_OK) == 0){
			rmdir(TMP_DIR);
		}
	}

	return 0;
}

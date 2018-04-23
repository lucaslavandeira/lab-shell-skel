#include "exec.h"
#include "parsing.h"

#define FILE_PERMISSIONS 0664


void handle_exec(struct cmd* cmd);
void handle_back(struct cmd* cmd);

void handle_redir(struct cmd* cmd);
// sets the "key" argument with the key part of
// the "arg" argument and null-terminates it
static void get_environ_key(char* arg, char* key) {

	int i;
	for (i = 0; arg[i] != '='; i++)
		key[i] = arg[i];

	key[i] = END_STRING;
}

// sets the "value" argument with the value part of
// the "arg" argument and null-terminates it
static void get_environ_value(char* arg, char* value, int idx) {

	int i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
		value[j] = arg[i];

	value[j] = END_STRING;
}

// sets the environment variables passed
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void set_environ_vars(char** eargv, int eargc) {

	// Your code here
} 

// opens the file in which the stdin/stdout or
// stderr flow will be redirected, and returns
// the file descriptor
// 
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
static int open_redir_fd(char* file) {

	// Your code here
	return -1;
}

// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
void exec_cmd(struct cmd* cmd) {

	switch (cmd->type) {

		case EXEC:
			// spawns a command
			//
			handle_exec(cmd);
			_exit(-1);
			break;

		case BACK: {
			// runs a command in background
			//
            handle_back(cmd);
			_exit(-1);
			break;
		}

		case REDIR: {
			// changes the input/output/stderr flow
			//
            handle_redir(cmd);
			_exit(-1);
			break;
		}
		
		case PIPE: {
			// pipes two commands
			//
			// Your code here
			printf("Pipes are not yet implemented\n");
				
			// free the memory allocated
			// for the pipe tree structure
			free_command(parsed_pipe);

			break;
		}
	}
}

void handle_exec(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;
    execvp(execcmd->argv[0], execcmd->argv);
}


void handle_back(struct cmd* cmd) {
    struct backcmd* back = (struct backcmd*) cmd;
    struct execcmd* execcmd = (struct execcmd*) back->c;
    execvp(execcmd->argv[0], execcmd->argv);
}


void handle_redir(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;

	int in_fd = STDIN_FILENO;
	int out_fd = STDOUT_FILENO;
	int err_fd = STDERR_FILENO;
	if (strncmp(execcmd->in_file, "", strlen(execcmd->in_file)) != 0) {
		in_fd = open(execcmd->in_file, O_RDONLY);
		if (in_fd < 0) {
			perror("redit in_file open");
			return;
		}
	}

	if (strncmp(execcmd->out_file, "", strlen(execcmd->out_file)) != 0) {
		out_fd = open(execcmd->out_file, O_CREAT | O_TRUNC | O_WRONLY, FILE_PERMISSIONS);
		if (out_fd < 0) {
			perror("redir out_file open");
			return;
		}
	}

	if (strncmp(execcmd->err_file, "", strlen(execcmd->err_file)) != 0) {
		if (strncmp(execcmd->err_file, "&1", strlen(execcmd->err_file)) == 0) {
			err_fd = STDOUT_FILENO;
		} else {
			err_fd = open(execcmd->err_file, O_CREAT | O_TRUNC | O_WRONLY, FILE_PERMISSIONS);
			if (err_fd < 0) {
				perror("redir err_file open");
				return;
			}
		}
	}
	dup2(in_fd, STDIN_FILENO);
    dup2(out_fd, STDOUT_FILENO);
	dup2(err_fd, STDERR_FILENO);
    handle_exec(cmd);
}

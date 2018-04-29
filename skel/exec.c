#include "exec.h"
#include "parsing.h"

#define FILE_PERMISSIONS 0664


void handle_exec(struct cmd* cmd);
void handle_back(struct cmd* cmd);
void handle_redir(struct cmd* cmd);
void handle_pipe(struct cmd* cmd) ;

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
    int fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, FILE_PERMISSIONS); // NOLINT
    if (fd < 0) {
        perror("redir out_file open");
        return -1;
    }
    return fd;
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

		case BACK: {
			// runs a command in background
			//
            handle_back(cmd);
			_exit(-1);
		}

		case REDIR: {
			// changes the input/output/stderr flow
			//
            handle_redir(cmd);
			_exit(-1);
		}
		
		case PIPE: {
			// pipes two commands
			//
			handle_pipe(cmd);
			// free the memory allocated
			// for the pipe tree structure
			free_command(parsed_pipe);
		}
        default:break;
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
			perror("redir in_file open");
			return;
		}
	}

	if (strncmp(execcmd->out_file, "", strlen(execcmd->out_file)) != 0) {
		out_fd = open_redir_fd(execcmd->out_file);
		if (out_fd < 0) {
            return;
		}
	}

	if (strncmp(execcmd->err_file, "", strlen(execcmd->err_file)) != 0) {
		if (strncmp(execcmd->err_file, "&1", strlen(execcmd->err_file)) == 0) {
			err_fd = STDOUT_FILENO;
		} else {
			err_fd = open_redir_fd(execcmd->err_file);
			if (err_fd < 0) {
			    return;
			}
		}
	}
	dup2(in_fd, STDIN_FILENO);
    dup2(out_fd, STDOUT_FILENO);
	dup2(err_fd, STDERR_FILENO);
    handle_exec(cmd);
}

void handle_pipe(struct cmd* cmd) {
	struct pipecmd* pipecmd = (struct pipecmd*) cmd;
	int pipefds[2] = {0};

	if (pipe(pipefds) < 0) {
		perror("pipecmd: error creating pipe");
		return;
	}

	pid_t first;
	if ((first = fork()) == 0) {
		dup2(pipefds[1], STDOUT_FILENO);
		exec_cmd(pipecmd->leftcmd);
	} else {
	    waitpid(first, 0, 0);
		dup2(pipefds[0], STDIN_FILENO);
		close(pipefds[1]);
		exec_cmd(pipecmd->rightcmd);
	}
}

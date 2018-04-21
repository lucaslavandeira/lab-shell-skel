#include "runcmd.h"

int status = 0;
struct cmd* parsed_pipe;

void set_env_variables(struct cmd* base_cmd);

// runs the command in 'cmd'
int run_cmd(char* cmd) {
	
	pid_t p;
	struct cmd *parsed;

	// if the "enter" key is pressed
	// just print the promt again
	if (cmd[0] == END_STRING)
		return 0;

	// cd built-in call
	if (cd(cmd))
		return 0;

	// exit built-in call
	if (exit_shell(cmd))
		return EXIT_SHELL;

	// pwd buil-in call
	if (pwd(cmd))
		return 0;

	// parses the command line
	parsed = parse_line(cmd);
	
	// forks and run the command
	if ((p = fork()) == 0) {

        set_env_variables(parsed);
		// keep a reference
		// to the parsed pipe cmd
		// so it can be freed later
		if (parsed->type == PIPE)
			parsed_pipe = parsed;

		exec_cmd(parsed);
	}

	// store the pid of the process
	parsed->pid = p;

	// background process special treatment
	// Hint:
	// - check if the process is 
	// 	going to be run in the 'back'
	// - print info about it with 
	// 	'print_back_info()'
	//
    if (parsed->type == BACK) {
        print_back_info(parsed);
        return 0;
    }

	// waits for the process to finish
	waitpid(p, &status, 0);
	
	print_status_info(parsed);
	
	free_command(parsed);

	return 0;
}

void set_env_variables(struct cmd* base_cmd) {
    struct execcmd* cmd = NULL;
    if (base_cmd->type == EXEC) {
        cmd = (struct execcmd*) base_cmd;
    } else if (base_cmd->type == BACK) {
        cmd = (struct execcmd*) ((struct backcmd*) base_cmd)->c;
    } else {
        fprintf(stderr, "Unrecognized type: %d\n", base_cmd->type);
        return;
    }

    for (int i = 0; i < cmd->eargc; ++i) {
        char* value = NULL;
        char name[ARGSIZE] = {0};
        size_t name_len = 0;
        for (int j = 0; j < strlen(cmd->eargv[i]); ++j) {
            if (cmd->eargv[i][j] == '=') {
                value = cmd->eargv[i] + j + 1;
                name_len = j;
                break;
            }
        }
        if (!value) {
            fprintf(stderr, "eargv without '=' found\n");
            return;
        }

        strncpy(name, cmd->eargv[i], name_len);
        if (setenv(name, value, 1) < 0) {
            perror("setenv");
        }
    }
}
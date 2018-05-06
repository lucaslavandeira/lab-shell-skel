#include "defs.h"
#include "types.h"
#include "readline.h"
#include "runcmd.h"

char promt[PRMTLEN] = {0};
pid_t background_process = 0;
char background_prompt[PRMTLEN] = {0};

// runs a shell command
static void run_shell() {

	char* cmd;

	while ((cmd = read_line(promt)) != NULL)
		if (run_cmd(cmd) == EXIT_SHELL)
			return;
}

// initialize the shell
// with the "HOME" directory
static void init_shell() {

	char buf[BUFLEN] = {0};
	char* home = getenv("HOME");

	if (chdir(home) < 0) {
		snprintf(buf, sizeof buf, "cannot cd to %s ", home);
		perror(buf);
	} else {
		snprintf(promt, sizeof promt, "(%s)", home);
	}	
}

void background_process_notify(int sig) {
    if (background_process) {
        printf("terminado: PID=%d (%s)\n", background_process, background_prompt);
    }
    background_process = 0;
    memset(background_prompt, 0, PRMTLEN);
}

void add_handler() {
    struct sigaction s;
    memset(&s, 0, sizeof(s));
    s.sa_flags = SA_SIGINFO | SA_RESTART;
    s.sa_handler = background_process_notify;
    sigaction(SIGCHLD, &s, NULL);
}

int main(void) {
    add_handler();

	init_shell();

	run_shell();

	return 0;
}


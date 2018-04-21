#include "builtin.h"
#include "defs.h"

// returns true if the 'exit' call
// should be performed
int exit_shell(char* cmd) {
    char command[ARGSIZE];
    if (sscanf(cmd, "%s", command) < 0) {
        return 0;
    }

    if (strncmp(command, "exit", ARGSIZE) != 0) {
        return 0;
    }

    return 1;
}

// returns true if "chdir" was performed
// this means that if 'cmd' contains:
// 	$ cd directory (change to 'directory')
// 	$ cd (change to HOME)
// it has to be executed and then return true
int cd(char* cmd) {
    char command[ARGSIZE];
    char dir[BUFLEN] = {0};
    if (sscanf(cmd, "%s %s", command, dir) < 0) {
        perror("cd sscanf");
        return 0;
    }

    if (strncmp(command, "cd", ARGSIZE) != 0) { // not cd
        return 0;
    }

    if (!strlen(dir)) {
        strncpy(dir, getenv("HOME"), ARGSIZE);
    }

	if (chdir(dir) < 0) {
        perror("cd chdir");
        return 0;
    }

    if (getcwd(dir, BUFLEN) < 0) {
        perror("cd getcwd");
        return 0;
    }

    snprintf(promt, PRMTLEN, "(%s)", dir);
	return 1;
}

// returns true if 'pwd' was invoked
// in the command line
int pwd(char* cmd) {
    char command[ARGSIZE];
    if (sscanf(cmd, "%s", command) < 0) {
        perror("pwd sscanf");
        return 0;
    }

    if (strncmp(command, "pwd", ARGSIZE) != 0) {
        return 0;
    }

    char wd[BUFLEN];
    if (getcwd(wd, BUFLEN) < 0) {
        perror("cd getcwd");
        return 0;
    }
    printf("%s\n", wd);
    return 1;
}


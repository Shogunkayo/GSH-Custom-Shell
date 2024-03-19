#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <ostream>
#include <regex>
#include <string>
#include <vector>
#include "main.h"

std::vector<std::string> global_history;
std::unordered_map<std::string, int (*)(std::vector<std::string>)> cmd_builtins {
	{"cd", &gsh_cd},
	{"touch", &gsh_touch},
	{"cp", &gsh_cp},
	{"ls", &gsh_ls},
	{"history", &gsh_history},
	{"clear", &gsh_clear}
};

void gsh_loop(void) {
	std::string line;
	std::vector<std::string> args;
	bool status;

	do {
		std::cout << "$ ";
		line = gsh_read_line();
		args = gsh_split_line(line);
		status = gsh_execute(args);

	} while (status);
}

std::string gsh_read_line(void) {
	std::string input;
	std::getline(std::cin, input);
	global_history.push_back(input);

	return input;
}

std::vector<std::string> gsh_split_line(std::string input) {
	std::regex re("[ \n\a\r\t]+");
	std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;
	
	return std::vector<std::string> {first, last};
}

int gsh_launch(std::vector<std::string> args) {
	pid_t pid;
	int status;

	char **c_args = new char*[args.size() + 1];

	for (long unsigned int i = 0; i < args.size(); i++) {
		c_args[i] = new char[args[i].length() + 1];
		std::strcpy(c_args[i], args[i].c_str());
	}

	pid = fork();
	if (pid == 0) {
		if (execvp(c_args[0], c_args) == -1) {
			perror("gsh");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0) {
		perror("gsh");
	}
	else {
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int gsh_execute(std::vector<std::string> args) {
	if (args[0].empty()) {
		return 1;
	}

	if (cmd_builtins.count(args[0])) {
		(* cmd_builtins[args[0]]) (args);
	}
	
	return gsh_launch(args);
}

std::string get_evar(std::string key) {
	char *val = std::getenv(key.c_str());
	std::string retval = "";
	if (val != NULL) {
		retval = val;
	}

	return retval;
}

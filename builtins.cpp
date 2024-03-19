#include "main.h"
#include <algorithm>
#include <bits/types/struct_timeval.h>
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/param.h>
#include <dirent.h>
#include <sys/time.h>
#include <cstdlib>

int gsh_history(std::vector<std::string> args) {
	if (args.size() > 1) {
		std::cerr << "gsh: history: Too many arguments" << std::endl;
		return 1;
	}

	for (auto cmd : global_history) {
		std::cout << cmd << "\n";
	}

	return 0;
}

int gsh_clear(std::vector<std::string> args) {
	if (args.size() > 1) {
		std::cerr << "gsh: clear: Too many arguments" << std::endl;
		return 1;
	}
	
	global_history.clear();
	return 0;
}

int gsh_touch(std::vector<std::string> args) {
	if (args.size() < 2) {
		std::cerr << "gsh: touch: Missing file operand" << std::endl;
		return 1;
	}
	
	struct stat buffer;
	struct timeval times[2];

	for (long unsigned int i = 1; i < args.size(); i++) {		
		if (stat(args[i].c_str(), &buffer) == 0) {
			// file already exists, update timestamps
			times[0].tv_sec = times[1].tv_sec = time(NULL);
			times[0].tv_usec = times[1].tv_usec = 0;
			
			if (utimes(args[i].c_str(), times) != 0) {
				perror("gsh");
				return 1;
			}
		}
		else {
			// file does not exist, create file
			std::ofstream {args[i]};
		}
	}
	
	return 0;
}

int gsh_cd(std::vector<std::string> args) {
	if (args.size() > 2) {
		std::cerr << "gsh: cd: Too many arguments\n";
		return 1;
	}

	if (args.size() < 2) {
		if (chdir(get_evar("HOME").c_str()) != 0) {
			perror("gsh");
		}
		return 0;
	}

	if (args[1].compare("..") == 0) {
		if (chdir("..") != 0){
			perror("gsh");
			return 1;
		}
		return 0;
	}
	else {
		if (chdir(args[1].c_str()) != 0) {
			perror("gsh");
			return 1;
		}
		return 0;
	}
}

int gsh_ls(std::vector<std::string> args) {
	std::vector<std::string> files;
	std::vector<std::string> paths;
	std::vector<std::string> flags;
	DIR *dirp;
	struct dirent *dp;

	for (long unsigned int i = 1; i < args.size(); i++) {
		if (args[i][0] == '-') {
			// flags not yet implemented
		}
		else {
			paths.push_back(args[i]);
		}
	}

	if (paths.empty()) {
		paths.push_back(".");
	}

	for (auto path : paths) {
		std::cout << "\n" << path << ":\n";
		files.clear();
		
		if ((dirp = opendir(path.c_str())) == NULL) {
			std::cerr << "gsh: ls: target '" << path << "': No such directory" << std::endl;
			continue;
		}

		while ((dp = readdir(dirp)) != NULL) {
			if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
				files.push_back(dp->d_name);
		}
		
		std::sort(files.begin(), files.end(), [] (const std::string lhs, const std::string rhs) {
			// convert to lowercase and compare only mismatched characters
			const auto result = std::mismatch(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), [] (const unsigned char lhs, const unsigned char rhs) {
				return tolower(lhs) == tolower(rhs);
			});
			
			return result.second != rhs.cend() && (result.first == lhs.cend() || tolower(*result.first) < tolower(*result.second));
		});

		std::cout << ".  ..";
		for (auto file : files) {
			std::cout << "  " << file;
		}
		std::cout << std::endl;
	}

	return 0;
}

int gsh_cp(std::vector<std::string> args) {
	if (args.size() < 2) {
		std::cerr << "gsh: cp: Missing file operand" << std::endl;
		return 1;
	}

	if (args.size() < 3) {
		std::cerr << "gsh: cp: Missing destination file operand after '" << args[1] << "'" << std::endl;
		return 1;
	}

	std::string destination = args[args.size()-1];
	int st;
	struct stat buf;

	if (args.size() > 3) {
		if ((st = lstat(destination.c_str(), &buf)) != 0) {
			std::cerr << "gsh: cp: target '" << destination << "': No such file or directory" << std::endl;
			return 1;
		}
		if (!S_ISDIR(buf.st_mode)) {
			std::cerr << "gsh: cp: target '" << destination << "': Not a directory" << std::endl;
			return 1;
		}
	}

	int fd_dest = open(destination.c_str(), O_WRONLY | O_CREAT, 0644);
	int fd_src;
	size_t size;
	char file_buf[BUFSIZ];

	for (long unsigned int i = 1; i < args.size() - 1; i++) {
		if ((fd_src = open(args[i].c_str(), O_RDONLY, 0)) < 0) {
			std::cerr << "gsh: cp: source '" << args[i] << "' : No such file or directory" << std::endl;
			continue;
		}
		
		while ((size = read(fd_src, file_buf, BUFSIZ)) > 0) {
			write(fd_dest, file_buf, size);
		}

		close(fd_src);
	}

	close(fd_dest);

	return 0;
}

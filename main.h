#include <vector>
#include <stack>
#include <string>

#ifndef MAIN_H
#define MAIN_H

// IOUtil
extern std::vector<std::string> global_history;

void gsh_loop(void);
std::string gsh_read_line(void);
std::vector<std::string> gsh_split_line(std::string);
int gsh_execute(std::vector<std::string>);
int gsh_launch(std::vector<std::string>);
std::string get_evar(std::string);

// Builtins
int gsh_cd(std::vector<std::string>);
int gsh_ls(std::vector<std::string>);
int gsh_touch(std::vector<std::string>);
int gsh_cp(std::vector<std::string>);
int gsh_history(std::vector<std::string>);
int gsh_clear(std::vector<std::string>);


#endif // !MAIN_H

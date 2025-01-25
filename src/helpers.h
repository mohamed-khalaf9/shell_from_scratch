#ifndef HELPERS_H
#define HELPERS_H
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include "trie.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stack>
#include <algorithm>

std::string parse_input_with_autocomplete(std::string &input, Trie &trie, bool first_time = true);
void autocomplete(std::string &input, Trie &trie);
void add_executables_to_trie(Trie &trie);
std::vector<std::string> split(const std::string &s, char delimiter);
bool is_path_exist(const std::string &path);
std::pair<std::string, std::string> fetch_command_and_argument(const std::string &input);
std::vector<std::string> handle_quoting(std::string argument);
std::vector<std::string> handle_single_qoutes(const std::string &argument);
std::vector<std::string> handle_double_quotes(const std::string &argument);
std::string handle_non_quoted_backslash(const std::string &argument);
std::string remove_extra_spaces(const std::string &str);
std::unordered_map<std::string, std::string> load_commands();
std::string trim(const std::string &str);
int detect_redirection(const std::string &argument);
std::pair<std::string, std::streambuf *> handle_redirection(const std::string &op, std::string file_name);
std::string is_executable_file_exists_in_path(const std::string &file_name);
bool handle_relative_path(std::vector<std::string> &path_tokens);
std::string remove_last_token_from_working_directory(const std::string &workingDirectory);
bool change_directory(const std::string &path);
std::tuple<std::string, std::string, std::string> parse_redirection(const std::string &argument, int redirection_index);
#endif
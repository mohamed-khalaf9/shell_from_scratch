
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <stack>
#include <fstream>
#include <algorithm>
#include "helpers.h"
#include "globals.h"
#include <cstdlib>
#include <sys/stat.h>
#include <stdio.h>
#include <optional>
#include <sys/wait.h>

void handle_cat(const std::string &argument)
{
  
  if (!argument.empty())
  {
    std::vector<std::string> file_paths;
    if (argument[0] == '\'' || argument[0] == '\"')
    {
      file_paths = handle_quoting(argument);

      for (auto &path : file_paths)
      {
        if (path == " " || path == "")
        {
          continue;
        }
        std::filesystem::path full_path = std::filesystem::absolute(path);
        if (std::filesystem::exists(full_path)&&std::filesystem::is_regular_file(path))
        {

          std::ifstream file(full_path);

          if (file.is_open())
          {
            std::string line;
            while (std::getline(file, line))
            {
              std::cout << line<<std::endl;
            }
            file.close();
          }
        }
        else
        {
          std::cerr << "cat: " << path << ": No such file\n";
        }
      }
    }
    else
    {

      file_paths = split(argument, ' ');
      for (auto &path : file_paths)
      {
        if (path == " " || path == "")
        {
          continue;
        }
        std::filesystem::path full_path = std::filesystem::absolute(path);
        if (path != " " && path != "" && std::filesystem::exists(full_path) && std::filesystem::is_regular_file(path))
        {

          std::ifstream file(full_path);

          if (file.is_open())
          {
            std::string line;
            while (std::getline(file, line))
            {
              std::cout<<line<<std::endl;
            }
            file.close();
          }
        }
        else
        {
          std::cerr << "cat: " << path << ": No such file or directory\n";
        }
      }
    }
    std::cout<<std::endl;
  }
 
}

void handle_ls(std::string &argument)
{

  argument = trim(argument); 
  if(argument[0] == '\'' || argument[0] == '\"')
  {
    std::vector<std::string> tokens = handle_quoting(argument);
    std::string tmp="";
    for(const auto &token : tokens)
    {
      tmp+=token;
    }
    argument = tmp;
  }
  else if(argument.find('\\') != std::string::npos)
  {
    argument = handle_non_quoted_backslash(argument);
  }

  std::string target_dir = argument.empty() ? WORKING_DIRECTORY : argument;

  std::optional<struct stat> path_state = is_path_exist(target_dir);
  if (path_state != std::nullopt && S_ISDIR(path_state->st_mode))
  {
    std::vector<std::string> entries;
    for (const auto &entry : std::filesystem::directory_iterator(target_dir))
    {
      std::string item = entry.path().filename().string();

      if(std::filesystem::is_directory(entry))
      {
        item+='/';
      }
      entries.push_back(item);
    }
    std::sort(entries.begin(), entries.end()); 

    for (const auto &entry : entries)
    {
      std::cout << entry << std::endl; 
      }
    
  }
  else if (path_state != std::nullopt && S_ISREG(path_state->st_mode))
  {
    std::cout << target_dir << std::endl;
  }
  else
  {
    std::cerr << "ls: " << argument << ": No such file or directory" << std::endl;
  }
}


void handle_echo(std::string &argument)
{
  if (!argument.empty())
  {
    if (argument[0] == '\'' || argument[0] == '\"')
    {
      std::vector<std::string> tokens = handle_quoting(argument);
      if (tokens.size() == 0)
      {
        std::cerr << argument << ": syntax error\n";
      }
      else
      {
        for (const auto &token : tokens)
        {
          std::cout << token;
        }
        std::cout << std::endl;
      }
    }
    // handle backslach in non quoted arguments for echo command
    else if (argument.find('\\') != std::string::npos)
    {

      std::cout << handle_non_quoted_backslash(argument) << std::endl;
    }
    else
    {

      std::string res = remove_extra_spaces(argument);
      std::cout << res << "\n";
    }
  }
}

void handle_type(std::string &argument, std::unordered_map<std::string, std::string> &commands)
{
  argument = trim(argument);
  
  if (!argument.empty())
  {
    if (commands.count(argument))
    {
      std::cout << argument << " " << commands[argument] << std::endl;
    }
    else
    {
      if(argument[0]== '\'' || argument[0] == '\"')
      {
        std::vector<std::string> tokens = handle_quoting(argument);
        std::string tmp="";
        for(const auto &token : tokens)
        {
          tmp+=token;
        }
        argument = tmp;
      }
      else if(argument.find('\\') != std::string::npos)
      {
        argument = handle_non_quoted_backslash(argument);
      }
      // check if this is an executable file
      std::string full_path = is_executable_file_exists_in_path(argument);
      if (full_path != "")
      {
        std::cout << argument << " is " << full_path << std::endl;
      }
      else
      {
        std::cerr << argument << ": not found\n";
      }
    }
  }
}

void handle_pwd()
{
  std::cout << WORKING_DIRECTORY << std::endl;
}

void handle_cd(std::string &argument)
{
  argument = trim(argument);
  //handle quoted paths - for now only handling arugments that start with single or double quotes
  if(argument[0] == '\'' || argument[0] == '\"')
  {
    std::vector<std::string> tokens = handle_quoting(argument);
    std::string tmp="";
    for(const auto &token : tokens)
    {
      tmp+=token;
    }
    argument = tmp;
  }
  else if(argument.find('\\') != std::string::npos)
  {
    argument = handle_non_quoted_backslash(argument);
  }
  if (!argument.empty() && change_directory(argument))
  {
    return;
  }
  else
  {
    std::cerr << argument << ": No such file or directory\n";
  }
}

void execute_Programm(std::string &command, std::string &argument)
{
  command = trim(command);
  argument = trim(argument);
  
  std::string programm_name;

  if (command[0] == '\'' || command[0] == '\"')
  {
    std::vector<std::string> tokens = handle_quoting(command);
    for (const auto &token : tokens)
    {
      programm_name += token;
    }
  }
  else if (command.find('\\') != std::string::npos)
  {
    programm_name = handle_non_quoted_backslash(command);
  }
  else
  {
    programm_name = command;
  }

  std::string programm_argument = argument;

  // check if programm is an executable file in path
  std::string full_path = is_executable_file_exists_in_path(programm_name);
  if (full_path != "")
  {

    std::vector<std::string> args = {programm_name,programm_argument};
    std::vector<char *> argv;

    for(const auto &arg : args)
    {
      argv.push_back(const_cast<char *>(arg.c_str()));

    }
    argv.push_back(nullptr);

    pid_t pid = fork();
    if(pid == -1)
    {
      std::cerr<<"fork failed\n";
    }
    else if(pid == 0)
    {
      execv(full_path.c_str(),argv.data());
      std::cerr<<"execv failed\n";
      exit(1);
    }
    else
    {
      int status;
      if(waitpid(pid,&status,0) == -1)
      {
        std::cerr << "Error waiting for child process: " <<"\n";
      }
      else
      {
        if(WIFEXITED(status))
        {
          std::cout<<"Programm exited with status: "<<WEXITSTATUS(status)<<std::endl;
        }
        else
        {
          std::cerr<<"Programm terminated abnormally\n";
        }

    }

    }
  }
  else{
 std::cerr << programm_name << ": not found\n";}
}


void run()
{

  std::streambuf *cout_original_buf = std::cout.rdbuf();
  std::streambuf *cerr_original_buf = std::cerr.rdbuf();

  Trie trie;
  trie.insert("echo");
  trie.insert("exit");
  trie.insert("type");
  trie.insert("pwd");
  trie.insert("cd");
  trie.insert("cat");
  trie.insert("ls");

  add_executables_to_trie(trie);

  // shell commands and descriptions
  std::unordered_map<std::string, std::string> commands = load_commands();

  std::string input = "";

  while (true)
  {
    std::cout << "$ ";
    input.clear();

    input = parse_input_with_autocomplete(input, trie);

    std::string command;
    std::string argument;

    std::pair<std::string, std::string> p = fetch_command_and_argument(input);
    command = p.first;
    argument = p.second;

    int redirection_index = detect_redirection(argument);
    if (redirection_index != -1)
    {
      std::tuple<std::string, std::string, std::string> t = parse_redirection(argument, redirection_index);
      std::string arg, op, file_name;
      arg = std::get<0>(t);
      op = std::get<1>(t);
      file_name = std::get<2>(t);

      argument = arg;
      std::pair<std::string, std::streambuf *> p = handle_redirection(op, file_name);
      if (p.first == "cout")
      {
        std::cout.rdbuf(p.second);
      }
      else if (p.first == "cerr")
      {
        std::cerr.rdbuf(p.second);
      }
    }

    command = trim(command);

    if (command == "exit")
    {
      exit(stoi(argument));
    }
    else if (command == "echo" || command == "echo:")
    {
      handle_echo(argument);
    }
    else if (command == "type" || command == "type:")
    {

      handle_type(argument, commands);
    }
    else if (command == "pwd" || command == "pwd:")
    {
      handle_pwd();
    }
    else if (command == "cd" || command == "cd:")
    {
      handle_cd(argument);
    }
    else if (command == "cat" || command == "cat:")
    {
      handle_cat(argument);
    }

    else if (command == "ls" || command == "ls:")
    {
      handle_ls(argument);
    }
    else
    {
      execute_Programm(command, argument);
    }

    std::cout.rdbuf(cout_original_buf);
    std::cerr.rdbuf(cerr_original_buf);
    file.close();
  }
}

int main()
{

  run();

  return 0;
}

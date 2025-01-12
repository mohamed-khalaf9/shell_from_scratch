#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <filesystem>

std::vector<std::string> split(const std::string &s, char delimiter)
{
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream ss(s);

  while (std::getline(ss, token, delimiter))
  {
    tokens.push_back(token);
  }

  return tokens;
}

std::string is_executable_file_exists_in_path(const std::string &file_name)
{
  const char *path = std::getenv("PATH");
  if (path)
  {

    std::vector<std::string> dirs = split(path, ':');

    for (const auto &dir : dirs)
    {
      std::string full_path = dir + "/" + file_name;
      if (std::filesystem::exists(full_path))
      {
        return full_path;
      }
    }
    return "";
  }
  else
    return "";
}

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // shell commands and descriptions
  std::unordered_map<std::string, std::string> commands;
  commands.emplace("type", "is a shell builtin");
  commands.emplace("echo", "is a shell builtin");
  commands.emplace("exit", "is a shell builtin");
  commands.emplace("pwd", "is a shell builtin");

  // Uncomment this block to pass the first stage
  std::string input;

  while (true)
  {
    std::cout << "$ ";
    std::getline(std::cin, input);
    std::stringstream ss(input);

    std::string command;
    std::string argument;
    ss >> command;

    std::getline(ss, argument);

    if (command == "exit")
    {
      exit(stoi(argument));
    }
    else if (command == "echo" || command == "echo:")
    {
      if (!argument.empty() && argument[0] == ' ')
      {
        argument.erase(argument.begin());
        std::cout << argument << std::endl;
      }
    }
    else if (command == "type" || command == "type:")
    {
      if (!argument.empty() && argument[0] == ' ')
      {
        argument.erase(argument.begin());
      }

      if (commands.count(argument))
      {
        std::cout << argument << " " << commands[argument] << std::endl;
      }
      else
      {
        // check if this is an executable file
        std::string full_path = is_executable_file_exists_in_path(argument);
        if (full_path != "")
        {
          std::cout << argument << " is " << full_path << std::endl;
        }
        else
        {
          std::cout << argument << ": not found\n";
        }
      }
    }
    else if(command=="pwd" || command=="pwd:"){
      std::cout<<std::filesystem::current_path()<<std::endl;
    }
    else
    {

      // exectue programm
      std::string programm_name = command;
      std::string programm_argument = argument;

      // check if programm is an executable file
      std::string full_path = is_executable_file_exists_in_path(programm_name);
      if (full_path != "")
      {

        std::string command = programm_name + " " + programm_argument;
        system(command.c_str());
      }
      else
      {
        std::cout << programm_name << ": not found\n";
      }
    }
  }
}

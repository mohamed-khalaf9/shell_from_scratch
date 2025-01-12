#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <filesystem>


std::string WORKING_DIRECTORY = std::filesystem::current_path().string();


std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(" ");
    size_t last = str.find_last_not_of(" ");
    return (first == std::string::npos || last == std::string::npos)
               ? ""
               : str.substr(first, last - first + 1);
}

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
bool is_path_exist(const std::string& path)
{
  return std::filesystem::exists(path);
}
std::string remove_last_token_from_working_directory(const std::string &workingDirectory) {
    std::string updatedDirectory = workingDirectory;  // Make a copy

    // Find the position of the last '/'
    std::size_t pos = updatedDirectory.find_last_of('/');

    // If '/' is found and it's not the only character (e.g., root '/')
    if (pos != std::string::npos && pos > 0) {
        // Erase everything from the last '/' to the end
        updatedDirectory.erase(pos);
    } else if (pos == 0) {
        // If we are at the root '/', set it to "/"
        updatedDirectory = "/";
    }

    return updatedDirectory;  // Return the updated directory
}

bool handel_relative_path(const std::vector<std::string>& path_tokens)
{
  for(int i=0; i<path_tokens.size(); i++)
  {
    if(path_tokens[i]==".")
    {
      int j = i+1;
      std::string tmp_path = WORKING_DIRECTORY;
      while(j<path_tokens.size() && (path_tokens[j]!="." || path_tokens[j]!=".."))
      {
        tmp_path+="/"+path_tokens[j];
        if(!is_path_exist(tmp_path)) return false;
        j++;
      }
      WORKING_DIRECTORY = tmp_path; 
    }
    else if(path_tokens[i]=="..")
    {
      int j = i+1;
      std::string tmp_path = WORKING_DIRECTORY;
      tmp_path = remove_last_token_from_working_directory(tmp_path);
      while(j<path_tokens.size() && (path_tokens[j]!="." || path_tokens[j]!=".."))
      {
        tmp_path += "/"+path_tokens[j];
        if(!is_path_exist(tmp_path)) return false;
        j++;
      }
      WORKING_DIRECTORY = tmp_path; 
    }

  }

  return true;
  

}
bool change_directory(const std::string& path){
  std::string trimmed_path = trim(path);
  std::vector<std::string> path_tokens = split(trimmed_path,'/');

  if(path_tokens.size()==0)
  {
    return false;
  }
  else if(path_tokens[0] == "." || path_tokens[0] == "..")
  {
    //handel relative path
    if(handel_relative_path(path_tokens))
    {
      return true;
    }
    else
    {
      return false;
    }

  }  
  else
  {
  if(is_path_exist(trimmed_path))
  {

    WORKING_DIRECTORY = path;
    return true;

  }
  else
  return false;
  }

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
      std::cout<<WORKING_DIRECTORY<<std::endl;
    }
    else if(command=="cd" || command=="cd:"){
      argument = trim(argument);
      if(!argument.empty()&&change_directory(argument))
      {
        continue;
      }
      else
      {
        std::cout<<argument<<": No such file or directory\n";
      }
      

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

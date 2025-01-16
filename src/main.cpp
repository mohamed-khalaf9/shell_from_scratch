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


  bool handle_relative_path(std::vector<std::string>& path_tokens) {
    for (const auto& token : path_tokens) {
        if (token == "..") {
            // Move up one level: pop the last directory from WORKING_DIRECTORY
            if (!WORKING_DIRECTORY.empty()) {
                size_t pos = WORKING_DIRECTORY.find_last_of('/');
                if (pos != std::string::npos) {
                    WORKING_DIRECTORY = WORKING_DIRECTORY.substr(0, pos);
                }
            }
        } else if (token != "." && token != "") {
            // For any other valid directory name, add it to WORKING_DIRECTORY
            if (WORKING_DIRECTORY.back() != '/') {
                WORKING_DIRECTORY += '/';
            }
            WORKING_DIRECTORY += token;
        }
    }

    // After handling all tokens, ensure the path is valid
    return is_path_exist(WORKING_DIRECTORY);
}

bool change_directory(const std::string& path){
  std::string trimmed_path = trim(path);
  if(trimmed_path == "~")
  {
    WORKING_DIRECTORY = std::getenv("HOME");
    return true;
  }
  std::vector<std::string> path_tokens = split(trimmed_path,'/');

  if(path_tokens.size()==0)
  {
    return false;
  }
  else if(path_tokens[0] == "." || path_tokens[0] == "..")
  {
    //handel relative path
    if(handle_relative_path(path_tokens))
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

std::string handle_quoting(std::string argument)
{
  std::stack<char> qoutes;
  std::string res = "";
  std::string token = "";

  if(argument.empty())  return argument;

  // handel single qoutes
  if(argument[0] == '\'')
  {
    for(auto c: argument)
    {
      if(qoutes.empty() && c=='\'')
      qoutes.push(c);

      else if(!qoutes.empty() && c=='\'')
      {
        qoutes.pop();
        res+=token;
        token="";
      }
      else
      {
        if(!qoutes.empty()) 
        token+=c;
        else
         res+=c;
      }
    }

    if(!qoutes.empty())
     return argument;


    return res;

  }
  else
  return argument;
}
std::string remove_extra_spaces(const std::string& str) {
    std::stringstream ss(str);
    std::string word, result;

    // Read each word and append with a single space
    while (ss >> word) {
        if (!result.empty()) {
            result += " ";
        }
        result += word;
    }

    return result;
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
  commands.emplace("cd", "is a shell builtin");
  commands.emplace("cat", "is a shell builtin");

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
    argument = trim(argument);
    
   

    if (command == "exit")
    {
      exit(stoi(argument));
    }
    else if (command == "echo" || command == "echo:")
    {
      if (!argument.empty())
      {
        if(argument[0] == '\'')
        {
          argument = handle_quoting(argument);
          std::cout << argument << std::endl;
        }
        else
        {
          std::cout << remove_extra_spaces(argument) << std::endl;
        }
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
    else if(command=="cat" || command == "cat:")
    {
      std::vector<std::string> file_paths = split(argument,' ');
      for(const auto& path : file_paths)
      {
        std::string file_path = trim(path);
        if(is_path_exist(file_path))
        {
          std::ifstream file(file_path);

          if(file.is_open()){
          std::string line;
          while(std::getline(file,line))
          {
            std::cout<<line<<std::endl;
          }
          file.close();
          }
          else
          {
            std::cout<<file_path<<": No such file or directory\n";
          }
          

        }
        else
        {
            std::cout<<file_path<<": No such file or directory\n";
          }

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

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
std::vector<std::string> handle_double_quotes(const std::string& argument){
  std::stack<char> qoutes;
  std::vector<std::string> tokens;
  std::string token = "";
  bool inside_quotes = false;
  bool escape = false;
  bool prev_space = false;

   for(int i=0 ; i<(int)argument.size(); i++)
    {
      char c = argument[i];
      
      if(qoutes.empty() && c=='\"' && !escape){
        prev_space = false;
        if(token!="")
        {
          tokens.push_back(token);
          token.clear();
        }
        qoutes.push(c);
        inside_quotes = true;
        continue;
      }

      else if(!qoutes.empty() && c=='\"' && qoutes.top()=='\"' && !escape)
      {
        qoutes.pop();
        tokens.push_back(token);
        token.clear();
        inside_quotes = false;
        continue;
      }
      else if(c=='\\' && !escape && ((i+1<(int)argument.size())&& (argument[i+1]=='\"' || argument[i+1]=='\\' || argument[i+1]=='$')))
      {
        escape = true;
        continue;
      }
      else if(c=='\\' && !escape)
      {
        token+=c;
        continue;
      }
      else if(c=='\\' || c=='$' || c=='\"'  && escape)
      {
       
          token +=c;
          escape = false;
          continue;
        // for future handling of specail characters using else

      }
        // Handle spaces outside quotes (normalize them to one space)
        else if (isspace(c) && !inside_quotes) {
            if (!prev_space) { // Only add one space if the previous character wasn't a space
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                token += " "; // Add exactly one space
                prev_space = true; // Set the flag to indicate a space has been added
            }
            continue;
    }
            prev_space = false;
            token += c;
            
        
        
    }
    tokens.push_back(token);
    
    if(!qoutes.empty())
    {
      tokens.clear();
      return tokens;
    }


    return tokens;

    }
std::vector<std::string> handle_single_qoutes(const std::string & argument)
{
  std::stack<char> qoutes;
  std::vector<std::string> tokens;
  std::string token = "";
  bool inside_qoutes = false;

   for(auto c: argument)
    {
      if(qoutes.empty() && c=='\''){
        if(token!="")
        {
          tokens.push_back(token);
          token.clear();
        }
        qoutes.push(c);
        inside_qoutes = true;
        continue;
      }

      else if(!qoutes.empty() && c=='\'' && qoutes.top()=='\'')
      {
        qoutes.pop();
        tokens.push_back(token);
        token.clear();
        inside_qoutes = false;
        continue;
      }
      else
      {
       token += c;
      }

  

    }
        if(!qoutes.empty())
    {
      tokens.clear();
      return tokens;
    }


    return tokens;
  
  

}

std::vector<std::string> handle_quoting(std::string argument)
{
  std::vector<std::string> tokens;

  // handel single qoutes
  if(argument[0]=='\'')
  {
    tokens = handle_single_qoutes(argument);
    return tokens;
  }
  else if(argument[0]=='\"')
  {
    tokens = handle_double_quotes(argument);
    return tokens;
  }
  else
  return tokens;
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

std::string handle_non_quoted_backslash(const std::string& argument)
{
  std::string res="";
  if(argument.find('\\') != std::string::npos && argument[0] != '\'' && argument[0]!='\"' )
  {
    for(const auto& c: argument)
    {
      if(c=='\\')
      continue;
      
      res+=c;
    }
    return res;
  }
  else
  return argument;

}

void handle_cat(const std::string& argument)
{
  if(!argument.empty()){
      if(argument[0] == '\'' || argument[0] == '\"')
      {
      std::vector<std::string> file_paths = handle_quoting(argument);
      for(const auto& path : file_paths)
      {
        if(path!= " " && is_path_exist(path))
        {
          

          std::ifstream file(path);

          if(file.is_open()){
          std::string line;
          while(std::getline(file,line))
          {
            std::cout<<line;
          }
          file.close();
          }
          
          
          

        }
       

      }
      }
      else
      {
        if(argument!= " " && is_path_exist(argument))
        {
          std::ifstream file(argument);
          if(file.is_open()){
          std::string line;
          while(std::getline(file,line))
          {
            std::cout<<line;
          }
          file.close();
          }
      }
      std::cout<<std::endl;

    
  }
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
  commands.emplace("cd", "is a shell builtin");
  commands.emplace("cat", "is /usr/bin/cat");

  // Uncomment this block to pass the first stage
  std::string input;

  while (true)
  {
    std::cout << "$ ";
    std::getline(std::cin, input);
    std::stringstream ss(input);

    std::string command;
    std::string argument;

       size_t pos = 0;

    // Skip leading spaces
    while (pos < input.size() && std::isspace(input[pos])) {
        ++pos;
    }

    // Check if the command is quoted
    if (input[pos] == '\'' || input[pos] == '\"') {
        char quote_char = input[pos]; // Store which quote we're using (single or double)
        command += input[pos++]; // Include the opening quote in the command

        // Collect the entire quoted string (including spaces inside the quotes)
        while (pos < input.size() && input[pos] != quote_char) {
            command += input[pos++];
        }

        if (pos < input.size()) {
            command += input[pos++]; // Include the closing quote in the command
        }
    } else {
        // Collect the command until we hit a space (no quotes involved)
        while (pos < input.size() && !std::isspace(input[pos])) {
            command += input[pos++];
        }
    }

    // Skip the spaces after the command to collect the argument
    while (pos < input.size() && std::isspace(input[pos])) {
        ++pos;
    }

    // Collect the argument (if there's anything left)
    if (pos < input.size()) {
        argument = input.substr(pos);
    }
    
   

    if (command == "exit")
    {
      exit(stoi(argument));
    }
    else if (command == "echo" || command == "echo:")
    {
      if (!argument.empty())
      {
        if(argument[0] == '\'' || argument[0] == '\"')
        {
         std::vector<std::string> tokens = handle_quoting(argument);
         if(tokens.size()==0)
         {
          std::cout<<argument<<": syntax error\n";
         }
         else
         {
          for(const auto& token: tokens)
          {
            std::cout<<token;
          }
          std::cout<<std::endl;
         }
        }
        // handle backslach in non quoted arguments for echo command
        else if(argument.find('\\') != std::string::npos)
        {
          std::cout<<handle_non_quoted_backslash(argument)<<std::endl;
        }
        else
        {
          std::cout << remove_extra_spaces(argument) << std::endl;
        }
      }
    }
    else if (command == "type" || command == "type:")
    {
      argument = trim(argument);

      if(!argument.empty())
      {
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
      handle_cat(argument);
    }
    else
    {
      if(command[0] == '\'' || command[0] == '\"')
      {
        handle_cat(argument);
        continue;
      }

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

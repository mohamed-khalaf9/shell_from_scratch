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
std::ofstream file; // for redirection

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
      std::vector<std::string>  file_paths;
      if(argument[0] == '\'' || argument[0] == '\"')
      {
      file_paths = handle_quoting(argument);
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
        else
        {
          std::cerr<<"cat: "<<path<<": No such file or directory\n";
        }
       

      }
      }
      else
      {
        file_paths = split(argument,' ');
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
        else{
          std::cerr<<"cat: "<<path<<": No such file or directory\n";
        }
       

      }
  }
  std::cout<<std::endl;
}
}

void handle_ls(std::string& argument)
{
  // Remove leading '-1' or any other flags before the actual path.
    // We remove '-1' or any numeric flags like '-n'
    size_t pos = 0;
    
    // Skip leading spaces or non-path flags like '-1' or '-'
    while (pos < argument.size() && (argument[pos] == '-' || std::isspace(argument[pos]))) {
        pos++;
    }

    // Check if the next characters form a valid numeric or flag-like pattern.
    if (pos < argument.size() && std::isdigit(argument[pos])) {
        // Remove any leading number if followed by a space or path separator
        while (pos < argument.size() && std::isdigit(argument[pos])) {
            pos++;
        }
    }
    
    // Now the string should have the remaining part of the path.
  argument = argument.substr(pos);
  argument=trim(argument);
  if(argument.empty())
  {
    for(const auto& entry: std::filesystem::directory_iterator(WORKING_DIRECTORY))
    {
      std::cout<<entry.path().filename()<<std::endl;
    }
  }
  else{
    if(is_path_exist(argument) && std::filesystem::is_directory(argument))
    {
      for(const auto& entry: std::filesystem::directory_iterator(argument))
      {
        std::cout<<entry.path().filename()<<std::endl;
      }
    }
    else if(is_path_exist(argument) && !std::filesystem::is_regular_file(argument))
    {
      std::cout<<argument<<std::endl;
    }
    else
    {
      std::cerr<<"ls: cannot access "<<"'"<<argument<<"'"<<": No such file or directory\n";
    }
  }
}

int detect_redirection(const std::string& argument)
{
  bool inside_single_quotes = false;
  bool inside_double_quotes = false;
  bool escape = false;

  for(int i=0; i<(int)argument.size(); i++)
  {
    char c = argument[i];
    if(c=='\"' && !escape)
    {
      inside_double_quotes = !inside_double_quotes;
    }
    if(c=='\'' && !escape)
    {
      inside_single_quotes = !inside_single_quotes;
    }
    if(c=='\\' && !escape)
    {
      escape = true;
    }
    if(c!='\\' && escape)
    {
      escape = false;
    }
    if(c=='>' && !inside_double_quotes && !inside_single_quotes && !escape)
    {
      return i;
    }
  }
  return -1;
}

std::pair<std::string,std::streambuf*> handle_redirection(const std::string& op, std::string file_name)
{
  std::string tmp_file_name="";
  if(file_name.empty())
  {
    return {"cout",std::cout.rdbuf()}; // set redirection to defualt output
  }
  //handle quoted file name
  if(file_name[0]== '\'' || file_name[0] =='\"')
  {
    std::vector<std::string> file_tokens = handle_quoting(file_name);
    
    for(const auto& token: file_tokens)
    {
      tmp_file_name+=token;
    }
    file_name = tmp_file_name;
  }
  //handle non quoted backslah in file name
  else if(file_name.find('\\')!= std::string::npos)
  {
    file_name = handle_non_quoted_backslash(file_name);
  }

  if(op == ">" || op == "1>")
  {
    file.open(file_name);
    if(file.is_open()){
    return {"cout",file.rdbuf()};

    }
    else{
    std::cerr<<file_name<<": No such file or directory\n";}
  }
  else if(op == ">>" || op == "1>>")
  {
  file.open(file_name,std::ios::app);
  if(file.is_open()){
    return {"cout",file.rdbuf()};
    }
    else
    std::cerr<<file_name<<": No such file or directory\n";
  }
  else if(op == "2>")
  {
    file.open(file_name);
    if(file.is_open())
    return {"cerr",file.rdbuf()};
    else
    std::cerr<<file_name<<": No such file or directory\n";
  }
  else if(op == "2>>")
  {
    file.open(file_name,std::ios::app);
    if(file.is_open())
    return {"cerr",file.rdbuf()};
    else
    std::cerr<<file_name<<": No such file or directory\n";

  }
  else
  {
    return {"cout",std::cout.rdbuf()};
  }

}



int main()
{
 
std::streambuf* cout_original_buf = std::cout.rdbuf(); 
std::streambuf* cerr_original_buf = std::cerr.rdbuf(); 


  
  
 

  // shell commands and descriptions
  std::unordered_map<std::string, std::string> commands;
  commands.emplace("type", "is a shell builtin");
  commands.emplace("echo", "is a shell builtin");
  commands.emplace("exit", "is a shell builtin");
  commands.emplace("pwd", "is a shell builtin");
  commands.emplace("cd", "is a shell builtin");
  commands.emplace("cat", "is /usr/bin/cat");
  commands.emplace("ls","is /usr/bin/ls");

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
    

    int redirection_index = detect_redirection(argument);
    if(redirection_index !=-1)
    {
      int start=redirection_index;
      int end=redirection_index;
      std::string op= std::string(1,argument[redirection_index]);
      std::string arg="";
      std::string file_name="";
      if(argument[redirection_index-1]=='1' || argument[redirection_index-1]=='2')
      {
        op = "";
        start = redirection_index-1;
        op = op + argument[start] + argument[redirection_index];
      }
      if(argument[redirection_index+1]=='>')
      {
        end = redirection_index+1;
        op = op + argument[redirection_index+1];
      }
      arg = argument.substr(0,start);
      file_name = argument.substr(end+1);
      arg = trim(arg);
      file_name = trim(file_name);
      argument = arg;
      std::pair<std::string, std::streambuf*> p = handle_redirection(op,file_name);
      if(p.first == "cout")
      {
        std::cout.rdbuf(p.second);
      }
      else if(p.first == "cerr")
      {
        std::cerr.rdbuf(p.second);
      }
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
          std::cerr<<argument<<": syntax error\n";
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
          
          std::string res = remove_extra_spaces(argument);
          std::cout << res << "\n";
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
          std::cerr << argument << ": not found\n";
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
        std::cerr<<argument<<": No such file or directory\n";
      }
      

    }
    else if(command=="cat" || command == "cat:")
    {
      handle_cat(argument);
    }
    else if(command == "ls" || command == "ls:")
    {
      handle_ls(argument);
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
        std::cerr<< programm_name << ": not found\n";
      }
    }
    std::cout.rdbuf(cout_original_buf);
    std::cerr.rdbuf(cerr_original_buf);
    file.close();
    

  }
}

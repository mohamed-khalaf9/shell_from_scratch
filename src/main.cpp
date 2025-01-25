
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
#include <conio.h>
#include "helpers.h"
#include "globals.h"















  

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








void handle_cat(const std::string& argument)
{
  bool append_new_line = false;
  if(!argument.empty()){
      std::vector<std::string>  file_paths;
      if(argument[0] == '\'' || argument[0] == '\"')
      {
      file_paths = handle_quoting(argument);
      
      if(file_paths.size()==1)
      { 
        append_new_line = true;
      }
      for(const auto& path : file_paths)
      {
        if(path == " " || path == "")
        {
          continue;
        }
        if(path!= " " && path!=""&& is_path_exist(path))
        {
          

          std::ifstream file(path);

          if(file.is_open()){
          std::string line;
          while(std::getline(file,line))
          {
            std::cout<<line;
            if(append_new_line)
            {
              std::cout<<std::endl;
              
            }
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
      if(file_paths.size()==1)
      { 
        append_new_line = true;
      
      }
        for(const auto& path : file_paths)
      {
        if(path==" " || path=="")
        {
          continue;
        }
        if(path!= " " && path!="" && is_path_exist(path))
        {
          

          std::ifstream file(path);

          if(file.is_open()){
          std::string line;
          while(std::getline(file,line))
          {
            std::cout<<line;
            if(append_new_line)
            {
              std::cout<<std::endl;
              
              
            }
          }
          file.close();
          }
          
          
          

        }
        else{
          std::cerr<<"cat: "<<path<<": No such file or directory\n";
        }
       

      }
  }
 
}
if(!append_new_line)
{
  std::cout<<std::endl;
}
}



void handle_ls(std::string& argument) {
    // Remove flags like '-1' and trim whitespace
    if(argument[0]=='-'){
    size_t pos = 0;
    while (pos < argument.size() && (argument[pos] == '-' || std::isspace(argument[pos]))) {
        pos++;
    }
    if (pos < argument.size() && std::isdigit(argument[pos])) {
        while (pos < argument.size() && std::isdigit(argument[pos])) {
            pos++;
        }
    }
    argument = argument.substr(pos);}

    argument = trim(argument); // Ensure trim() removes whitespace properly

    // Resolve the directory path
    std::string target_dir = argument.empty() ? WORKING_DIRECTORY : argument;

    // List directory entries (sorted)
    if (is_path_exist(target_dir) && std::filesystem::is_directory(target_dir)) {
        std::vector<std::string> entries;
        for (const auto& entry : std::filesystem::directory_iterator(target_dir)) {
            entries.push_back(entry.path().filename().string());
        }
        std::sort(entries.begin(), entries.end()); // Alphabetical sort
        
        for (const auto& entry : entries) {
            std::cout<< entry << std::endl; // Ensure each entry is on a new line
        }
    } else if (is_path_exist(target_dir) && std::filesystem::is_regular_file(target_dir)) {
        std::cout<< target_dir << std::endl;
        
    } else {
        std::cerr << "ls: " << argument << ": No such file or directory" << std::endl;
    }
}





void handle_echo(std::string& argument)
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

void handle_type(std::string& argument, std::unordered_map<std::string,std::string>& commands)
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

void handle_pwd()
{
  std::cout << WORKING_DIRECTORY << std::endl;
}




void run()
{
  std::streambuf* cout_original_buf = std::cout.rdbuf(); 
std::streambuf* cerr_original_buf = std::cerr.rdbuf(); 


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
  

 
  std::string input="";

  while (true)
  {
    std::cout << "$ ";
    input.clear();

    input = parse_input_with_autocomplete(input,trie);
   
    

    std::string command;
    std::string argument;

    std::pair<std::string,std::string> p = fetch_command_and_argument(input);
    command = p.first;
    argument = p.second;

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
  
   
    
    
   
    command = trim(command);

    if (command == "exit" )
    {
      exit(stoi(argument));
    }
    else if (command == "echo" || command == "echo:")
    {
      handle_echo(argument);
    }
    else if (command == "type" || command == "type:")
    {

      handle_type(argument,commands);
      
    }
    else if(command=="pwd" || command=="pwd:"){
      handle_pwd();
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

int main()
{
  
  

  run();
  return 0;

}

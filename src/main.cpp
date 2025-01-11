#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <filesystem>

std::vector<std::string> split(const std::string& s, char delimiter){
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream ss(s);
  
  while(std::getline(ss,token,delimiter))
  {
    tokens.push_back(token);
  }

  return tokens;
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // shell commands and descriptions
  std::unordered_map<std::string, std::string> commands;
  commands.emplace("type","is a shell builtin");
  commands.emplace("echo","is a shell builtin");
  commands.emplace("exit","is a shell builtin");
  
  // Uncomment this block to pass the first stage
  std::string input;


  while(true){
  std::cout << "$ ";
  std::getline(std::cin,input);
  std::stringstream ss(input);

  std::string command;
  std::string argument;
  ss >> command;

  std::getline(ss,argument);

  if(command == "exit")
  {
    exit(stoi(argument));
  }
  else if(command == "echo" || command == "echo:")
  {
    if(!argument.empty() && argument[0] == ' '){
      argument.erase(argument.begin());
      std::cout<<argument<<std::endl;
    }
    
  }
   else if(command == "type" || command == "type:")
  {
    if(!argument.empty() && argument[0] == ' '){
    argument.erase(argument.begin());
    }

    if(commands.count(argument))
    {
      std::cout<<argument<<" "<<commands[argument]<<std::endl;
    }
    else
    {
      // check if this is an executable file 
      //1-access path  environment variable
      const char* path = std::getenv("PATH");
      if(path)
      {
        bool is_found = false; // for checking if the executable file is found or not

        //2- split the path into tokens
        std::vector<std::string> dirs = split(path,':');
        //3- search for the executable file in the directories
        for(const auto& dir: dirs)
        {
         
          std::string full_path = dir + "/" + argument;
          

          if(std::filesystem::exists(full_path))
          {
            std::cout<<argument<<" is "<<full_path<<std::endl;
            is_found = true;
            break;
          }
        }

        if(!is_found)
        {
          std::cout<<argument<<": not found\n";
        }


      }
      else
      {
        std::cout<<argument<<": not found\n";
      }
     
      
    }
     





  }
  else{
  std::cout << command+": command not found\n";
  }


  }
}
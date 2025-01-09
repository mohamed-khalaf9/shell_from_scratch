#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unordered_map>

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
      std::cout<<argument<<": not found\n";
    }
     





  }
  else{
  std::cout << command+": command not found\n";
  }


  }
}
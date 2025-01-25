#include "helpers.h"



void autocomplete(std::string& input, Trie& trie)
{
  std::vector<std::string> suggestions = trie.searchPrefix(input);
  if(suggestions.size()==0)
  { 
    std::cout<<"\r$ "<<input;
    std::cout<<"\a";
  }
  if(suggestions.size()==1)
  {
    
    input = suggestions[0]+" ";
    
    std::cout<<"\r$ "<<input;
    
    
    
  }
  if(suggestions.size()>1)
  {
    std::cout<<std::endl;
    for(const auto& suggestion: suggestions)
    {
      
      std::cout<<suggestion<<std::endl;
    }
    input.clear();
    std::cout<<"$ ";
  }
}

std::string parse_input_with_autocomplete(std::string& input,Trie& trie,bool first_time)
{
   while(true)
    {

     
        char ch = _getch();

      if(ch=='\n' || ch==13)
      {
        std::cout<<std::endl;
        return input;
        
        
      }
      else if(ch=='\t'|| ch==9) 
      {
        autocomplete(input,trie);
        
        
      }
      else if(ch==127 || ch=='\b')
      {
        if(!input.empty())
        {
          input.pop_back();
          std::cout<<"\b \b";
        }

      }
      else
      {
        input+=ch;
        std::cout<<ch;
      }

      }
      
      
      
    

      
      return input;
  

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
bool is_path_exist(const std::string& path)
{
  return std::filesystem::exists(path);
}
void add_executables_to_trie(Trie& trie) 
{
    const char* path = std::getenv("PATH");
    if (path) {
        std::vector<std::string> dirs = split(path, ';');
        for (const auto& dir : dirs) {
            if(dir.empty() || !is_path_exist(dir))
            {
              continue;
            }
            for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                if (entry.is_regular_file() && entry.path().filename().string() != "shell") {
                    trie.insert(entry.path().filename().string());
                    
                }
            }
        }
    }
}

std::pair<std::string,std::string> fetch_command_and_argument(const std::string& input)
{
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
    
    return {command, argument};

}

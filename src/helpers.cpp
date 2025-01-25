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


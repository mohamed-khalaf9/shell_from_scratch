#include "helpers.h"
#include "globals.h"



char getch() {
    struct termios oldt, newt;
    char ch;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Set terminal to raw mode
    newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Read the character
    ch = getchar();

    // Restore the original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

void autocomplete(std::string &input, Trie &trie)
{
    std::vector<std::string> suggestions = trie.searchPrefix(input);
    if (suggestions.size() == 0)
    {
        std::cout << "\r$ " << input;
        std::cout << "\a";
    }
    if (suggestions.size() == 1)
    {

        input = suggestions[0] + " ";

        std::cout << "\r$ " << input;
    }
    if (suggestions.size() > 1)
    {
        std::cout << std::endl;
        for (const auto &suggestion : suggestions)
        {

            std::cout << suggestion << std::endl;
        }
        input.clear();
        std::cout << "$ ";
    }
}

std::string parse_input_with_autocomplete(std::string &input, Trie &trie, bool first_time)
{
    while (true)
    {

        char ch = getch();

        if (ch == '\n' || ch == 13)
        {
            std::cout << std::endl;
            return input;
        }
        else if (ch == '\t' || ch == 9)
        {
            autocomplete(input, trie);
        }
        else if (ch == 127 || ch == '\b')
        {
            if (!input.empty())
            {
                input.pop_back();
                std::cout << "\b \b";
            }
        }
        else
        {
            input += ch;
            std::cout << ch;
        }
    }

    return input;
}
std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    bool escape = false; 

    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];

        if (escape) {
            // Add the current character to the token, even if it's a delimiter.
            token += c;
            escape = false; 
        } else if (c == '\\') {
            escape = true;
        } else if (c == delimiter) {
            tokens.push_back(token);
            token.clear();
        } else {
            
            token += c;
        }
    }

    
    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

 
 std::optional<struct stat> is_path_exist(std::string &path)
{
    struct stat buffer;
    if(stat(path.c_str(), &buffer) == 0)
    return buffer;
    else return std::nullopt;
}
void add_executables_to_trie(Trie &trie)
{
    const char *path = std::getenv("PATH");
    if (path)
    {
        std::vector<std::string> dirs = split(path, ':');
        for (auto &dir : dirs)
        {
            std::optional<struct stat> path_stat = is_path_exist(dir);
            if (dir.empty() || path_stat == std::nullopt || !S_ISDIR(path_stat->st_mode))
            {
                continue;
            }
            for (const auto &entry : std::filesystem::directory_iterator(dir))
            {
                 if (entry.is_regular_file()) {
                    auto perms = entry.status().permissions();
                    if ((perms & std::filesystem::perms::owner_exec) != std::filesystem::perms::none) {
                        std::string filename = entry.path().filename().string();
                        if (filename != "shell") {
                            trie.insert(filename);
                        }
                    }
                }
            }
        }
    }
}

std::pair<std::string, std::string> fetch_command_and_argument(const std::string &input)
{
    std::string command;
    std::string argument;

    size_t pos = 0;

    // Skip leading spaces
    while (pos < input.size() && std::isspace(input[pos]))
    {
        ++pos;
    }

    // Check if the command is quoted
    if (input[pos] == '\'' || input[pos] == '\"')
    {
        char quote_char = input[pos]; // Store which quote we're using (single or double)
        command += input[pos++];      // Include the opening quote in the command

        // Collect the entire quoted string (including spaces inside the quotes)
        while (pos < input.size() && input[pos] != quote_char)
        {
            command += input[pos++];
        }

        if (pos < input.size())
        {
            command += input[pos++]; // Include the closing quote in the command
        }
    }
    else
    {
        // Collect the command until we hit a space (no quotes involved)
        while (pos < input.size() && !std::isspace(input[pos]))
        {
            command += input[pos++];
        }
    }

    // Skip the spaces after the command to collect the argument
    while (pos < input.size() && std::isspace(input[pos]))
    {
        ++pos;
    }

    // Collect the argument (if there's anything left)
    if (pos < input.size())
    {
        argument = input.substr(pos);
    }

    return {command, argument};
}
std::string handle_non_quoted_backslash(const std::string &argument)
{
    std::string res = "";
    if (argument.find('\\') != std::string::npos && argument[0] != '\'' && argument[0] != '\"')
    {
        for (const auto &c : argument)
        {
            if (c == '\\')
                continue;

            res += c;
        }
        return res;
    }
    else
        return argument;
}
std::vector<std::string> handle_double_quotes(const std::string &argument)
{
    std::stack<char> qoutes;
    std::vector<std::string> tokens;
    std::string token = "";
    bool inside_quotes = false;
    bool escape = false;
    bool prev_space = false;

    for (int i = 0; i < (int)argument.size(); i++)
    {
        char c = argument[i];

        if (qoutes.empty() && c == '\"' && !escape)
        {
            prev_space = false;
            if (token != "")
            {
                tokens.push_back(token);
                token.clear();
            }
            qoutes.push(c);
            inside_quotes = true;
            continue;
        }

        else if (!qoutes.empty() && c == '\"' && qoutes.top() == '\"' && !escape)
        {
            qoutes.pop();
            tokens.push_back(token);
            token.clear();
            inside_quotes = false;
            continue;
        }
        else if (c == '\\' && !escape && ((i + 1 < (int)argument.size()) && (argument[i + 1] == '\"' || argument[i + 1] == '\\' || argument[i + 1] == '$')))
        {
            escape = true;
            continue;
        }
        else if (c == '\\' && !escape)
        {
            token += c;
            continue;
        }
        else if (c == '\\' || c == '$' || c == '\"' && escape)
        {

            token += c;
            escape = false;
            continue;
            // for future handling of specail characters using else
        }
        // Handle spaces outside quotes (normalize them to one space)
        else if (isspace(c) && !inside_quotes)
        {
            if (!prev_space)
            { // Only add one space if the previous character wasn't a space
                if (!token.empty())
                {
                    tokens.push_back(token);
                    token.clear();
                }
                token += " ";      // Add exactly one space
                prev_space = true; // Set the flag to indicate a space has been added
            }
            continue;
        }
        prev_space = false;
        token += c;
    }
    tokens.push_back(token);

    if (!qoutes.empty())
    {
        tokens.clear();
        return tokens;
    }

    return tokens;
}
std::vector<std::string> handle_single_qoutes(const std::string &argument)
{
    std::stack<char> qoutes;
    std::vector<std::string> tokens;
    std::string token = "";
    bool inside_qoutes = false;

    for (auto c : argument)
    {
        if (qoutes.empty() && c == '\'')
        {
            if (token != "")
            {
                tokens.push_back(token);
                token.clear();
            }
            qoutes.push(c);
            inside_qoutes = true;
            continue;
        }

        else if (!qoutes.empty() && c == '\'' && qoutes.top() == '\'')
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
    if (!qoutes.empty())
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
    if (argument[0] == '\'')
    {
        tokens = handle_single_qoutes(argument);
        return tokens;
    }
    else if (argument[0] == '\"')
    {
        tokens = handle_double_quotes(argument);
        return tokens;
    }
    else
        return tokens;
}
std::unordered_map<std::string, std::string> load_commands()
{
    std::unordered_map<std::string, std::string> commands;
    commands.emplace("type", "is a shell builtin");
    commands.emplace("echo", "is a shell builtin");
    commands.emplace("exit", "is a shell builtin");
    commands.emplace("pwd", "is a shell builtin");
    commands.emplace("cd", "is a shell builtin");
    commands.emplace("cat", "is /usr/bin/cat");
    commands.emplace("ls", "is /usr/bin/ls");

    return commands;
}

std::string trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" ");
    size_t last = str.find_last_not_of(" ");
    return (first == std::string::npos || last == std::string::npos)
               ? ""
               : str.substr(first, last - first + 1);
}

int detect_redirection(const std::string &argument)
{
    bool inside_single_quotes = false;
    bool inside_double_quotes = false;
    bool escape = false;

    for (int i = 0; i < (int)argument.size(); i++)
    {
        char c = argument[i];
        if (c == '\"' && !escape)
        {
            inside_double_quotes = !inside_double_quotes;
        }
        if (c == '\'' && !escape)
        {
            inside_single_quotes = !inside_single_quotes;
        }
        if (c == '\\' && !escape)
        {
            escape = true;
        }
        if (c != '\\' && escape)
        {
            escape = false;
        }
        if (c == '>' && !inside_double_quotes && !inside_single_quotes && !escape)
        {
            return i;
        }
    }
    return -1;
}

std::pair<std::string, std::streambuf *> handle_redirection(const std::string &op, std::string file_name)
{
    std::string tmp_file_name = "";
    if (file_name.empty())
    {
        return {"cout", std::cout.rdbuf()}; // set redirection to defualt output
    }
    // handle quoted file name
    if (file_name[0] == '\'' || file_name[0] == '\"')
    {
        std::vector<std::string> file_tokens = handle_quoting(file_name);

        for (const auto &token : file_tokens)
        {
            tmp_file_name += token;
        }
        file_name = tmp_file_name;
    }
    // handle non quoted backslah in file name
    else if (file_name.find('\\') != std::string::npos)
    {
        file_name = handle_non_quoted_backslash(file_name);
    }

    if (op == ">" || op == "1>")
    {
        file.open(file_name, std::ios::trunc);
        if (file.is_open())
        {
            return {"cout", file.rdbuf()};
        }
        else
        {
            std::cerr << file_name << ": No such file or directory\n";
        }
    }
    else if (op == ">>" || op == "1>>")
    {
        file.open(file_name, std::ios::app);
        if (file.is_open())
        {
            return {"cout", file.rdbuf()};
        }
        else
            std::cerr << file_name << ": No such file or directory\n";
    }
    else if (op == "2>")
    {
        file.open(file_name, std::ios::trunc);
        if (file.is_open())
            return {"cerr", file.rdbuf()};
        else
            std::cerr << file_name << ": No such file or directory\n";
    }
    else if (op == "2>>")
    {
        file.open(file_name, std::ios::app);
        if (file.is_open())
            return {"cerr", file.rdbuf()};
        else
            std::cerr << file_name << ": No such file or directory\n";
    }
    else
    {
        return {"cout", std::cout.rdbuf()};
    }
    return {"cout", std::cout.rdbuf()};
}

std::string is_executable_file_exists_in_path(const std::string &file_name)
{
    const char *path = std::getenv("PATH");
    if (path)
    {

        std::vector<std::string> dirs = split(path, ':');

        for (const auto &dir : dirs)
        {
            try{
            for(const auto &file : std::filesystem::directory_iterator(dir))
            {
                if(file.path().filename().string()== file_name)
                {
                    struct stat file_stat;
                    if(stat(file.path().c_str(), &file_stat)==0)
                    {
                        if(file_stat.st_mode & S_IXUSR)
                        {
                            return file.path().string();
                        }
                    }
                }


            }
            }
            catch(const std::filesystem::filesystem_error &e)
            {
                std::cout<<e.what()<<std::endl;
            }
            
        }
        return "";
    }
    else
        return "";
}

bool handle_relative_path(std::vector<std::string> &path_tokens)
{
    for (const auto &token : path_tokens)
    {
        if (token == "..")
        {
            // Move up one level: pop the last directory from WORKING_DIRECTORY
            if (!WORKING_DIRECTORY.empty())
            {
                size_t pos = WORKING_DIRECTORY.find_last_of('/');
                if (pos != std::string::npos)
                {
                    WORKING_DIRECTORY = WORKING_DIRECTORY.substr(0, pos);
                }
            }
        }
        else if (token != "." && token != "")
        {
            // For any other valid directory name, add it to WORKING_DIRECTORY
            if (WORKING_DIRECTORY.back() != '/')
            {
                WORKING_DIRECTORY += '/';
            }
            WORKING_DIRECTORY += token;
        }
    }

    // After handling all tokens, ensure the path is valid
    std::optional<struct stat> path_stat = is_path_exist(WORKING_DIRECTORY);
    return path_stat != std::nullopt;
}

std::string remove_last_token_from_working_directory(const std::string &workingDirectory)
{
    std::string updatedDirectory = workingDirectory; // Make a copy

    // Find the position of the last '/'
    std::size_t pos = updatedDirectory.find_last_of('/');

    // If '/' is found and it's not the only character (e.g., root '/')
    if (pos != std::string::npos && pos > 0)
    {
        // Erase everything from the last '/' to the end
        updatedDirectory.erase(pos);
    }
    else if (pos == 0)
    {
        // If we are at the root '/', set it to "/"
        updatedDirectory = "/";
    }

    return updatedDirectory; // Return the updated directory
}

std::string remove_extra_spaces(const std::string &str)
{
    std::stringstream ss(str);
    std::string word, result;

    // Read each word and append with a single space
    while (ss >> word)
    {
        if (!result.empty())
        {
            result += " ";
        }
        result += word;
    }

    return result;
}

bool change_directory(const std::string &path)
{
    std::string trimmed_path = trim(path);
    if(trimmed_path.empty())
    {
        return false;
    }
    if (trimmed_path == "~")
    {
        WORKING_DIRECTORY = std::getenv("HOME");
        if(chdir(WORKING_DIRECTORY.c_str())==0)
        return true;
        else return false;
    }
    std::vector<std::string> path_tokens = split(trimmed_path, '/');

    if (path_tokens.size() == 0)
    {
        return false;
    }
    else if (path_tokens[0] == "." || path_tokens[0] == "..")
    {
        // handel relative path
        if (handle_relative_path(path_tokens))
        {
            if(chdir(WORKING_DIRECTORY.c_str())==0)
            return true;
            else return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        std::optional<struct stat> path_stat = is_path_exist(trimmed_path);
        if (path_stat != std::nullopt && S_ISDIR(path_stat->st_mode))
        {

            WORKING_DIRECTORY = trimmed_path;
            if(chdir(WORKING_DIRECTORY.c_str())==0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
            return false;
    }
}

std::tuple<std::string, std::string, std::string> parse_redirection(const std::string &argument, int redirection_index)
{
    int start = redirection_index;
    int end = redirection_index;
    std::string op = std::string(1, argument[redirection_index]);
    std::string arg = "";
    std::string file_name = "";

    if (argument[redirection_index - 1] == '1' || argument[redirection_index - 1] == '2')
    {
        op = "";
        start = redirection_index - 1;
        op = op + argument[start] + argument[redirection_index];
    }
    if (argument[redirection_index + 1] == '>')
    {
        end = redirection_index + 1;
        op = op + argument[redirection_index + 1];
    }
    arg = argument.substr(0, start);
    file_name = argument.substr(end + 1);
    arg = trim(arg);
    file_name = trim(file_name);

    return std::make_tuple(arg, op, file_name);
}
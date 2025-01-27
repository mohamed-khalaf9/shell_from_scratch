

### **File: `main.cpp`**  
**Core Functions and Logic** 

1. **`void handle_cat(const std::string &argument)`** 
   - **Purpose**: Outputs the contents of files specified in `argument`. 
   - **Key Details**: 
     - **Quoted Path Handling**: Uses `handle_quoting()` to split paths enclosed in `'` or `"`, allowing spaces in filenames (e.g., `cat 'file name.txt'`). 
     - **Absolute Path Resolution**: Converts relative paths to absolute using `std::filesystem::absolute()`. 
     - **Validation**: Checks if the path exists and is a regular file (`std::filesystem::is_regular_file`). 
     - **Error Handling**: Prints `cat: <path>: No such file` if the file is invalid. 
     - **Edge Cases**: Skips empty or space-only paths after splitting. 
     
2. **`void handle_ls(std::string &argument)`** 
   - **Purpose**: Lists directory entries or prints a file path. 
   - **Key Details**: 
     - **Argument Sanitization**: Trims spaces and resolves backslashes (e.g., `ls my\ dir` → `ls my dir`). 
     - **Target Directory**: Defaults to `WORKING_DIRECTORY` if no argument is provided. 
     - **Output Format**: Directories are suffixed with `/` (e.g., `documents/`). 
     - **Sorting**: Entries are sorted lexicographically for consistency. 
     - **Error Handling**: Fails if the path is neither a directory nor a file. 

3. **`void handle_echo(std::string &argument)`** 
   - **Purpose**: Prints arguments with proper escaping and spacing. 
   - **Key Details**: 
     - **Quoted Arguments**: Preserves spaces and special characters inside quotes (e.g., `echo "Hello   World"` → `Hello   World`). 
     - **Backslash Escapes**: Removes unquoted backslashes (e.g., `echo Hello\ World` → `Hello World`). 
     - **Space Normalization**: Collapses multiple spaces outside quotes (e.g., `echo   Hello` → `Hello`). 

4. **`void handle_type(std::string &argument, ...)`** 
   - **Purpose**: Identifies if a command is a built-in or external. 
   - **Key Details**: 
     - **Built-in Check**: Searches the `commands` map (e.g., `type echo` → `echo is a shell builtin`). 
     - **External Check**: Uses `is_executable_file_exists_in_path()` to search `PATH` (e.g., `type ls` → `ls is /usr/bin/ls`). 
     - **Quoting Support**: Handles quoted command names (e.g., `type 'my cmd'`). 

5. **`void execute_Programm(std::string &command, ...)`** 
   - **Purpose**: Executes external programs via `fork()` and `execv()`. 
   - **Key Details**: 
     - **Executable Resolution**: Uses `is_executable_file_exists_in_path()` to find the full path (e.g., `firefox` → `/usr/bin/firefox`). 
     - **Argument Handling**: Converts `programm_argument` into a `char*` array for `execv()`. 
     - **Process Management**: 
       - `fork()` creates a child process. On success, `execv()` replaces the child’s memory with the program. 
       - The parent waits for the child using `waitpid()` and reports exit statuses (e.g., `Programm exited with status: 0`). 
     - **Security Note**: `const_cast<char*>` is used to comply with `execv`’s `char* const argv[]` requirement. 

6. **`void run()`** 
   - **Purpose**: Main shell loop with autocompletion and redirection. 
   - **Key Details**: 
     - **Autocompletion Setup**: Initializes a `Trie` with built-ins (e.g., `echo`, `cd`) and executables from `PATH` (via `add_executables_to_trie()`). 
     - **Input Parsing**: Uses `parse_input_with_autocomplete()` to handle `Tab` keypresses and input editing (backspace support). 
     - **Redirection Workflow**: 
       1. `detect_redirection()` finds `>` or `>>` operators, ignoring quoted sections. 
       2. `parse_redirection()` splits the input into argument, operator, and filename.
       3. `handle_redirection()` redirects `stdout`/`stderr` to a file (e.g., `echo "test" > out.txt`). 
     - **Command Routing**: Uses `fetch_command_and_argument()` to split input into `command` and `argument`, then dispatches to handlers. 

---

### **File: `helpers.cpp`**  
**Critical Functions and Implementation** 

1. **`std::vector<std::string> split(const std::string &s, char delimiter)`** 
   - **Purpose**: Splits a string by `delimiter`, ignoring escaped delimiters (e.g., `split("a\\ b", ' ')` → `["a b"]`). 
   - **Mechanism**: 
     - Uses an `escape` flag to skip processing the next character. 
     - Preserves escaped delimiters (e.g., `\ ` becomes space). 

2. **`std::vector<std::string> handle_double_quotes(...)`** 
   - **Purpose**: Parses double-quoted arguments, allowing escaped `"`, `\`, and `$` (e.g., `"Hello \"World\""` → `["Hello "World""]`). 
   - **Stack-Based Tracking**: Uses a stack to track nested quotes and ensure proper pairing. 

3. **`std::string is_executable_file_exists_in_path(...)`** 
   - **Purpose**: Resolves an executable’s full path using the `PATH` environment variable. 
   - **Steps**: 
     1. Splits `PATH` into directories (e.g., `/usr/bin:/bin` → `["/usr/bin", "/bin"]`). 
     2. Checks each directory for a file matching `file_name` with execute permissions (`S_IXUSR`). 

4. **`bool change_directory(const std::string &path)`** 
   - **Purpose**: Updates `WORKING_DIRECTORY` and navigates to the new path. 
   - **Logic**: 
     - Handles `~` by resolving to the `HOME` environment variable. 
     - Processes `.` (current dir), `..` (parent dir), and relative paths (e.g., `docs/../src` → `src`). 
     - Validates the path using `is_path_exist()` and updates `WORKING_DIRECTORY` on success. 

5. **`std::tuple<std::string, ...> parse_redirection(...)`** 
   - **Purpose**: Extracts redirection operator and filename from input. 
   - **Operator Detection**: 
     - Handles `>`, `>>`, `2>`, and `2>>` (e.g., `2>>` appends stderr to a file). 
     - Ignores operators inside quotes (e.g., `echo ">"` is not a redirection). 

---

### **File: `trie.h`**  
**Trie Structure and Autocompletion** 

1. **`TrieNode` Class** 
   - **Members**: 
     - `children`: A hash map linking characters to child nodes. 
     - `isEndOfWord`: Flag marking the end of a valid command (e.g., `echo` ends at 'o'). 

2. **`Trie` Class** 
   - **Methods**: 
     - **`insert()`**: Adds a command to the trie (e.g., inserting `ls` creates nodes for `l` → `s`). 
     - **`searchPrefix()`**: Returns all commands starting with a prefix (e.g., `se` → `["set", "search"]`). 
   - **Autocompletion Workflow**: 
     - On `Tab` press, `parse_input_with_autocomplete()` calls `autocomplete()`, which uses `trie.searchPrefix()` to suggest completions. 

---

### **File: `globals.cpp`**  
**Global State Management** 

1. **`std::string WORKING_DIRECTORY`** 
   - **Initialization**: Set to the current working directory at shell startup. 
   - **Modification**: Updated by `handle_cd()` via `change_directory()`. 

2. **`std::ofstream file`** 
   - **Redirection Handling**: 
     - Opened in `handle_redirection()` for `>` (truncate) or `>>` (append). 
     - Closed after each command execution in `run()`. 

---

### **Edge Cases and Error Handling**  

1. **Invalid Paths** 
   - `handle_cat` and `handle_ls` validate paths using `std::filesystem::exists()` and print descriptive errors. 

2. **Malformed Quotes** 
   - `handle_quoting()` returns an empty vector for unmatched quotes, triggering syntax errors (e.g., `echo "Hello` → `syntax error`). 

3. **Failed Process Execution** 
   - `execute_Programm()` checks `execv()` return values and prints `execv failed` on errors. 

---

### **Key Design Decisions**  

1. **Autocompletion Trie** 
   - Populated at startup with built-ins and executables for real-time suggestions. 

2. **Redirection Buffering** 
   - Uses `std::cout.rdbuf()` to swap output buffers, allowing seamless redirection without modifying `std::cout` calls. 

3. **Quoting and Escaping** 
   - Uniform handling across commands ensures consistency (e.g., `echo`, `cat`, and `ls` all use `handle_quoting()`). 

---

### **Known Limitations**  

1. **No Pipeline Support** 
   - Commands like `ls | grep .txt` are not supported. 

2. **Limited Signal Handling** 
   - Interrupts (e.g., `Ctrl+C`) are not explicitly handled. 

---

### **Example Workflow**  

**Input**: `$ echo "Hello $USER" > greeting.txt` 
1. **fetch commmand and argument**: echo and "Hello world" > greeting.txt
1. **Redirection Detection**: `detect_redirection()` finds `>` at position 12. 
2. **Parsing**: `parse_redirection()` splits into argument `"Hello world"`, operator `>`, and filename `greeting.txt`. 
3. **Redirection**: `handle_redirection()` opens `greeting.txt` and redirects `stdout`. 
4. **Execution**: `handle_echo()` prints `Hello $USER` to the file. 


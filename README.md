# Shell_from_scratch
# Overview
A lightweight, feature-rich custom shell built for **Linux (Ubuntu)** that supports various commands, quoting mechanisms, redirection, and a robust auto-completion system powered by a custom trie data structure. The shell provides essential functionality for users to interact with the file system, handle text processing,run a program that is located in the system's PATH,and navigate directories.
# Features
**1. Supported Commands:**
 - **echo**: Displays input text, supports single, and double quotes and non-quoted backslash for arguments.
 - **ls**:  Lists directory contents, supports single, and double quotes and non-quoted backslash for paths.
 - **pwd**: Prints the current working directories.
 - **cd**: Changes the current directory, supports absolute paths, absolute paths with quoting and non-quoted backslash, and relative paths.
 - **cat**: Displays the content of files, supports multiple file names or custom file names with quoting and non-quoted backslash in file names.
 - **type**: Checks if a given name is a built-in command or an executable file, and supports arguments with quoting.
   
 **2. Run Program:**
   - Run a program that is located in the system's PATH by typing the program name.

 **3. Quoting mechanisms:**
 - **Single quotes ('):** Preserves literal text within the quotes.
 - **Double quotes ("):** preserves literal text within the quotes as well supports handling for backslash followed by ($,\,",',n) within the quotes.
 - **Non-Quoted-Backslash handling(\)**: preserves the literal value of the character following the backslash.

**4. Redirection:**
- **Output redirection (> and >>):**
  - \> or 1>: Overwrites the output to the specified file, if not exist, creates it.
  - \>\> or 1\>\>: Appends the output to the specified file, if not exist, creates it.
- **Error redirection (2> and 2>>):**
   - 2>: Overwrites error messages to the specified file, if not exist, create it.
   - 2>\>: Appends error messages to the specified file, if not exist, create it.
  
**5. Autocompletion For Commands and Executables:**
   - write some characters and Press **TAB**:
       - if only one match is found it will replace the current input.
       - if multiple matches are found, the shell displays them and waits for an input.
       - if no matches are found, the shell will preserve the current input.
    
  # Technical Details:
   - 
  
     
         




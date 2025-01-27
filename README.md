# Shell_from_scratch
## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Technical Details](#technical-details)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Learning Outcomes](#learning-outcomes)

## Overview
A lightweight, feature-rich custom shell built for **Linux (Ubuntu)** that supports various commands, quoting mechanisms, redirection, and a robust auto-completion system powered by a custom trie data structure. The shell provides essential functionality for users to interact with the file system, handle text processing, run a program that is located in the system's PATH, and navigate directories.
## Features
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
  - \> or 1>: Overwrites the output to the specified file, if it does not exist, creates it.
  - \>\> or 1\>\>: Appends the output to the specified file, if it does not exist, creates it.
- **Error redirection (2> and 2>>):**
   - 2>: Overwrites error messages to the specified file, if it does not exist, create it.
   - 2>\>: Appends error messages to the specified file, if it does not exist, create it.
  
**5. Autocompletion For Commands and Executables:**
   - Type some characters and Press **TAB**:
       - if only one match is found it will replace the current input.
       - if multiple matches are found, the shell displays them and waits for an input.
       - if no matches are found, the shell will preserve the current input.
    
  ## Technical Details
   - **Programming languages:** C++
   - **Source Code Documentation:** [Documentation File](./shell-source-code-documentation.md)
   - **Trie Data Structure:** The trie is used to store all commands and executable file names for fast prefix-based lookups.

  ## Installation
  1. **Prerequisites:**
     - **Linux (Ubuntu)**: This project is designed to run on Ubuntu-based Linux systems.
     - **g++ Compiler**: You’ll need the g++ compiler to compile the source code. If it's not already installed, you can install it by running the following commands:
       ```
       sudo apt update
       sudo apt install g++
       ```  
  2. **Steps:**
     - Clone the repository: ```git clone https://github.com/mohamed-khalaf9/shell_from_scratch.git```
     - Navigate to the project directory: ```cd <project_dir>```
     - Compile the shell: ```g++ -o shell src/*.cpp```
     - Run the shell: ```./shell```
       
   ## Usage
   - **echo**
```
$ echo 'hello world'
hello world
$ echo "hello \$world "
hello $world 
$
```
    
   - **ls**
```
$ ls
globals.cpp
globals.h
helpers
helpers.cpp
helpers.h
main
main.cpp
trie.h
$ ls /home/mohamed-khalaf/test
shell_from_scratch/
$
```
- **pwd**
```
$ pwd
/home/mohamed-khalaf/shell_from_scratch/src
```
- **cd**
```
$ cd ..
$ pwd
/home/mohamed-khalaf/shell_from_scratch
$ cd ./src
$ pwd
/home/mohamed-khalaf/shell_from_scratch/src
$ cd /home/mohamed-khalaf 
$ pwd
/home/mohamed-khalaf
$
```
- **output and Error redirection**
```
$ echo hello world > hello.txt
$ ls error 2>> hello.txt
$
```
- **cat**
```
$ cat /home/mohamed-khalaf/shell_from_scratch/src/hello.txt
hello world
ls: error: No such file or directory

$
```
- **Run program exists in system's PATH:**
  
```
  $ hello
  hello world! 
  Programm exited with status: 0
  $
```
- **type**

```
$ type echo
echo is a shell builtin
$ tyep type
tyep: not found
$ type type
type is a shell builtin
$ type hello
hello is /usr/local/bin/hello
$
```
- **auto-completion with multiples matches**

 ```
  $ hel <TAB>
  helpztags
  hello
  $
 ``` 
  
> [!NOTE]
> You can test the auto-completion feature by typing some characters and pressing TAB. Additionally, you can explore the shell and test more commands in detail, or find bugs :).

## Contributing
  You can contribute with: **Code Refactoring - Bug fixing - New Features - Code Documentation**  
**Steps:**
1. Fork the repository and clone it locally.  
2. Create a branch for your feature or bug fix.  
3. Ensure your code follows the project's style and is well-documented.  
4. Test your changes thoroughly.  
5. Submit a pull request with a detailed explanation of your work.  

For questions or discussions, feel free to open an issue.  


## License
This project is licensed under the MIT License. You are free to use, modify, and distribute this software as long as the original license is included. See the [LICENSE](LICENSE) file for more details.

## Learning Outcomes

- **System-Level Programming**
    - Gained hands-on experience with Linux-specific system calls like fork, exec, dup2, stat, and wait, improving my understanding of process creation and management.
    - First-time practical exposure to Linux internals and shell environments.

- **Operating Systems Knowledge**
    - Strengthened my understanding of key OS concepts such as input/output redirection, process management, and file descriptors.

- **Problem-Solving and Debugging**
    - Tackled numerous bugs and edge cases, especially while implementing quoting, backslashes, and redirection.
    - Developed a deeper appreciation for debugging strategies and testing edge cases systematically.

- **Designing Solutions**
    - Designed and implemented efficient solutions for challenging features:
      - Quoting Mechanisms: Handled input parsing for single and double quotes with escape sequences.
      - Autocompletion: Leveraged a custom trie data structure for fast prefix-based search.
      - Redirection: Built a modular redirection handler to support overwriting and appending outputs and errors.

- **Code Refactoring**
    - Refactored the entire codebase after the first implementation, improving modularity, readability, and scalability.

- **Coding Skills**
    - Wrote a significant amount of code for the project, which sharpened my logical thinking and coding efficiency.
    - Improved my proficiency in C++.


   



  
     
         
   
   
  
     
         




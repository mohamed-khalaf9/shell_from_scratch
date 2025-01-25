#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>


std::string WORKING_DIRECTORY = std::filesystem::current_path().string();
std::ofstream file; // for redirection

#endif
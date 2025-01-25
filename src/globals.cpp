#include "globals.h"

std::string WORKING_DIRECTORY = std::filesystem::current_path().string();
std::ofstream file; // for redirection
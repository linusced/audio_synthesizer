#ifndef FILES
#define FILES

#include <CoreServices/CoreServices.h>
#include <string>
#include <filesystem>
#include <fstream>

std::string filePath(std::string relativePath);

std::string applicationSupportPath(std::string relativePath);

#endif
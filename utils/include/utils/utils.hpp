#pragma once

#include <vector>
#include <string>

__declspec(dllexport) void ReadEntireFileIntoVector(char const* path, std::vector<std::string>& lines);


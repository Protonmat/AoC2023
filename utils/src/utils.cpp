#include <iostream>
#include <fstream>

#include "utils/utils.hpp"

void ReadEntireFileIntoVector(char const* path, std::vector<std::string>& lines) {
    if (!path) {
        return;
    }

    std::ifstream input(path, std::ios::in);

    if(!input.is_open()) {
        std::printf("Error opening file %s\n", path);
        return; 
    }
    
    for(std::string v; std::getline(input, v);) lines.push_back(v);
}
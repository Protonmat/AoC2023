#include <array>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

// 53934 is not the answer
// 53868 is the right answer for the second challenge

size_t getnumbers(std::string &codeLine) {
    if(codeLine.size() == 0) return 0;
    size_t              first{0}, last{0};
    std::string         numbers[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    std::vector<size_t> nums;
    nums.reserve(codeLine.length());

    char *ptrLine = codeLine.data();
    while(*ptrLine != '\0') {
        if(*ptrLine > '0' && *ptrLine <= '9') {
            nums.push_back((size_t)*ptrLine - '0');
            ++ptrLine;

        } else {
            auto number{1};
            auto found{false};
            for(auto &num : numbers) {
                auto index{0u};
                while((ptrLine + index) && index != num.length() && num[index] == *(ptrLine + index)) ++index;
                if(num.length() == index) {
                    nums.push_back(number);
                    ptrLine += index - 1;
                    found = true;
                    // ptrLine += index;
                }
                ++number;
            }

            if(!found) {
                ++ptrLine;
            }
        }
    }

    first = *nums.begin();
    last  = *(nums.begin() + (nums.size() - 1));

    return 10 * first + last;
}

int main(int argc, char const *argv[]) {
    std::array<std::string, 1000u> inputs{};
    std::ifstream                  input("input.txt", std::ios::in);

    if(!input.is_open()) {
        std::printf("Error opening file\n");
        return 0;
    }

    size_t index{0};
    while(!input.eof()) {
        input >> inputs[index];
        ++index;
    }
    size_t sum{0};
    size_t counter{0};
    for(auto &in : inputs) {
        size_t number{getnumbers(in)};
        std::printf("Value for : %03zu) %s -> %zu\n", ++counter, in.c_str(), number);
        sum += number;
    }

    std::printf("Final result is %zu\n", sum);

    /* code */
    return 0;
}

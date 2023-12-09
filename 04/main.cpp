#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <format>
#include <map>
#include <algorithm>
#include <execution>

void ReadEntireFileIntoVector(char const *path, std::vector<std::string> &lines) {
    if(!path) {
        return;
    }

    std::ifstream input(path, std::ios::in);

    if(!input.is_open()) {
        std::printf("Error opening file %s\n", path);
        return;
    }

    for(std::string v; std::getline(input, v);) lines.push_back(v);
}

struct Card {
    size_t id{0};
    std::map<size_t, size_t> numbers;
    size_t result{0};
};

constexpr std::istream &operator>>(std::istream &is, Card &card) {
    std::string str_void{};
    
    is >> str_void >> card.id >> str_void;
    size_t key_{0};

    while(is >> key_) {
        std::printf("Printing ... %zu\n", key_);;
        card.numbers[key_] = 0;
    }

    if (is.fail()) {
        is.clear();
        is >> str_void;
    }
    
    key_ = 0;
    size_t times {0};
    while(is >> key_) {
        std::printf("Finding ... %zu\n", key_);
        if ( card.numbers.find(key_) != card.numbers.end() ) {
            ++times;
        }
    }

    card.result = times < 2 ? times :  1ul << (times - 1);
    return is;
}


constexpr std::ostream &operator <<(std::ostream &os, Card &card ){ 
    std::cout << std::format("Card: {}\n", card.id);
    return os;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[]) {
    constexpr size_t         test_lines_count {6};
    constexpr size_t         current_lines_count{220};
    std::vector<std::string> lines;
    std::vector<std::string> test_lines;
    lines.reserve(current_lines_count);
    test_lines.reserve(test_lines_count);

    ReadEntireFileIntoVector("input.txt", lines);
    ReadEntireFileIntoVector("test_input.txt", test_lines);
    std::printf("Lines %zu\n", lines.size());
    
    size_t test_final_result{0};
    size_t final_result{0};


    std::for_each(std::execution::par, begin(test_lines), end(test_lines), [&test_final_result]( auto &line ) {
        std::stringstream ss(line);
        Card card;
        ss >> card;
        test_final_result += card.result;
    });


    std::for_each(std::execution::par, begin(lines), end(lines), [&final_result]( auto &line ) {
        std::stringstream ss(line);
        Card card;
        ss >> card;
        final_result += card.result;
    });

    std::printf("Test result is %zu\n", test_final_result);
    std::printf("Final result is %zu\n",  final_result);
    /* code */
    return 0;
}

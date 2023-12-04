#include <array>
#include <vector>
#include <fstream>
#include <iostream>
#include <format>
#include <string>
#include <sstream>

#include <thread>
#include <mutex>
#include <execution>
#include <algorithm>

std::mutex g_pages_mutex;

struct Position {
    size_t initialPosition_{0};
    size_t finalPosition_{0};
};

struct Symbol {
    char     symbol_{0};
    Position position_{0, 0};
};

struct Number {
    size_t   value_{0};
    Position position_{0, 0};
};

struct Line {
    size_t              lineN{0};
    std::vector<Number> numbers_;
    std::vector<Symbol> symbols_;
};

constexpr std::istream &operator>>(std::istream &is, Number &number) {
    char p = is.peek();
    if(p == 0 || is.eof()) {
        return is;
    }

    if(p >= '0' && p <= '9') {
        size_t cur = is.tellg();
        is >> number.value_;
        size_t end       = ((size_t)is.tellg() > 140) ? 140u : (size_t)is.tellg();
        number.position_ = Position{cur, end};
    }
    return is;
};

constexpr std::istream &operator>>(std::istream &is, std::vector<Number> &numbers) {
    char p = is.peek();
    if(p == 0 || is.eof()) {
        return is;
    }

    if(p >= '0' && p <= '9') {
        Number number;
        is >> number;
        numbers.push_back(number);
    }
    return is;
};

constexpr std::istream &operator>>(std::istream &is, Symbol &symbol) {
    char p = is.peek();

    if(p == 0 || is.eof()) {
        return is;
    }

    if(p == '.' || (p >= '0' && p <= '9')) {
        return is;
    }

    size_t cur = is.tellg();
    is >> symbol.symbol_;
    size_t end       = ((size_t)is.tellg() > 140) ? 140u : (size_t)is.tellg();
    symbol.position_ = Position{cur, end};

    return is;
};

constexpr std::istream &operator>>(std::istream &is, std::vector<Symbol> &symbols) {
    char p = is.peek();

    if(p == 0 || is.eof()) {
        return is;
    }

    if(p == '.' || (p >= '0' && p <= '9')) {
        return is;
    }
    Symbol symbol;
    is >> symbol;
    symbols.push_back(symbol);
    return is;
};

constexpr std::istream &operator>>(std::istream &is, Line &line) {
    while(is.good()) {
        while(is.peek() == '.') is.get();
        if(!is.good()) {
            continue;
        }
        is >> line.numbers_;
        is >> line.symbols_;
    }

    return is;
};

constexpr std::ostream &operator<<(std::ostream &os, Number &number) {
    std::cout << std::format("Number ({}, {}) {}\n", number.position_.initialPosition_, number.position_.finalPosition_,
                             number.value_);
    return os;
}

constexpr std::ostream &operator<<(std::ostream &os, Symbol &symbol) {
    std::cout << std::format("Symbol ({}, {}) {}\n", symbol.position_.initialPosition_, symbol.position_.finalPosition_,
                             symbol.symbol_);
    return os;
}

constexpr std::ostream &operator<<(std::ostream &os, Line &line) {
    os << std::format("Parsing ({}) \n", line.lineN);

    for(auto &number : line.numbers_) {
        os << "\t" << number;
    }

    for(auto &symbol : line.symbols_) {
        os << "\t" << symbol;
    }

    return os;
};

Line Parse(std::string &data, size_t dataN) {


    std::stringstream ss(data);
    Line              line{.lineN = dataN};
    ss >> line;

    std::cout << line;

    return line;
};

int main(int argc, char const *argv[]) {
    std::ifstream input("input.txt", std::ios::in);

    if(!input.is_open()) {
        std::printf("Error opening file\n");
        return 0;
    }

    std::vector<std::string> inputs;
    std::vector<Line>        lines;
    inputs.reserve(140);
    lines.reserve(140);

    for(std::string v; std::getline(input, v);) inputs.push_back(v);

    input.close();
    auto count{0};

    std::for_each(std::execution::par, std::begin(inputs), std::end(inputs), [&](auto &input) {
        std::lock_guard<std::mutex> guard(g_pages_mutex);
        lines.push_back(Parse(input, ++count));
    });

    std::mutex same_line_mutex, up_line_mutex, down_line_mutex;

    auto checkUp   = [&]() {};
    auto checkLine = [&]() {};
    auto checkDown = [&]() {};

    std::for_each(std::execution::par, begin(lines), end(lines), [&](Line &line) {
        if(line.lineN % 2) return;
        std::printf("Running line %zu\n", line.lineN);
        checkUp();
        checkLine();
        checkDown();
    });

    std::for_each(std::execution::par, begin(lines), end(lines), [&](Line &line) {
        if(!(line.lineN % 2)) return;
        std::printf("Running line %zu\n", line.lineN);
    });

    // std::printf("Final result for fist game is   %zu\n", sum);
    // std::printf("Final result for second game is %zu\n", sum2);

    /* code */
    return 0;
}

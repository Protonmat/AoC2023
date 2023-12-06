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

constexpr size_t number_of_lines{140};

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

constexpr std::ostream &operator<<(std::ostream &os, std::vector<Line> &lines) {
    for(auto &line : lines) {
        std::string result;
        result.insert(0, number_of_lines, '.');

        for(auto &number : line.numbers_) {
            std::string val{std::format("{}", number.value_)};

            if(number.value_ == 0) {
                val = std::string(number.position_.finalPosition_ - number.position_.initialPosition_, '0');
            }
            result.replace(number.position_.initialPosition_, val.size(), val);
        }
        for(auto &symbol : line.symbols_) {
            std::string val{std::format("{}", symbol.symbol_)};
            result.replace(symbol.position_.initialPosition_, val.size(), val);
        }
        os << result << "\n";
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

size_t SumLine(Line &actualLine, Line &otherLine) {
    size_t sum{0};

    auto numbs = otherLine.numbers_.begin();
    auto symbl = actualLine.symbols_.begin();

    while(symbl != actualLine.symbols_.end()) {
        while(numbs != otherLine.numbers_.end()) {
            if(numbs->value_ == 0) {
                ++numbs;
                continue;
            }
            if(symbl->position_.initialPosition_ <= numbs->position_.finalPosition_ &&
               symbl->position_.finalPosition_ >= numbs->position_.initialPosition_) {
                std::cout << "Got a hit: \n"
                          << "\t" << *symbl << "\t" << *numbs << "\n";
                sum += numbs->value_;
                numbs->value_ = 0;
            }
            ++numbs;
        }
        numbs = otherLine.numbers_.begin();
        ++symbl;
    }

    return sum;
};

int main(int argc, char const *argv[]) {
    std::ifstream input("input.txt", std::ios::in);

    if(!input.is_open()) {
        std::printf("Error opening file\n");
        return 0;
    }

    std::vector<std::string> inputs;
    std::vector<Line>        lines;
    std::vector<Line>        lines2;
    inputs.reserve(number_of_lines);
    lines.reserve(number_of_lines);
    lines2.reserve(number_of_lines);

    for(std::string v; std::getline(input, v);) inputs.push_back(v);

    input.close();
    auto count{0};

    std::for_each(std::execution::par, std::begin(inputs), std::end(inputs), [&](auto &input) {
        std::lock_guard<std::mutex> guard(g_pages_mutex);
        lines.push_back(Parse(input, count++));
    }); 

    lines2 = lines;   

    auto checkUp = [&](Line &cLine) -> size_t {
        size_t sum{0};
        if(cLine.lineN == 0) {
            return sum;
        }

        auto &otherLine = lines[cLine.lineN - 1];
        sum = SumLine(cLine, otherLine);
        return sum;
    };

    auto checkLine = [&](Line &cLine) -> size_t {
        return SumLine(cLine, cLine);
    };
    auto checkDown = [&](Line &cLine) -> size_t {
        size_t sum {0};

        if (cLine.lineN == number_of_lines - 1 ) {
            return sum;
        }

        auto &otherLine = lines[cLine.lineN + 1];
        sum = SumLine(cLine, otherLine);
        return sum;
    };

    size_t result{0};
    std::for_each(std::execution::par, begin(lines), begin(lines) + (number_of_lines / 2u), [&](Line &line) {
        // if(line.lineN % 2) return;
        // std::printf("Running line %zu\n", line.lineN);
        result += checkUp(line);
        result += checkLine(line);
        result += checkDown(line);
    });
    std::printf("--------------------------------------\n");
    std::for_each(std::execution::par, begin(lines) + (number_of_lines / 2u), end(lines), [&](Line &line) {
        // if(!(line.lineN % 2)) return;
        // std::printf("Running line %zu\n", line.lineN);
        result += checkUp(line);
        result += checkLine(line);
        result += checkDown(line);
    });

    // std::printf("--------------------------------------\n");
    // std::printf("--------------------------------------\n");
    // std::printf("--------------------------------------\n");
    // std::printf("--------------------------------------\n");
    // std::printf("--------------------------------------\n");
    // std::cout << lines;
    // std::printf("--------------------------------------\n");
    // std::printf("--------------------------------------\n");
    // std::printf("--------------------------------------\n");
    // std::printf("--------------------------------------\n");

    size_t result2{0};
    std::for_each(std::execution::par, begin(lines2), end(lines2), [&](Line &line) {
        std::printf("Running line %zu\n", line.lineN);

        std::for_each(std::execution::par, begin(line.symbols_), end(line.symbols_), [&](Symbol const &sym) {
            Number *otherNumber{nullptr};

            auto computeNumber = [&](Number & number_) {
                if(number_.position_.initialPosition_ > sym.position_.finalPosition_) {
                    return;
                }

                if(number_.position_.finalPosition_ < sym.position_.initialPosition_) {
                    return;
                }

                if(!otherNumber) {
                    otherNumber = &number_;
                    return;
                }

                result2 += (otherNumber->value_ * number_.value_);
                otherNumber->value_ = 0;
                number_.value_      = 0;
                // reset previous number
                otherNumber = nullptr;
            };

            if(line.lineN != 0) {
                // do up operation check
                Line &otherLine = lines2[line.lineN - 1];
                for(auto &number_ : otherLine.numbers_) {
                    computeNumber(number_);
                }
            };

            // do side operation check
            {
                for (auto &number_ : line.numbers_)
                {
                    computeNumber(number_);
                }
            }
            

            if(line.lineN != number_of_lines - 1) {
                // do down operation
                Line &otherLine = lines2[line.lineN + 1];
                for(auto &number_ : otherLine.numbers_) {
                    computeNumber(number_);
                }
            }            
        });
    });


    std::printf("Final result for first game is %zu\n", result);
    std::printf("Final result for second game is   %zu\n", result2);

    /* code */
    return 0;
}

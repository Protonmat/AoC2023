#include <array>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>





struct Game {
    size_t id_{0};
};

struct Color {
    std::string name;
    size_t      value{0};
};

std::ostream &operator<<(std::ostream &os, Color &color) {
    os << color.name << ": " << color.value;
    return os;
}

std::istream &operator>>(std::istream &is, Color &color) {
    is >> color.value >> color.name;
    if(color.name[color.name.size() - 1] == ',' || color.name[color.name.size() - 1] == ';') {
        color.name.pop_back();
    }
    return is;
};

auto ParseSecondGame = [](std::string game, size_t &r_, size_t &g_, size_t &b_) -> bool {
    size_t             gameId;
    std::string        name;
    char               colon;
    std::istringstream stream(game);
    Color              color_{};
    stream >> name >> gameId >> colon;

    if (game.length() == 0) {
        return false;
    }

    std::cout << name << " # " << gameId << "\n";
    while(stream >> color_) {
        std::cout << "Color: " << color_ << "\n";
        if(color_.name == "red" && color_.value > r_) {
            r_ = color_.value;
        }

        if(color_.name == "green" && color_.value > g_) {
            g_ = color_.value;
        }

        if(color_.name == "blue" && color_.value > b_) {
            b_ = color_.value;
        }
    }

    return true;
};

bool ParseGameFirstPart(size_t r, size_t g, size_t b, std::string game) {
    // Game 4: 1 green, 2 blue; 1 blue, 4 green; 8 red, 3 blue, 3 green; 8 red, 2 green, 1 blue; 7 green, 3 blue, 2 red;
    // 1 red, 1 green, 3 blue

    size_t gameId{0};
    if(game.length() == 0) {
        return false;
    }

    auto parseSectors = [&](std::string game, size_t const &r_, size_t const &g_, size_t const &b_) -> bool {
        size_t             gameId;
        std::string        name;
        char               colon;
        std::istringstream stream(game);
        Color              color_{};

        stream >> name >> gameId >> colon;

        // std::cout << name << " # " << gameId << "\n";
        while(stream >> color_) {
            // std::cout << "Color: " << color_ << "\n";
            if(color_.name == "red" && color_.value > r) {
                return false;
            }

            if(color_.name == "green" && color_.value > g) {
                return false;
            }

            if(color_.name == "blue" && color_.value > b) {
                return false;
            }
        }

        return true;
    };

    size_t r_{12}, g_{13}, b_{14};
    return parseSectors(game, r_, g_, b_);
};

int main(int argc, char const *argv[]) {
    std::ifstream input("input.txt", std::ios::in);

    if(!input.is_open()) {
        std::printf("Error opening file\n");
        return 0;
    }

    size_t index{0};
    size_t sum{0};

    
    size_t sum2{0};

    while(input.good()) {
        std::string line;
        size_t r{0}, g{0}, b{0};

        std::getline(input, line);
        if(ParseGameFirstPart(12, 13, 14, line)) {
            sum += index + 1;
        }
        if (ParseSecondGame(line, r, g, b)) {
            std::cout << "Values for game 2 #" << index +1 << " are " << r << " red " << g << " green " << b << " blue\n";
            sum2 += (r * g * b);
        };
        ++index;
    }

    input.close();

    std::printf("Final result for fist game is   %zu\n", sum);
    std::printf("Final result for second game is %zu\n", sum2);

    /* code */
    return 0;
}

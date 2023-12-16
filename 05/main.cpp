#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <format>
#include <map>
#include <algorithm>
#include <execution>

#include <utils/utils.hpp>

struct FieldMap {
    size_t destination_{0};
    size_t source_{0};
    size_t quantity_{0};
};

struct Seed {
    size_t number{0};
    size_t amount{0};
    size_t soil{0};
    size_t fertilizer{0};
    size_t water{0};
    size_t light;
    size_t temperature{0};
    size_t humidity{0};
    size_t location{0};
};

constexpr std::istream &operator>>(std::istream &is, FieldMap &fieldmap) {
    is >> fieldmap.destination_ >> fieldmap.source_ >> fieldmap.quantity_;
    return is;
}

constexpr std::istream &operator>>(std::ifstream &file, std::vector<FieldMap> &field) {
    // eat leyend
    std::string void_string{};
    file >> void_string;  // remove <map name>
    file >> void_string;  // remove <map:>

    FieldMap fm;
    while(file >> fm) {
        field.push_back(fm);
    }

    if(file.fail()) {
        file.clear();
    }
    return file;
};

auto GetLowestLocationFile = [](std::ifstream &file, size_t reserve_seeds) -> size_t {
    std::vector<Seed>     seeds;
    std::vector<FieldMap> seeds_to_soil;
    std::vector<FieldMap> soil_to_ferilizer;
    std::vector<FieldMap> ferilizer_to_water;
    std::vector<FieldMap> water_to_light;
    std::vector<FieldMap> light_to_temperature;
    std::vector<FieldMap> temperature_to_humidity;
    std::vector<FieldMap> humidity_to_location;

    seeds.reserve(reserve_seeds);

    std::string void_string{};

    file >> void_string;

    size_t seed_number;
    while(file >> seed_number) {
        seeds.push_back(Seed{.number = seed_number});
        std::printf("Last got %zu\n", seed_number);
    };

    if(file.failbit) {
        file.clear();
    }

    auto SetFieldMap = [](std::ifstream &file, std::vector<FieldMap> &field) {
        // eat leyend
        std::string void_string{};
        file >> void_string;  // remove <map name>
        file >> void_string;  // remove <map:>

        FieldMap fm;
        while(file >> fm) {
            field.push_back(fm);
        }

        if(file.fail()) {
            file.clear();
        }
    };

    SetFieldMap(file, seeds_to_soil);
    SetFieldMap(file, soil_to_ferilizer);
    SetFieldMap(file, ferilizer_to_water);
    SetFieldMap(file, water_to_light);
    SetFieldMap(file, light_to_temperature);
    SetFieldMap(file, temperature_to_humidity);
    SetFieldMap(file, humidity_to_location);

    auto Process = [](std::vector<FieldMap> const &field, size_t const &n) -> size_t {
        size_t result{0};
        std::for_each(std::execution::par, begin(field), end(field), [n, &result](FieldMap const &f) {
            if(n >= f.source_ && n <= (f.source_ + f.quantity_)) {
                result = (n - f.source_) + f.destination_;
            }
        });

        return result == 0 ? n : result;
    };

    auto UpateSeed = [&](Seed &seed) {
        seed.soil        = Process(seeds_to_soil, seed.number);
        seed.fertilizer  = Process(soil_to_ferilizer, seed.soil);
        seed.water       = Process(ferilizer_to_water, seed.fertilizer);
        seed.light       = Process(water_to_light, seed.water);
        seed.temperature = Process(light_to_temperature, seed.light);
        seed.humidity    = Process(temperature_to_humidity, seed.temperature);
        seed.location    = Process(humidity_to_location, seed.humidity);
    };
    size_t lowest_location{ULLONG_MAX};
    std::for_each(std::execution::par, begin(seeds), end(seeds), [&](Seed &seed) {
        UpateSeed(seed);
        if(seed.location < lowest_location) {
            lowest_location = seed.location;
        }
    });

    return lowest_location;
};

std::ostream &operator<<(std::ostream &os, FieldMap &fm) {
    os << std::format("{} {} {}", fm.destination_, fm.source_, fm.quantity_);
    return os;
}

std::string GetNameOfLevel(size_t level) {
    switch(level) {
        case 0:
            return "seed-to-soil:";
        case 1:
            return "soil-to-fertilizer:";
        case 2:
            return "fertilizer-to-water:";
        case 3:
            return "water-to-light:";
        case 4:
            return "light-to-temperature:";
        case 5:
            return "temperature-to-humidity:";
        case 6:
            return "humidity-to-location:";
        default:
            return "wtf";
    }
}

void SecondSolution(std::map<size_t, std::vector<FieldMap>> &map, FieldMap rhs, size_t &final_result, size_t level = 0);

void SecondSolution(std::map<size_t, std::vector<FieldMap>> &map, FieldMap rhs, size_t &final_result, size_t level) {
    std::string tabs(level, '\t');
    // std::cout << tabs << GetNameOfLevel(level) << ", " << rhs << "\n";

    if(level > 6) {
        if(rhs.source_ < final_result) {
            std::cout << tabs << "New Final result is : " << rhs.source_ << "\n";
            final_result = rhs.source_;
        }
        // std::cout << tabs << "Max Level reached\n";
        return;
    }

    for(auto &field : map[level]) {
        std::cout << tabs << GetNameOfLevel(level) << ", " << field << "\n";

        size_t a1{0}, a2{0}, b1{0}, b2{0};
        size_t dest{0};
        a1 = rhs.source_;
        a2 = a1 + rhs.quantity_;

        b1   = field.source_;
        b2   = b1 + field.quantity_;
        dest = field.destination_;

        std::cout << tabs << std::format("Comparing: {} {}\n", a1, a2);
        std::cout << tabs << std::format("           {} {}\n", b1, b2);
        // Ignore out of bounds
        if(a1 > b2 || a2 < b1) {
            std::cout << tabs << "Out of bounds\n";
            continue;
        }

        /*
            |----------|
            |----------|
            b1 == a1 && b2 == a2
        */

        if(b1 == a1 && b2 == a2) {
            std::cout << tabs << "First case\n";
            rhs.quantity_ = 0;
            SecondSolution(map, {dest, dest, field.quantity_}, final_result, level + 1);
            continue;
        }

        /*
                    |-----------|
            |----------|
            b1 < a1 && b2 < a2
            first case
        */
        if(a1 >= b1 && a2 >= b2) {
            std::cout << tabs << "Second case\n";
            rhs.source_   = b2;
            rhs.quantity_ = a2 - b2;
            SecondSolution(map, {dest + a1 - b1, dest + a1 - b1, b2 - a1}, final_result, level + 1);
            // go next
            continue;
        }

        /*
            |----------|
                |-----------|

            a1 < b1 && a2 < b2
        */

        if(a1 <= b1 && a2 <= b2) {
            std::cout << tabs << "third case\n";
            rhs.quantity_ = b1 - a1;
            SecondSolution(map, {dest, dest, a2-b1}, final_result, level + 1);
            // go next
            continue;
        }

        /*
               |----|
            |----------|
            a1 > b1 && a2 < b2
        */

        if(a1 >= b1 && a2 <= b2) {
            std::cout << tabs << "fourth case\n";
            FieldMap f{dest + a1 - b1, dest + a1 - b1, rhs.quantity_};
            SecondSolution(map, f, final_result, level + 1);
            rhs.quantity_ = 0;
            // go next
            continue;
        }

        /*
         |----------|
            |----|
            a1 <= b1 && a2 >= b2
        */
        if(a1 <= b1 && a2 >= b2) {
            std::cout << tabs << "Fifth case\n";
            rhs.quantity_ = 0;
            SecondSolution(map, {dest, dest, b2 - b1}, final_result, level + 1);
            SecondSolution(map, {a1, a1, b1 - a1}, final_result, level);
            SecondSolution(map, {b2, b2, a2 - b2}, final_result, level);
            continue;
        }
    }

    // Process the rest
    std::cout << tabs << "Rest case\n";
    if(rhs.quantity_ != 0) {
        SecondSolution(map, rhs, final_result, level + 1);
    }
};

auto GetLowestLocationFileSecond = [](std::ifstream &file, size_t reserve_seeds) -> size_t {
    std::vector<Seed>     seeds;
    std::vector<FieldMap> seeds_to_soil;
    std::vector<FieldMap> soil_to_ferilizer;
    std::vector<FieldMap> ferilizer_to_water;
    std::vector<FieldMap> water_to_light;
    std::vector<FieldMap> light_to_temperature;
    std::vector<FieldMap> temperature_to_humidity;
    std::vector<FieldMap> humidity_to_location;

    seeds.reserve(reserve_seeds / 2);

    std::string void_string{};

    file >> void_string;

    size_t seed_number;
    size_t ammount{0};
    while(file >> seed_number >> ammount) {
        seeds.push_back(Seed{.number = seed_number, .amount = ammount});
        std::printf("Start from %zu for %zu seeds\n", seed_number, ammount);
    };

    if(file.failbit) {
        file.clear();
    }

    std::map<size_t, std::vector<FieldMap>> levels;

    file >> seeds_to_soil;
    file >> soil_to_ferilizer;
    file >> ferilizer_to_water;
    file >> water_to_light;
    file >> light_to_temperature;
    file >> temperature_to_humidity;
    file >> humidity_to_location;

    levels[0] = seeds_to_soil;
    levels[1] = soil_to_ferilizer;
    levels[2] = ferilizer_to_water;
    levels[3] = water_to_light;
    levels[4] = light_to_temperature;
    levels[5] = temperature_to_humidity;
    levels[6] = humidity_to_location;

    size_t lowest_location{SIZE_MAX};

    // declare a function thats going to be used as a recursive function
    // Recursive function to solve each intersection into the next level

    std::for_each(std::execution::par, begin(seeds), end(seeds), [&levels, &lowest_location](Seed &seed) {
        std::printf("Seed : %zu - %zu\n", seed.number, seed.amount);
        // Recu(levels, FieldMap{.destination_ = seed.number, .source_ = seed.number, .quantity_ = seed.amount - 1}, 0,
        //      lowest_location);
        SecondSolution(levels, {.destination_ = seed.number, .source_ = seed.number, .quantity_ = seed.amount},
                       lowest_location, 0);
    });

    return lowest_location;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[]) {
    // create a seed vector
    constexpr size_t test_seed_count{4};
    constexpr size_t seed_count{20};

    // std::ifstream test_file_first("test_input.txt", std::ios::in);
    // std::ifstream file_first("input.txt", std::ios::in);

    // if(!test_file_first || !file_first) {
    //     std::cerr << "Invalid files\n";
    //     return 0;
    // }

    std::ifstream test_file_second("test_input.txt", std::ios::in);

    // size_t location_test_first = GetLowestLocationFile(test_file_first, test_seed_count);
    // size_t location_first      = GetLowestLocationFile(file_first, seed_count);

    size_t location_test_second = GetLowestLocationFileSecond(test_file_second, test_seed_count);

    // std::ifstream file_second("input.txt", std::ios::in);
    // size_t location_second      = GetLowestLocationFileSecond(file_second, seed_count);

    // std::printf("Location Test First: %zu\n", location_test_first);
    // std::printf("Location First: %zu\n", location_first);

    std::printf("Location Test Second: %zu\n", location_test_second);
    // std::printf("Location Second: %zu\n", location_second);

    {
        std::ifstream file("input.txt", std::ios::in);
        size_t        location = GetLowestLocationFileSecond(file, 2);
        std::printf("Location Second: %zu\n", location);
    }

    return 0;
}

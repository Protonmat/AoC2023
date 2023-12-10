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

    size_t lowest_location{ULLONG_MAX};

    auto Recu = [&levels](size_t n, size_t q, size_t level){

        if (level == 7) {
            return;
        }
        auto l = levels[level];
        std::for_each(std::execution::par, begin(l), end(l) , [](FieldMap const& f){
            
        });
    };




    return lowest_location;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[]) {
    // create a seed vector
    constexpr size_t test_seed_count{4};
    constexpr size_t seed_count{20};

    std::ifstream test_file_first("test_input.txt", std::ios::in);
    std::ifstream file_first("input.txt", std::ios::in);

    if(!test_file_first || !file_first) {
        std::cerr << "Invalid files\n";
        return 0;
    }

    std::ifstream test_file_second("test_input.txt", std::ios::in);
    std::ifstream file_second("input.txt", std::ios::in);

    size_t location_test_first = GetLowestLocationFile(test_file_first, test_seed_count);
    size_t location_first      = GetLowestLocationFile(file_first, seed_count);

    size_t location_test_second = GetLowestLocationFileSecond(test_file_second, test_seed_count);
    size_t location_second      = GetLowestLocationFileSecond(file_second, seed_count);

    std::printf("Location Test First: %zu\n", location_test_first);
    std::printf("Location First: %zu\n", location_first);

    return 0;
}

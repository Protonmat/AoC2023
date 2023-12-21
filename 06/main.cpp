#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <format>
#include <map>
#include <cmath>
#include <algorithm>
#include <execution>

#include <utils/utils.hpp>

struct Time {
    size_t time_;
};

struct Distance {
    size_t distance_;
};

std::istream &operator>>(std::istream &is, std::vector<Time> &times) {
    std::string void_str("");
    is >> void_str;
    Time t;
    while(is >> t.time_) {
        times.push_back(t);
    }
    if(is.fail()) {
        is.clear();
    }
    return is;
};

std::istream &operator>>(std::istream &is, std::vector<Distance> &distances) {
    std::string void_str("");
    is >> void_str;
    Distance d;
    while(is >> d.distance_) {
        distances.push_back(d);
    }

    if(is.fail()) {
        is.clear();
    }
    return is;
};

std::ostream &operator<<(std::ostream &os, std::vector<Distance> distances) {
    for(auto const &dist : distances) {
        os << dist.distance_ << " ";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, std::vector<Time> times) {
    for(auto const &time : times) {
        os << time.time_ << " ";
    }
    return os;
}

void ProcessFileFirst(std::string_view const &input) {
    std::vector<Time>     times;
    std::vector<Distance> distances;

    times.reserve(4);
    distances.reserve(4);

    std::ifstream file(input.data(), std::ios::in);

    if(!file) {
        std::printf("Failed to open file: %s\n", input);
        exit(1);
    }

    file >> times;
    file >> distances;
    // ඞ sus
    std::cout << "times: " << times << "\n";
    std::cout << "distances: " << distances << "\n";

    auto getFirstResult = [](Time &time, Distance &distance) -> size_t {
        int64_t a{-1}, b{int64_t(time.time_)}, c{int64_t(-(distance.distance_))};

        float radiz = (b * b) - (4.f * a * c);
        float root = std::sqrt(radiz);
        float min = ((-b) + root) / (2 * a);
        float max = ((-b) - root) / (2 * a);

        // if ( (-(min*min) + (time.time_ * min)) == distance.distance_ ) {
        //     std::cout << "Adding one to min: " << min << "\n";
        //     min += 1.f;
        // }

        // if ( (-(max*max) + (time.time_ * max)) == distance.distance_ ) {
        //     std::cout << "Substraction one to max: " << max << "\n";
        //     max -= 1.f;
        // }

        if ( std::round(min) == min ) {
            min++;
        } else if ( std::round(min) - min < 0.5f ) {
            min = std::ceil(min);
        } else {
            min = std::floor(min);
        }

        if (std::round(max) == max) {
            max--;
        } else if ( std::round(max) - max > 0.5f ) {
            max = std::ceil(max);
        } else {
            max = std::floor(max);
        }


        auto result = std::floor(max) - std::round(min) + 1;
        return result;
    };

    size_t total_result{1};
    for(size_t i{0}; i != times.size(); ++i) {
        auto res = getFirstResult(times.at(i), distances.at(i));
        // std::cout << "Res is " << res << "\n";
        total_result *= res;
    }

    std::printf("Result is: %zu\n", total_result);
}

void ProcessFileSecond(std::string_view const &input) {
    std::vector<Time>     times;
    std::vector<Distance> distances;

    times.reserve(4);
    distances.reserve(4);

    std::ifstream file(input.data(), std::ios::in);

    if(!file) {
        std::printf("Failed to open file: %s\n", input);
        exit(1);
    }

    file >> times;
    file >> distances;
    // ඞ sus
    std::cout << "times: " << times << "\n";
    std::cout << "distances: " << distances << "\n";

    auto getResult = [](Time &&time, Distance &&distance) -> size_t {
        int64_t a{-1}, b{int64_t(time.time_)}, c{int64_t(-(distance.distance_))};

        float radiz = (b * b) - (4.f * a * c);
        float root = std::sqrt(radiz);
        float min = ((-b) + root) / (2 * a);
        float max = ((-b) - root) / (2 * a);

        if ( std::round(min) == min ) {
            min++;
        } else if ( std::round(min) - min < 0.5f ) {
            min = std::ceil(min);
        } else {
            min = std::floor(min);
        }

        if (std::round(max) == max) {
            max--;
        } else if ( std::round(max) - max > 0.5f ) {
            max = std::ceil(max);
        } else {
            max = std::floor(max);
        }


        //  4-8-9
        auto result = std::floor(max) - std::round(min) + 1;
        return result;
    };

    size_t total_result{1};
    size_t total_time{0};
    size_t total_distance{0};

    size_t digits{0};
    for (size_t i{times.size()}; i != 0; --i) {
        total_time += times[i-1].time_ * std::pow(10, digits);
        digits += std::ceil(std::log10(times[i-1].time_)); 
    }
    digits = 0;
    for (size_t i{distances.size()}; i != 0; --i) {
        total_distance += distances[i-1].distance_ * std::pow(10, digits);
        digits += std::ceil(std::log10(distances[i-1].distance_)); 
    }

    // for(size_t i{0}; i != times.size(); ++i) {
        

    //     total_time     += times[i].time_ * std::pow(10, times.size() - i);
    //     total_distance += distances[i].distance_ * std::pow(10, distances.size() - i);
    //     // std::cout << "Res is " << res << "\n";        
    // }

    auto res = getResult(Time{total_time}, Distance{total_distance});
    std::printf("Result is: %zu\n", res);
}


int main(int argc, char const *argv[]) {
    // ProcessFileFirst("test_input.txt");
    // ProcessFileFirst("input.txt");
    ProcessFileSecond("test_input.txt");
    ProcessFileSecond("input.txt");

    
    /* code */
    std::size_t asd;
    return 0;
}

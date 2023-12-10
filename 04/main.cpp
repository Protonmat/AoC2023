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

struct Card {
    size_t  id{0};
    size_t  result{0};
    size_t  winners{0};
    std::map<size_t, size_t> numbers;
};

constexpr std::istream &operator>>(std::istream &is, Card &card) {
    std::string str_void{};
    
    is >> str_void >> card.id >> str_void;
    size_t key_{0};

    while(is >> key_) {
        // std::printf("Printing ... %zu\n", key_);;
        card.numbers[key_] = 0;
    }

    if (is.fail()) {
        is.clear();
        is >> str_void;
    }
    
    key_ = 0;
    
    while(is >> key_) {
        // std::printf("Finding ... %zu\n", key_);
        if ( card.numbers.find(key_) != card.numbers.end() ) {
            ++card.winners;
        }
    }

    card.result = card.winners < 2 ? card.winners :  1ul << (card.winners - 1);
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

    std::map<size_t, size_t> test_cards;    
    std::map<size_t, size_t> cards;

    ReadEntireFileIntoVector("input.txt", lines);
    ReadEntireFileIntoVector("test_input.txt", test_lines);
    std::printf("Lines %zu\n", lines.size());
    
    size_t test_final_result{0};
    size_t final_result{0};

    auto AddCards = [] (Card & card, std::map<size_t, size_t>& deck) {
        size_t times{1};
        auto card_in_deck = deck.find(card.id);
        if (card_in_deck == deck.end()) {
            deck.insert(std::pair(card.id, 1));
        } else {
            card_in_deck->second +=1;
            times = card_in_deck->second;
        }

        if (card.winners == 0) {
            return;
        }

        for( size_t i{ card.id + 1}; i != (card.id + card.winners + 1) ; ++i ) {
            auto found = deck.find(i);
            if ( found != deck.end() ) {
                found->second += (times*1);
            } else {
              deck[i] = (times*1);
            }
        }

    };


    std::for_each(std::execution::par, begin(test_lines), end(test_lines), [&AddCards, &test_cards, &test_final_result]( auto &line ) {
        std::stringstream ss(line);
        Card card;
        ss >> card;
        AddCards(card, test_cards);
        test_final_result += card.result;
    });


    std::for_each(std::execution::par, begin(lines), end(lines), [&AddCards, &cards, &final_result]( auto &line ) {
        std::stringstream ss(line);
        Card card;
        ss >> card;
        AddCards(card, cards);
        final_result += card.result;
    });

    size_t test_second_result {0};
    size_t second_result{0};

    std::for_each(std::execution::par, begin(test_cards), end(test_cards), [&test_second_result](auto& item) {
        test_second_result += item.second;
    });

    std::for_each(std::execution::par, begin(cards), end(cards), [&second_result](auto& item) {
        second_result += item.second;
    });



    std::printf("Test result is %zu\n", test_final_result);
    std::printf("Test second result is %zu\n", test_second_result);
    
    std::printf("Final result is %zu\n",  final_result);
    std::printf("Final seoncd result is %zu\n",  second_result);


    /* code */
    return 0;
}

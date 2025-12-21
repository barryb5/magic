#include <iostream>
#include <fstream>
#include <filesystem>
#include "include/helpers.hpp"
#include <vector>
#include "include/card.hpp"
#include <memory>

int main(int argc, char** argv)
{
    std::cout << "Hello World Test" << std::endl;

    std::vector<std::shared_ptr<Card>> deck =
        load_cards_to_deck("card_data/cards_raw/card_library.txt");

    std::cout << "Loaded " << deck.size() << " cards\n";

    return 0;
}
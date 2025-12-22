#include <iostream>
#include <fstream>
#include <filesystem>
#include "include/helpers.hpp"
#include <vector>
#include "include/card.hpp"
#include <memory>
#include "include/player.hpp"
#include "include/boardstate.hpp"

int main(int argc, char** argv)
{
    std::cout << "Hello World Test" << std::endl;

    std::vector<std::shared_ptr<Card>> deck =
        load_cards_to_deck("card_data/cards_raw/card_library.txt");

    Player player = Player("Ezuri, Claw of Progress", deck);

    player.printCards();

    player.start();

    player.printHand();

    BoardState board(player);

    board.printBoardState();

    return 0;
}
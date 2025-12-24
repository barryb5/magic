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

    player.start();

    BoardState board(player);

    board.nextTurn();

    std::cout << board.printBoardStateForLLM() << std::endl;

    // int card_num = 0;
    // std::cout << "Enter the number of the card in your hand to play as a permanent: ";
    // std::cin >> card_num;

    // if (card_num < 0 || static_cast<size_t>(card_num) >= player.hand.size())
    // {
    //     std::cout << "Invalid card number." << std::endl;
    //     return 1;
    // }

    // std::shared_ptr<Card> card_to_play = player.hand[card_num];

    // board.playPermanent(card_to_play);
    
    // board.player.printHand();

    // board.printBoardState();


    return 0;
}
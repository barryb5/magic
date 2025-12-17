#include "../include/player.hpp"

#include "../include/card.hpp"
#include <random>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>

void Player::shuffle()
{
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(deck.begin(), deck.end(), g);
}

void Player::start()
{
    shuffle();

    // Draw first 7 cards
    std::move(deck.begin(), deck.begin() + 7, std::back_inserter(hand));
    deck.erase(deck.begin(), deck.begin() + 7);

    // Put commander in command zone
    commander_in_command_zone = true;
}

void Player::draw()
{
    hand.push_back(std::move(deck.front()));
    deck.erase(deck.begin());
}

void Player::drawSpecific(const Card& card)
{
    auto handItr = std::find(hand.begin(), hand.end(), card);

    if (handItr != hand.end())
    {
        // Card is in hand already
        std::cout << "ERROR card " << card.name << " is already in your hand" << std::endl;
        return;
    }

    auto libraryItr = std::find(library.begin(), library.end(), card);

    if (libraryItr == library.end())
    {
        // Card isn't in deck
        std::cout << "ERROR card " << card.name << " is not in your deck" << std::endl;
        return;
    }

    hand.push_back(std::move(*libraryItr));
    library.erase(libraryItr);
}

void Player::changeLife(int life)
{
    health += life;
}

void Player::mill(size_t n)
{
    // In case mill is larger than deck
    std::size_t k = std::min(n, library.size());
    auto first = library.begin();
    auto mid = library.begin() + static_cast<std::ptrdiff_t>(k);

    graveyard.insert(
        graveyard.begin(),
        std::make_move_iterator(std::reverse_iterator(mid)),
        std::make_move_iterator(std::reverse_iterator(first)));

    // Remove those k elements from vec1
    graveyard.erase(first, mid);
}
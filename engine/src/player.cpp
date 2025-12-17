#include "player.hpp"

#include "card.hpp"
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

    auto deckItr = std::find(deck.begin(), deck.end(), card);

    if (deckItr == deck.end())
    {
        // Card isn't in deck
        std::cout << "ERROR card " << card.name << " is not in your deck" << std::endl;
        return;
    }

    hand.push_back(std::move(*deckItr));
    deck.erase(deckItr);
}
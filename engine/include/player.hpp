#pragma once

#include <vector>
#include "card.hpp"

class Player
{
public:
    Card commander;
    bool commander_in_command_zone = true;
    int health = 40;
    std::vector<Card> deck;
    std::vector<Card> library{};
    std::vector<Card> graveyard{};
    std::vector<Card> exile{};
    std::vector<Card> hand{};

    Player(Card commander, std::vector<Card> deck) : commander(std::move(commander)), deck(std::move(deck)) {}

    void shuffle();
    void start();
    void draw();
    void drawSpecific(const Card& card);
    void changeLife(int life);
    void mill(size_t n);
};
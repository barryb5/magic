#pragma once

#include <vector>
#include <memory>
#include "card.hpp"

class Player
{
public:
    std::shared_ptr<Card> commander;
    bool commander_in_command_zone = true;
    int health = 40;
    std::vector<std::shared_ptr<Card>> deck;
    std::vector<std::shared_ptr<Card>> library{};
    std::vector<std::shared_ptr<Card>> graveyard{};
    std::vector<std::shared_ptr<Card>> exile{};
    std::vector<std::shared_ptr<Card>> hand{};

    Player() = delete;
    Player(std::shared_ptr<Card> commander, std::vector<std::shared_ptr<Card>> deck) : commander(std::move(commander)), deck(std::move(deck)) {}
    Player(Card commander, std::vector<std::shared_ptr<Card>> deck) : commander(std::make_shared<Card>(commander)), deck(std::move(deck)) {}
    Player(std::string commander, std::vector<std::shared_ptr<Card>> deck);

    void printCards() const;
    void printHand() const;
    void shuffle();
    void start();
    void draw();
    void drawSpecific(const Card& card);
    void changeLife(int life);
    void mill(size_t n);
};
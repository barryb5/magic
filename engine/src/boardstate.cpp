#include "../include/boardstate.hpp"
#include <iostream>
#include "../include/card.hpp"

void BoardState::printBoardState() const
{
    std::cout << "Board State:\n";
    std::cout << "Player Health: " << player.health << "\n";

    std::cout << "Lands on Board (" << lands.size() << "):\n";
    for (const auto& land_ptr : lands)
    {
        if (land_ptr)
            land_ptr->printNameAndOracle();
    }

    std::cout << "Creatures on Board (" << creatures.size() << "):\n";
    for (const auto& creature_ptr : creatures)
    {
        if (creature_ptr)
            creature_ptr->printNameAndOracle();
    }

    std::cout << "Enchantments on Board (" << enchantments.size() << "):\n";
    for (const auto& enchantment_ptr : enchantments)
    {
        if (enchantment_ptr)
            enchantment_ptr->printNameAndOracle();
    }

    std::cout << "Artifacts on Board (" << artifacts.size() << "):\n";
    for (const auto& artifact_ptr : artifacts)
    {
        if (artifact_ptr)
            artifact_ptr->printNameAndOracle();
    }
}

void BoardState::playPermanent(const std::shared_ptr<Card>& card)
{
    if (!card)
    {
        std::cerr << "Cannot play a null card.\n";
        return;
    }

    // Determine the type of the card and add it to the appropriate zone
    if (card->type_line.find("Land") != card->type_line.end())
    {
        lands.push_back(card);
    }
    else if (card->isCreature())
    {
        creatures.push_back(card);
    }
    else if (card->type_line.find("Enchantment") != card->type_line.end())
    {
        enchantments.push_back(card);
    }
    else if (card->type_line.find("Artifact") != card->type_line.end())
    {
        artifacts.push_back(card);
    }
    else
    {
        std::cerr << "Card '" << card->name << "' is not a permanent type.\n";
        return;
    }

    // Remove the card from player's hand
    auto it = std::find(player.hand.begin(), player.hand.end(), card);
    if (it != player.hand.end())
    {
        player.hand.erase(it);
    }
    else
    {
        std::cerr << "Card '" << card->name << "' not found in hand.\n";
    }
}

void BoardState::playPermanent(const std::string& card_name)
{
    auto it = std::find_if(player.hand.begin(), player.hand.end(),
                           [&card_name](const std::shared_ptr<Card>& cptr)
                           { return cptr && cptr->name == card_name; });

    if (it == player.hand.end())
    {
        std::cerr << "Card '" << card_name << "' not found in hand.\n";
        return;
    }

    playPermanent(*it);
}
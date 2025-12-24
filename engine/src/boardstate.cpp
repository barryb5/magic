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


std::string BoardState::printBoardStateForLLM()
{
    std::string result;

    result += "Health: " + std::to_string(player.health) + "\n";
    result += "Turn: " + std::to_string(turn_number) + "\n";
    result += "Experience: " + std::to_string(experience_counters) + "\n";

    result += "----\n";

    result += "Lands " + std::to_string(lands.size()) + ":\n";
    for (const auto& land_ptr : lands)
    {
        if (land_ptr)
        {
            result += land_ptr->printForLLM() + "\n";
            result += "----\n";
        }
    }

    result += "Creatures " + std::to_string(creatures.size()) + ":\n";
    for (const auto& creature_ptr : creatures)
    {
        if (creature_ptr)
        {
            result += creature_ptr->printForLLM() + "\n";
            result += "----\n";
        }
    }

    result += "Enchantments " + std::to_string(enchantments.size()) + ":\n";
    for (const auto& enchantment_ptr : enchantments)
    {
        if (enchantment_ptr)
        {
            result += enchantment_ptr->printForLLM() + "\n";
            result += "----\n";
        }
    }

    result += "Artifacts " + std::to_string(artifacts.size()) + ":\n";
    for (const auto& artifact_ptr : artifacts)
    {
        if (artifact_ptr)
        {
            result += artifact_ptr->printForLLM() + "\n";
            result += "----\n";
        }
    }

    result += "---\nHand\n";
    result += player.printHandForLLM();

    return result;
}

bool BoardState::addPermanentToBattlefield(const std::shared_ptr<Card>& card)
{
    if (!card)
    {
        std::cerr << "Cannot play a null card.\n";
        return false;
    }


    if (card->type_line.find(CardType::LAND) != card->type_line.end())
    {
        lands.push_back(card);
    }
    else if (card->isCreature())
    {
        creatures.push_back(card);
    }
    else if (card->type_line.find(CardType::ENCHANTMENT) != card->type_line.end())
    {
        enchantments.push_back(card);
    }
    else if (card->type_line.find(CardType::ARTIFACT) != card->type_line.end())
    {
        artifacts.push_back(card);
    }
    else
    {
        std::cerr << "Card '" << card->name << "' is not a permanent type.\n";
        return false;
    }

    return true;
}


void BoardState::playPermanent(const std::shared_ptr<Card>& card)
{
    if (!addPermanentToBattlefield(card))
        return;

    // Remove from hand
    auto it = std::find(player.hand.begin(), player.hand.end(), card);
    if (it != player.hand.end())
    {
        player.hand.erase(it);
    }
    else if (player.commander_in_command_zone && *card == *(player.commander))
    {
        player.commander_in_command_zone = false;
    }
    else
    {
        std::cerr << "Card '" << card->name << "' not found in hand.\n";
    }
}

void BoardState::playPermanent(const std::string& card_name)
{
    // Independently: this overload does not call the shared_ptr overload.
    auto it = std::find_if(
        player.hand.begin(), player.hand.end(),
        [&card_name](const std::shared_ptr<Card>& cptr)
        {
            return cptr && cptr->name == card_name;
        });

    if (it == player.hand.end())
    {
        if (player.commander_in_command_zone && card_name == player.commander->name)
        {
            player.commander_in_command_zone = false;
            addPermanentToBattlefield(player.commander);
            return;
        }
        std::cerr << "Card '" << card_name << "' not found in hand.\n";
        return;
    }

    // Use the found instance directly and erase by iterator (no second search).
    const std::shared_ptr<Card> card = *it;

    if (!addPermanentToBattlefield(card))
        return;

    player.hand.erase(it);
}

void BoardState::nextTurn()
{
    // Update Turn Number
    ++turn_number;

    // Untap all permanents
    for (const auto& land_ptr : lands)
    {
        if (land_ptr)
        {
            land_ptr->tapped = false;
        }
    }

    for (const auto& creature_ptr : creatures)
    {
        if (creature_ptr)
        {
            creature_ptr->summoning_sick = false;
            creature_ptr->tapped = false;
        }
    }

    for (const auto& enchantment_ptr : enchantments)
    {
        if (enchantment_ptr)
        {
            enchantment_ptr->tapped = false;
        }
    }

    for (const auto& artifact_ptr : artifacts)
    {
        if (artifact_ptr)
        {
            artifact_ptr->tapped = false;
        }
    }

    // Draw Card
    player.draw();
}
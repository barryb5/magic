#pragma once

#include <string>
#include <vector>
#include <memory>
#include "card.hpp"

struct Stats
{
    int health_change = 0;
    int cards_drawn = 0;
    int experience_counters = 0;
};

struct Response
{
    std::vector<std::shared_ptr<Card>> cards_played;
    std::vector<std::shared_ptr<Card>> cards_removed;
    Stats stats;
    std::unordered_map<std::string, int> mana_pool;
    std::unordered_map<std::string, int> mana_available;
};

void from_json(const nlohmann::json &j, Response &r, std::vector<std::shared_ptr<Card>> &card_library);

std::string trim(std::string s);
std::string getCardName(std::string card_str);
int tokenValue(std::string tok);
bool isDigits(const std::string &s);
std::vector<std::shared_ptr<Card>> load_cards_to_deck(const std::string &path);

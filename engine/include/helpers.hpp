#pragma once

#include <string>
#include <vector>
#include <memory>
#include "card.hpp"

std::string trim(std::string s);
int tokenValue(std::string tok);
bool isDigits(const std::string &s);
std::vector<std::shared_ptr<Card>> load_cards_to_deck(const std::string &path);

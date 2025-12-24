#include "../include/card.hpp"

#include "../include/helpers.hpp"
#include <string>
#include <cctype>
#include <algorithm>
#include <iostream>

void Card::printNameAndOracle() const
{
    std::cout << name << " - " << oracle_text << "\n";
}

std::string Card::printForLLM() const
{
    std::string result = name + "|";

    for (const auto& type : type_line)
    {
        result += card_type_to_string(type) + ",";
    }
    result.pop_back();

    if (!oracle_text.empty())
        result += "|" + oracle_text;
    
    if (!mana_cost.empty())
        result += "|" + mana_cost;
        
    result += "|";

    for (const auto& kw : keywords)
    {
        result += kw + ",";
    }
    
    result.pop_back();

    if (tapped)
    {
        result += "|tapped";
    }
    else
    {
        result += "|untapped";
    }

    if (isCreature())
    {
        if (power.has_value() && toughness.has_value())
        {
            result += "|" + std::to_string(*power) + "|" + std::to_string(*toughness);
        }
        result += summoning_sick ? "|summoning_sick" : "|not_summoning_sick";
    }

    return result;
}

bool Card::isCreature() const
{
    return type_line.find(CREATURE) != type_line.end();
}

int Card::getRawCost() const
{
    int total = 0;
    for (size_t i = 0; i < mana_cost.size();)
    {
        if (mana_cost[i] != '{')
        {
            ++i;
            continue;
        }
        size_t j = mana_cost.find('}', i + 1);
        if (j == std::string::npos)
            break; // malformed input, stop
        total += tokenValue(mana_cost.substr(i + 1, j - (i + 1)));
        i = j + 1;
    }
    return total;
}

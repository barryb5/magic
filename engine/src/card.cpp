#include "../include/card.hpp"

#include "../include/helpers.hpp"
#include <string>
#include <cctype>
#include <algorithm>

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
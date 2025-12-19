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

inline void from_json(const json &j, Card &c)
{
    c.name = j.value("name", "");
    c.id = j.value("id", "");
    c.mana_cost = j.value("mana_cost", "");
    c.oracle_text = j.value("oracle_text", "");

    const std::string type_line_str = j.value("type_line", "");
    c.type_line = parse_type_line(type_line_str);

    c.power = get_optional_int(j, "power");
    c.toughness = get_optional_int(j, "toughness");

    c.keywords.clear();
    if (j.contains("keywords") && j["keywords"].is_array())
    {
        c.keywords = j["keywords"].get<std::vector<std::string>>();
    }

    c.all_parts.clear();
    if (j.contains("all_parts") && j["all_parts"].is_array())
    {
        for (const auto &part : j["all_parts"])
        {
            if (part.is_string())
            {
                c.all_parts.push_back(part.get<std::string>());
            }
            else if (part.is_object())
            {
                if (part.contains("id") && part["id"].is_string())
                    c.all_parts.push_back(part["id"].get<std::string>());
                else if (part.contains("name") && part["name"].is_string())
                    c.all_parts.push_back(part["name"].get<std::string>());
            }
        }
    }
}
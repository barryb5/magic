#include "../include/helpers.hpp"

#include <string>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include "../include/card.hpp"

void from_json(const nlohmann::json &j, Response &r, std::vector<std::shared_ptr<Card>> &card_library)
{
    r.cards_played.clear();
    if (j.contains("Cards Played") && j["Cards Played"].is_array())
    {
        for (const auto &card_j : j["Cards Played"])
        {
            if (!card_j.is_string())
                continue;
            std::string card_str = card_j.get<std::string>();

            std::string card_name = getCardName(card_str);
            auto it = std::find_if(card_library.begin(), card_library.end(),
                                    [&card_name](const std::shared_ptr<Card> &c)
                                    { return c->name == card_name; });
            if (it != card_library.end())
            {
                r.cards_played.push_back(*it);
            }
        }
    }

    r.cards_removed.clear();
    if (j.contains("Cards Removed") && j["Cards Removed"].is_array())
    {
        for (const auto &card_j : j["Cards Removed"])
        {
            if (!card_j.is_string())
                continue;

            std::string card_str = card_j.get<std::string>();

            std::string card_name = getCardName(card_str);
            auto it = std::find_if(card_library.begin(), card_library.end(),
                                    [&card_name](const std::shared_ptr<Card> &c)
                                    { return c->name == card_name; });
            if (it != card_library.end())
            {
                r.cards_removed.push_back(*it);
            }
        }
    }

    if (j.contains("Stats") && j["Stats"].is_object())
    {
        const auto &stats_json = j["Stats"];
        r.stats.health_change = stats_json.value("Health Change", 0);
        r.stats.cards_drawn = stats_json.value("Cards Drawn", 0);
        r.stats.experience_counters = stats_json.value("Experience Counters", 0);
    }

    r.mana_pool.clear();
    if (j.contains("Mana Pool") && j["Mana Pool"].is_array())
    {
        for (const auto &mana : j["Mana Pool"])
        {
            if (!obj.is_object())
                continue;
            for (auto it = obj.begin(); it != obj.end(); ++it)
            {
                if (it.value().is_number_integer())
                    r.mana_pool[it.key()] = it.value().get<int>();
            }
        }
    }

    r.mana_available.clear();
    if (j.contains("Mana Available") && j["Mana Available"].is_array())
    {
        for (const auto &mana : j["Mana Available"])
        {
            if (!mana.is_object())
                continue;
            for (auto it = mana.begin(); it != mana.end(); ++it)
            {
                if (it.value().is_number_integer())
                {
                    r.mana_available[it.key()] = it.value().get<int>();
                }
            }
        }
    }
}

std::string trim(std::string s)
{
    auto notSpace = [](unsigned char c)
    { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

std::string getCardName(std::string card_str)
{
    size_t id_pos = card_str.find('|');
    if (id_pos != std::string::npos)
    {
        return trim(card_str.substr(0, id_pos));
    }
    return trim(card_str);
}

int tokenValue(std::string tok)
{
    tok = trim(tok);
    if (tok.empty())
        return 0;

    // {X} -> 0
    if (tok == "X" || tok == "x")
        return 0;

    // Pure number like {2}, {10}
    if (isDigits(tok))
        return std::stoi(tok);

    // Hybrid / phyrexian tokens (e.g. "2/U", "W/U", "U/P")
    auto slashPos = tok.find('/');
    if (slashPos != std::string::npos)
    {
        std::string left = tok.substr(0, slashPos);
        std::string right = tok.substr(slashPos + 1);

        left = trim(left);
        right = trim(right);

        // If one side is a number (e.g. "2/U"), count that number
        if (isDigits(left))
            return std::stoi(left);
        if (isDigits(right))
            return std::stoi(right);

        // Otherwise (e.g. "W/U" or "U/P"), count as 1
        return 1;
    }

    // Any other symbol counts as 1 (U/G/W/B/R/C/S, etc.)
    return 1;
}

bool isDigits(const std::string &s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(),
                                     [](unsigned char c)
                                     { return std::isdigit(c); });
}

std::vector<std::shared_ptr<Card>> load_cards_to_deck(const std::string& path)
{
    std::vector<std::shared_ptr<Card>> deck;

    std::ifstream library(path);
    if (!library.is_open())
    {
        std::cerr << "Error opening file: " << path << "\n";
        return deck;
    }

    // Read entire file into a string (best for JSON array files)
    const std::string content((std::istreambuf_iterator<char>(library)),
                               std::istreambuf_iterator<char>());

    try
    {
        json j = json::parse(content);

        if (!j.is_array())
        {
            std::cerr << "Expected top-level JSON array in: " << path << "\n";
            return deck;
        }

        // Reserve based on sum of counts (optional, but avoids reallocs)
        size_t total_cards = 0;
        for (const auto& entry : j)
        {
            if (entry.is_object())
            {
                int c = entry.value("count", 1);
                if (c < 1) c = 1;
                total_cards += static_cast<size_t>(c);
            }
        }
        deck.reserve(total_cards);

        // Parse each entry and expand by count
        for (size_t i = 0; i < j.size(); ++i)
        {
            const auto& item = j[i];

            if (!item.is_object())
            {
                std::cerr << "Entry " << i << " is not an object; skipping.\n";
                continue;
            }

            try
            {
                DeckEntry e = item.get<DeckEntry>();

                if (e.count < 1)
                {
                    std::cerr << "Entry " << i << " has invalid count (" << e.count
                              << "); treating as 1.\n";
                    e.count = 1;
                }

                // Optional sanity check: if name/id missing, warn
                if (e.card.name.empty() || e.card.id.empty())
                {
                    std::cerr << "Warning: Entry " << i
                              << " parsed a card with missing name/id (name=\""
                              << e.card.name << "\", id=\"" << e.card.id << "\").\n";
                }

                for (int n = 0; n < e.count; ++n)
                {
                    deck.push_back(std::make_shared<Card>(e.card)); // copy base card
                }
            }
            catch (const json::out_of_range& ex)
            {
                // e.g. missing "card" because you used j.at("card") in from_json
                std::cerr << "Entry " << i << " missing required field: " << ex.what()
                          << " ; skipping.\n";
            }
            catch (const json::type_error& ex)
            {
                std::cerr << "Entry " << i << " has wrong type: " << ex.what()
                          << " ; skipping.\n";
            }
        }

        return deck;
    }
    catch (const json::parse_error& e)
    {
        std::cerr << "JSON parse error in " << path << ": " << e.what() << "\n";
        return deck;
    }
    catch (const json::exception& e)
    {
        // Any other nlohmann::json exception
        std::cerr << "JSON error in " << path << ": " << e.what() << "\n";
        return deck;
    }
}

#include "../include/helpers.hpp"

#include <string>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include "../include/card.hpp"

std::string trim(std::string s)
{
    auto notSpace = [](unsigned char c)
    { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
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

std::vector<std::shared_ptr<Card>> load_cards_to_deck(const std::string &path)
{
    std::vector<std::shared_ptr<Card>> deck;

    std::ifstream library(path);
    if (!library.is_open())
    {
        std::cerr << "Error opening file: " << path << "\n";
        return deck;
    }

    // Read entire file into a string first (best for JSON array files)
    std::string content((std::istreambuf_iterator<char>(library)),
                        std::istreambuf_iterator<char>());

    // Try parsing as a single JSON document (array or object)
    try
    {
        json j = json::parse(content);

        if (j.is_array())
        {
            deck.reserve(j.size());
            for (const auto &item : j)
            {
                // Uses your from_json(json, Card&)
                Card c = item.get<Card>();
                deck.push_back(std::make_shared<Card>(std::move(c)));
            }
        }
        else if (j.is_object())
        {
            Card c = j.get<Card>();
            deck.push_back(std::make_shared<Card>(std::move(c)));
        }
        else
        {
            std::cerr << "Top-level JSON is not an array or object.\n";
        }

        return deck;
    }
    catch (const json::parse_error &)
    {
        // If parsing whole file fails, fall back to JSONL (one object per line).
    }

    library.close();
    library.open(path);
    if (!library.is_open())
    {
        std::cerr << "Error reopening file: " << path << "\n";
        return deck;
    }

    std::string line;
    size_t line_no = 0;
    while (std::getline(library, line))
    {
        ++line_no;
        if (line.empty())
            continue;

        try
        {
            json jline = json::parse(line);
            if (!jline.is_object())
            {
                std::cerr << "Line " << line_no << " is not a JSON object; skipping.\n";
                continue;
            }

            Card c = jline.get<Card>();
            deck.push_back(std::make_shared<Card>(std::move(c)));
        }
        catch (const json::parse_error &e)
        {
            std::cerr << "JSON parse error on line " << line_no << ": " << e.what() << "\n";
        }
        catch (const json::type_error &e)
        {
            std::cerr << "JSON type error on line " << line_no << ": " << e.what() << "\n";
        }
    }

    return deck;
}
#include "../include/helpers.hpp"

#include <string>
#include <algorithm>

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
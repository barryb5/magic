#pragma once

#include <string>
#include <vector>
#include <optional>
#include <set>
#include <nlohmann/json.hpp>

using nlohmann::json;

enum CardType
{
    CREATURE,
    SORCERY,
    INSTANT,
    ENCHANTMENT,
    ARTIFACT,
    PLANESWALKER,
    LAND
};

static inline bool contains_wordish(const std::string &s, const std::string &needle)
{
    return s.find(needle) != std::string::npos;
}

static std::set<CardType> parse_type_line(const std::string &type_line)
{
    std::set<CardType> types;

    if (contains_wordish(type_line, "Creature"))
        types.insert(CREATURE);
    if (contains_wordish(type_line, "Sorcery"))
        types.insert(SORCERY);
    if (contains_wordish(type_line, "Instant"))
        types.insert(INSTANT);
    if (contains_wordish(type_line, "Enchantment"))
        types.insert(ENCHANTMENT);
    if (contains_wordish(type_line, "Artifact"))
        types.insert(ARTIFACT);
    if (contains_wordish(type_line, "Planeswalker"))
        types.insert(PLANESWALKER);
    if (contains_wordish(type_line, "Land"))
        types.insert(LAND);

    return types;
}

static std::optional<int> get_optional_int(const json& j, const char* key) {
    auto it = j.find(key);
    if (it == j.end() || it->is_null()) return std::nullopt;
    return it->get<int>();
}

class Card
{
public:
    std::string name;
    std::string id;
    std::string mana_cost;
    std::set<CardType> type_line;
    std::string oracle_text;
    std::optional<int> power;
    std::optional<int> toughness;
    std::vector<std::string> keywords;
    std::vector<std::string> all_parts;

    Card(std::string name, std::string id, std::string mana_cost,
         std::set<CardType> type_line, std::string oracle_text,
         std::optional<int> power, std::optional<int> toughness,
         std::vector<std::string> keywords,
         std::vector<std::string> all_parts)
        : name(name), id(id), mana_cost(mana_cost), type_line(type_line),
          oracle_text(oracle_text), power(power), toughness(toughness),
          keywords(keywords), all_parts(all_parts) {};
    Card() = default;

    bool operator==(const Card &other) const { return id == other.id; }

    bool isCreature() const;
    int getRawCost() const;
    void printNameAndOracle() const;
};

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
        c.keywords = j["keywords"].get<std::vector<std::string>>();

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

struct DeckEntry {
    int count = 1;
    Card card;
};

inline void from_json(const json& j, DeckEntry& e) {
    e.count = j.value("count", 1);
    e.card  = j.at("card").get<Card>();
}
#include <string>
#include <vector>
#include <optional>
#include <set>

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
    ~Card();

    bool isCreature() const;
};
#include "player.hpp"
#include "card.hpp"
#include <string>
#include <iostream>

class BoardState
{
public:
    Player player;
    size_t turn_number = 0;
    size_t experience_counters = 0;

    std::vector<std::shared_ptr<Card>> lands{};
    std::vector<std::shared_ptr<Card>> creatures{};
    std::vector<std::shared_ptr<Card>> enchantments{};
    std::vector<std::shared_ptr<Card>> artifacts{};
    std::unordered_map<Color, int> manaPool{};

    BoardState(Player player) : player(std::move(player)) {}

    void printBoardState() const;
    std::string printBoardStateForLLM();
    bool addPermanentToBattlefield(const std::shared_ptr<Card>& card);
    void playPermanent(const std::shared_ptr<Card>& card);
    void playPermanent(const std::string& card);

    void nextTurn();
};
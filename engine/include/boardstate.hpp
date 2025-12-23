#include "player.hpp"
#include "card.hpp"
#include <string>
#include <iostream>

class BoardState
{
public:
    Player player;

    std::vector<std::shared_ptr<Card>> lands{};
    std::vector<std::shared_ptr<Card>> creatures{};
    std::vector<std::shared_ptr<Card>> enchantments{};
    std::vector<std::shared_ptr<Card>> artifacts{};
    std::unordered_map<Color, int> manaPool{};

    BoardState(Player player) : player(std::move(player)) {}

    void printBoardState() const;
    bool addPermanentToBattlefield(const std::shared_ptr<Card>& card);
    void playPermanent(const std::shared_ptr<Card>& card);
    void playPermanent(const std::string& card);
};
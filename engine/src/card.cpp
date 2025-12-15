#include "../include/card.hpp"

bool Card::isCreature() const
{
    return type_line.find(CREATURE) != type_line.end();
}
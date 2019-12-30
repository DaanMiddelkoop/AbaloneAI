#include "team.h"

Team::Team(sf::Color color)
{
    this->color = color;
    this->board = 0;
}

sf::Color Team::getColor() {
    return this->color;
}

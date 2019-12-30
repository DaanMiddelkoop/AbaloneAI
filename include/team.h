#ifndef TEAM_H
#define TEAM_H

#include <cstdint>

#include <SFML/Graphics.hpp>

#include "BitBoard.h"

typedef unsigned __int128 uint128_t;

class Team
{
    public:
        Team(sf::Color color);

        sf::Color getColor();

        uint128_t board;

    protected:

    private:
        sf::Color color;
};

#endif // TEAM_H

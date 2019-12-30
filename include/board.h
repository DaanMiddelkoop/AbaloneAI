#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "tile.h"
#include "move.h"
#include "team.h"

class Controller
{
    public:
        Controller();

        void draw(sf::RenderWindow* window);
        void addTeam(sf::Color color);
        void setup();
        int getBumps(Team* team, uint128_t* buffer, int buffer_len);
        int getMoves(Team* team, uint128_t* buffer, int buffer_len);

        bool checkScoreAndCleanEdges(uint128_t* board);


        std::vector<Team> teams;
};


#endif // BOARD_H

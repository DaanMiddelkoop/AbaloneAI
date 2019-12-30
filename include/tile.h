#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>

#include "team.h"

class Tile
{
    public:
        Tile();
        Tile(int i);
        sf::Color getColor();

        void setTeam(Team* team);
        Team* getTeam();

        int getPos();

        void setLeft(Tile*);
        void setRight(Tile*);
        void setUpLeft(Tile*);
        void setUpRight(Tile*);
        void setDownLeft(Tile*);
        void setDownRight(Tile*);

        Tile* getLeft();
        Tile* getRight();
        Tile* getUpLeft();
        Tile* getUpRight();
        Tile* getDownLeft();
        Tile* getDownRight();

        Tile* links[6] = {NULL};
    protected:

    private:
        Team* team;
        int pos;
};

#endif // TILE_H

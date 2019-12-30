#include "tile.h"

Tile::Tile() {

}

Tile::Tile(int i) {
    this->team = NULL;
    this->pos = i;
}

void Tile::setTeam(Team* team) {
    this->team = team;
}

Team* Tile::getTeam() {
    return this->team;
}

sf::Color Tile::getColor() {
    if (this->team == NULL) {
        return sf::Color(139, 69, 19);
    }
    return this->team->getColor();
}

int Tile::getPos() {
    return this->pos;
}

void Tile::setLeft(Tile* tile) {
    this->links[0] = tile;
}

void Tile::setRight(Tile* tile) {
    this->links[3] = tile;
}

void Tile::setUpLeft(Tile* tile) {
    this->links[1] = tile;
}

void Tile::setUpRight(Tile* tile) {
    this->links[2] = tile;
}

void Tile::setDownLeft(Tile* tile) {
    this->links[5] = tile;
}

void Tile::setDownRight(Tile* tile) {
    this->links[4] = tile;
}

Tile* Tile::getLeft() {
    return this->links[0];
}

Tile* Tile::getRight() {
    return this->links[3];
}

Tile* Tile::getUpLeft() {
    return this->links[1];
}

Tile* Tile::getUpRight() {
    return this->links[2];
}

Tile* Tile::getDownLeft() {
    return this->links[5];
}

Tile* Tile::getDownRight() {
    return this->links[4];
}



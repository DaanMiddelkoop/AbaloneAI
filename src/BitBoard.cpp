#include "BitBoard.h"

BitBoard::BitBoard()
{
    p1 = 0;
    p2 = 0;
    p3 = 0;
}

BitBoard::BitBoard(uint64_t p1, uint64_t p2, uint64_t p3) {
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
}

BitBoard BitBoard::op_and(BitBoard* other) {
    return BitBoard(p1 & other->p1, p2 & other->p2, p3 & other->p3);
}

BitBoard BitBoard::op_or(BitBoard* other) {
    return BitBoard(p1 | other->p1, p2 | other->p2, p3 | other->p3);
}

BitBoard BitBoard::op_shift(int amount) {
    return BitBoard((p1 >> amount) | (p2 >> (64 + amount)), (p2 >> amount) | (p3 >> (64 + amount)) | (p1 >> (-64 - amount)), (p3 >> amount) | (p2 >> (-64 - amount)));
}

bool BitBoard::get(int x, int y) {
    int index = x * 13 + y;
}

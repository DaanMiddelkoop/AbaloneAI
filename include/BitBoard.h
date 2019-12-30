#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>

class BitBoard
{
    public:
        BitBoard();
        BitBoard(uint64_t p1, uint64_t p2, uint64_t p3);

        BitBoard op_and(BitBoard* other);
        BitBoard op_or(BitBoard* other);
        BitBoard op_shift(int num);

        bool get(int x, int y);



    protected:

    private:
        uint64_t p1;
        uint64_t p2;
        uint64_t p3;
};

#endif // BITBOARD_H

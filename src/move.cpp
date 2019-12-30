#include "move.h"

Move::Move()
{
    for(int i = 0; i < 4; i++) {
        old_positions[i] = -1;
        new_positions[i] = -1;
    }

    score = false;
}

#ifndef AI_H
#define AI_H

#include "board.h"
#include "team.h"

class AI
{
    public:
        AI();
        void bestmove(Team* team, Team* enemy, Controller* controller, int total_team_score, int total_enemy_score, uint128_t* best_move, uint128_t* new_enemy_board);
        int minimax(Team* team, Team* enemy, Controller* controller, int total_team_score, int total_enemy_score, int depth);

    protected:

    private:
};

#endif // AI_H

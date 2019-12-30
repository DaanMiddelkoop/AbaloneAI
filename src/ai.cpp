#include <iostream>

#include "ai.h"

AI::AI()
{
    //ctor
}

void AI::bestmove(Team* team, Team* enemy, Controller* controller, int total_team_score, int total_enemy_score, uint128_t* best_move, uint128_t* new_enemy_board) {
    uint128_t possible_bumps[100];
    uint128_t possible_moves[100];
    int total_bumps = controller->getBumps(team, possible_bumps, sizeof(possible_bumps));
    int total_moves = controller->getMoves(team, possible_moves, sizeof(possible_moves));

    uint128_t currentBoard = team->board;
    uint128_t enemyBoard = enemy->board;

    int best_score = -999999;

    if (total_bumps + total_moves <= 0) {
        std::cout << "No moves to start wit" << std::endl;
    }

    for (int m = 0; m < total_bumps; m += 2) {
        team->board = possible_bumps[m];
        enemy->board = possible_bumps[m + 1];

        int scored = controller->checkScoreAndCleanEdges(&enemy->board) ? 1 : 0;

        int score = -AI::minimax(enemy, team, controller, total_enemy_score, total_team_score + scored, 0) + scored;

        if (score > best_score) {
            best_score = score;
            *best_move = team->board;
            *new_enemy_board = possible_bumps[m + 1];
        }
    }

    for (int m = 0; m < total_moves; m++) {
        team->board = possible_moves[m];

        std::cout << "Testing possible moves: " << std::endl;
        controller->printBoard(team->board);

        int score = -AI::minimax(enemy, team, controller, total_enemy_score, total_team_score, 0);

        if (score > best_score) {
            best_score = score;
            *best_move = team->board;
            *new_enemy_board = enemyBoard;
        }
    }

    team->board = currentBoard;
    enemy->board = enemyBoard;
}


int AI::minimax(Team* team, Team* enemy, Controller* controller, int total_team_score, int total_enemy_score, int depth) {
    if (depth > 1) {
        return 0;
    }


    if (total_enemy_score >= 6) {
        return -1000;
    }


    uint128_t possible_bumps[100];
    uint128_t possible_moves[100];
    int total_bumps = controller->getBumps(team, possible_bumps, sizeof(possible_bumps));
    int total_moves = controller->getMoves(team, possible_moves, sizeof(possible_moves));

    uint128_t currentBoard = team->board;
    uint128_t enemyBoard = enemy->board;

    int best_score = -999999;

    if (total_bumps + total_moves <= 0) {
        //std::cout << "NO MORE POSSIBLE MOVES, GHALP" << std::endl;
    }

    for (int m = 0; m < total_bumps; m += 2) {
        team->board = possible_bumps[m];
        enemy->board = possible_bumps[m + 1];

        int scored = controller->checkScoreAndCleanEdges(&enemy->board) ? 1 : 0;

        int score = -AI::minimax(enemy, team, controller, total_enemy_score, total_team_score + scored, depth + 1) + scored;

        if (score > best_score) {
            best_score = score;
        }
    }

    for (int m = 0; m < total_moves; m++) {
        team->board = possible_moves[m];

//         std::cout << "Testing possible moves: " << std::endl;
//        controller->printBoard(team->board);

        int score = -AI::minimax(enemy, team, controller, total_enemy_score, total_team_score, depth + 1);

        if (score > best_score) {
            best_score = score;
        }
    }

    team->board = currentBoard;
    enemy->board = enemyBoard;

    return best_score;
}








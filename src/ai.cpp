#include <iostream>
#include <unordered_map>

#include "ai.h"
#include <assert.h>

static int ROW_SIZE = 11;

const char MAX_DEPTH = 4;

#define ls(X, Y) ((Y) > 0 ? (X) >> (Y) : (X) << (-Y))

const __uint128_t HOTSPOTS =
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 9)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 8)  +
        (((__uint128_t) 0b00000111000) << ROW_SIZE * 7)  +
        (((__uint128_t) 0b00001111000) << ROW_SIZE * 6)  +
        (((__uint128_t) 0b00011111000) << ROW_SIZE * 5)  +
        (((__uint128_t) 0b00011110000) << ROW_SIZE * 4)  +
        (((__uint128_t) 0b00011100000) << ROW_SIZE * 3)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 2)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 1);

const __uint128_t OUTER_RING =
        (((__uint128_t) 0b00000111110) << ROW_SIZE * 9)  +
        (((__uint128_t) 0b00001000010) << ROW_SIZE * 8)  +
        (((__uint128_t) 0b00010000010) << ROW_SIZE * 7)  +
        (((__uint128_t) 0b00100000010) << ROW_SIZE * 6)  +
        (((__uint128_t) 0b01000000010) << ROW_SIZE * 5)  +
        (((__uint128_t) 0b01000000100) << ROW_SIZE * 4)  +
        (((__uint128_t) 0b01000001000) << ROW_SIZE * 3)  +
        (((__uint128_t) 0b01000010000) << ROW_SIZE * 2)  +
        (((__uint128_t) 0b01111100000) << ROW_SIZE * 1);

const __uint128_t RING0 =
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 9)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 8)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 7)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 6)  +
        (((__uint128_t) 0b00000100000) << ROW_SIZE * 5)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 4)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 3)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 2)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 1);

const __uint128_t RING1 =
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 9)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 8)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 7)  +
        (((__uint128_t) 0b00000110000) << ROW_SIZE * 6)  +
        (((__uint128_t) 0b00001010000) << ROW_SIZE * 5)  +
        (((__uint128_t) 0b00001100000) << ROW_SIZE * 4)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 3)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 2)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 1);

const __uint128_t RING2 =
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 9)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 8)  +
        (((__uint128_t) 0b00000111000) << ROW_SIZE * 7)  +
        (((__uint128_t) 0b00001001000) << ROW_SIZE * 6)  +
        (((__uint128_t) 0b00010001000) << ROW_SIZE * 5)  +
        (((__uint128_t) 0b00010010000) << ROW_SIZE * 4)  +
        (((__uint128_t) 0b00011100000) << ROW_SIZE * 3)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 2)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 1);

const __uint128_t RING3 =
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 9)  +
        (((__uint128_t) 0b00000111100) << ROW_SIZE * 8)  +
        (((__uint128_t) 0b00001000100) << ROW_SIZE * 7)  +
        (((__uint128_t) 0b00010000100) << ROW_SIZE * 6)  +
        (((__uint128_t) 0b00100000100) << ROW_SIZE * 5)  +
        (((__uint128_t) 0b00100001000) << ROW_SIZE * 4)  +
        (((__uint128_t) 0b00100010000) << ROW_SIZE * 3)  +
        (((__uint128_t) 0b00111100000) << ROW_SIZE * 2)  +
        (((__uint128_t) 0b00000000000) << ROW_SIZE * 1);

const uint128_t rings[] = {RING0, RING1, RING2, RING3, OUTER_RING};

struct TranspositionEntry {
    uint128_t team;
    uint128_t enemy;

    bool operator==(const TranspositionEntry &other) const
  { return (team == other.team
            && enemy == other.enemy);
  }
};

struct TranspositionResult {
    int score;
    char depth;
    union {
        char alphabetabreak;
        struct {
            unsigned char alpha: 1;
            unsigned char beta: 1;
        };
    };
};

template<>
struct std::hash<TranspositionEntry>
{
std::size_t operator()(const TranspositionEntry& k) const
{
  using std::size_t;
  using std::hash;
  using std::string;

  // Compute individual hash values for first,
  // second and third and combine them using XOR
  // and bit shifting:

  return ((std::hash<uint128_t>()(k.team)
           ^ (std::hash<uint128_t>()(k.enemy) << 1)) >> 1);
}
};

static std::unordered_map<TranspositionEntry, TranspositionResult> transposition_table;


int count_bits(uint128_t board) {
    return __builtin_popcountll(board) + __builtin_popcountll(board >> 64);
}

int get_center_heuristic(uint128_t board) {
    int score = 0;
    for (int i = 0; i < 5; i++) {
        score += count_bits(board & rings[i]) * ((5 - i) * (5 - i));
    }
    return score;
}

int heuristic_score(uint128_t team, uint128_t enemy) {
    return get_center_heuristic(team) - get_center_heuristic(enemy);
}

bool transposition_lookup(uint128_t team, uint128_t enemy, TranspositionResult* result) {
    TranspositionEntry key = {
        .team = team,
        .enemy = enemy,
    };

    std::unordered_map<TranspositionEntry,TranspositionResult>::const_iterator value = transposition_table.find (key);

    if (value == transposition_table.end()) {
        return false;
    }

    *result = value->second;
    return true;
}

void add_to_transposition_table(uint128_t team, uint128_t enemy, bool alpha_break, bool beta_break, int score, char depth) {

    struct TranspositionEntry te = {
        .team = team,
        .enemy = enemy,
    };

    struct TranspositionResult tr = {
        .score = score,
        .depth = depth,
    };

    tr.alpha = alpha_break;
    tr.beta = beta_break;

    // insert.
    transposition_table[te] = tr;
}





AI::AI()
{
    //ctor
}

void AI::bestmove(Team* team, Team* enemy, Controller* controller, int total_team_score, int total_enemy_score, uint128_t* best_move, uint128_t* new_enemy_board) {
    uint128_t possible_moves[600];
    int total_moves = controller->getBumps(team, possible_moves, 600);
    total_moves += controller->getMoves(team, possible_moves + (total_moves), 600 - (total_moves));

    std::cerr << "Score of current board before making a move " << heuristic_score(team->board, enemy->board) << std::endl;

    uint128_t currentBoard = team->board;
    uint128_t enemyBoard = enemy->board;


    for (int depth = 1; depth <= MAX_DEPTH; depth++) {
        int best_score = -999999;

        if (total_moves <= 0) {
            std::cerr << "No moves to start wit" << std::endl;
        }

        std::cerr << "Iterative deepening: " << depth << std::endl;


        for (int m = 0; m < total_moves; m += 2) {
            team->board = possible_moves[m];
            enemy->board = possible_moves[m + 1];

            int scored = controller->checkScoreAndCleanEdges(&enemy->board) ? 1 : 0;

            int score = -AI::minimax(enemy, team, controller, total_enemy_score, total_team_score + scored, depth, -99999, 99999) + scored;

            if (score > best_score) {
                best_score = score;
                *best_move = team->board;
                *new_enemy_board = possible_moves[m + 1];
            }
        }

        std::cerr << "move score: " << best_score << std::endl;

        team->board = currentBoard;
        enemy->board = enemyBoard;
    }
}


int AI::minimax(Team* team, Team* enemy, Controller* controller, int total_team_score, int total_enemy_score, int depth, int alpha, int beta) {
    // Check transposition table.
    int originalAlpha = alpha;

    TranspositionResult tr_result;
    if (transposition_lookup(team->board, enemy->board, &tr_result) && (tr_result.depth >= depth)) {

        if (tr_result.alphabetabreak == 0) {
            return tr_result.score;
        } else if (tr_result.alpha != 0) {
            alpha = std::max(alpha, tr_result.score);
        } else {
            beta = std::min(beta, tr_result.score);
        }

        if (alpha > beta) {

            //std::cerr << depth << " Found the move within the transposition table" << std::endl;
            return tr_result.score;
        }
    }





    if (depth == 0) {
        return heuristic_score(team->board, enemy->board) + ((total_team_score - total_enemy_score) * 100);
    }


    if (total_enemy_score >= 6) {
        return -1000;
    }

    uint128_t possible_moves[1000];
    int total_moves = controller->getBumps(team, possible_moves, 1000);
    total_moves += controller->getMoves(team, possible_moves + (total_moves), 1000 - (total_moves));

    assert(total_moves < 1000);

    uint128_t currentBoard = team->board;
    uint128_t enemyBoard = enemy->board;

    int best_score = -999999;


    for (int m = 0; m < total_moves; m += 2) {
        team->board = possible_moves[m];
        enemy->board = possible_moves[m + 1];

        int scored = controller->checkScoreAndCleanEdges(&enemy->board) ? 1 : 0;

        int score = -AI::minimax(enemy, team, controller, total_enemy_score, total_team_score + scored, depth - 1, -beta, -alpha) + scored;

        if (score > alpha) {
            alpha = score;
            if (alpha >= beta) {
                team->board = currentBoard;
                enemy->board = enemyBoard;

                add_to_transposition_table(team->board, enemy->board, score <= originalAlpha, !(score <= originalAlpha) && (score >= beta), score, depth);
                return score;
            }
        }



        if (score > best_score) {
            best_score = score;
        }
    }

    team->board = currentBoard;
    enemy->board = enemyBoard;
    add_to_transposition_table(team->board, enemy->board, best_score <= originalAlpha, !(best_score <= originalAlpha) && (best_score >= beta), best_score, depth);

    return best_score;
}








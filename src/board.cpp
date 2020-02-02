#include <iostream>
#include <math.h>
#include <string>
#include <assert.h>

#include "board.h"
#include "BitBoard.h"

typedef unsigned __int128 uint128_t;

static const uint128_t P1_SETUP = (uint128_t)0b1111100000111111000000111 << 80;
static const uint128_t P2_SETUP = (uint128_t)0b111000000111111000001111100 << 14;
static uint128_t EDGES = ((uint128_t)0x1FFFE0F81E03806 << 64) | 0xC0380F03E0FFFF;

static const int UP_LEFT = -1;
static const int UP_RIGHT = -10;
static const int RIGHT = 1;
static const int DOWN_RIGHT = 11;
static const int DOWN_LEFT = 10;
static const int LEFT = -1;

int count_bits2(uint128_t board) {
    return __builtin_popcountll(board) + __builtin_popcountll(board >> 64);
}

static const int DIRECTIONS[6] = {UP_LEFT, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN_LEFT, LEFT};

#define ls(X, Y) ((Y) > 0 ? (X) >> (Y) : (X) << (-Y))

Controller::Controller() {
    this->teams = std::vector<Team>();
}

void Controller::draw(sf::RenderWindow* window) {
    sf::CircleShape shape(10.f);


    int x_offset = 0;



    shape.setFillColor(sf::Color(139, 69, 19));

    for (int y = 0; y < 11; y++) {
        for (int x = 0; x < 11; x++) {

            if (~EDGES & ((uint128_t)1 << ((10 - y) * 11 + (10 - x) ))) {
                shape.setPosition(sf::Vector2f((x * 20) + x_offset, y * 20 + 100));
                window->draw(shape);
            }
        }

        x_offset += 10;
    }

    x_offset = 0;

    for (int y = 0; y < 11; y++) {
        for (int x = 0; x < 11; x++) {
            for (int t = 0; t < teams.size(); t++) {
                if (teams[t].board & ((uint128_t)1 << ((10 - y) * 11 + (10 - x) ))) {
                    shape.setFillColor(teams[t].getColor());
                    shape.setPosition(sf::Vector2f((x * 20) + x_offset, y * 20 + 100));
                    window->draw(shape);



                }
            }

        }
        x_offset += 10;
    }


    sf::Font font;
    if (!font.loadFromFile("Scotland.otf"))
    {
        assert(false);
    }

    sf::Text text;
    text.setFont(font);
    text.setString("P0: " + std::to_string(count_bits2(teams[0].board)));
    text.setCharacterSize(40);
    text.setPosition(400, 100);
    text.setFillColor(sf::Color(0, 0, 0));
    window->draw(text);

    text.setString("P1: " + std::to_string(count_bits2(teams[1].board)));
    text.setPosition(400, 160);
    window->draw(text);
}
void Controller::addTeam(sf::Color color) {
    this->teams.push_back(Team(color));
}

void Controller::setup() {

//    EDGES = 0;
//    int row_sizes[] = {6, 7, 8, 9, 10, 11, 10, 9, 8, 7, 6};
//    for (int x = 0; x < 11; x++) {
//        for (int y = 0; y < 11; y++) {
//            if ((x == 0) || (y == 0) || (x == 10) || (y == 10) || (x + y < 6) || (x + y > 14)) {
//                EDGES |= (uint128_t)1 << (10 - y) * 11 + (10 - x);
//            }
//        }
//    }
//
//    uint64_t low = (uint64_t)EDGES;
//    uint64_t high = (uint64_t)(EDGES >> 64);
//
//    char buf[100] = {0};
//    sprintf(buf, "%lX", high);
//    std::cout << "EDGES: " << buf << "    ";
//    sprintf(buf, "%lX", low);
//    std::cout << buf << std::endl;


    if (this->teams.size() == 2) {
        this->teams[0].board = P1_SETUP;
        this->teams[1].board = P2_SETUP;
    }


}

bool Controller::checkScoreAndCleanEdges(uint128_t* board) {
    bool score = false;
    if ((*board & EDGES) > 0) {
        score = true;
    }


    *board &= ~EDGES;

    return score;
}

int Controller::getBumps(Team* team, uint128_t* buffer, int buffer_len) {
    int pointer = 0;
    int team_id = team - &teams[0];
    uint128_t taken = 0;
    for (int t = 0; t < teams.size(); t++) {
        taken |= teams[t].board;
    }
    uint128_t move_grid;
    uint128_t new_board;
    uint128_t taken_with_borders = taken | EDGES | ((uint128_t)0b1111111 << 121);
    uint128_t enemies = taken ^ team->board;

    // Bump 2.
    for (int dir = 0; dir < 6; dir++) {
        move_grid = team->board & ls(team->board, DIRECTIONS[dir]) & ls(enemies, (DIRECTIONS[dir] * 2)) & ls(~taken, (DIRECTIONS[dir] * 3));
        while (move_grid > (uint128_t)0) {
            uint128_t next_move_grid = move_grid & (move_grid - 1);
            uint128_t single_move = move_grid ^ next_move_grid;
            move_grid = next_move_grid;

            new_board = team->board ^ single_move;
            new_board |= ls(single_move, (-DIRECTIONS[dir] * 2));

            buffer[pointer] = new_board;
            pointer += 1;


            for (int e = 0; e < teams.size(); e++) {
                if (e == team_id) {
                    continue;
                }

                if ((single_move & ls(teams[e].board, (DIRECTIONS[dir] * 2))) > (uint128_t)0) {
                    new_board = teams[e].board ^ ls(single_move, (-DIRECTIONS[dir] * 2));
                    new_board |= ls(single_move, (-DIRECTIONS[dir] * 3));
                    break;
                }
            }

            buffer[pointer] = new_board;
            pointer++;
        }


        // 3 units bumping 1.
        move_grid = team->board & ls(team->board, DIRECTIONS[dir]) & ls(team->board, (DIRECTIONS[dir] * 2)) & ls(enemies, (DIRECTIONS[dir] * 3)) & ls(~taken, (DIRECTIONS[dir] * 4));
        while (move_grid > (uint128_t)0) {
            uint128_t next_move_grid = move_grid & (move_grid - 1);
            uint128_t single_move = move_grid ^ next_move_grid;
            move_grid = next_move_grid;

            new_board = team->board ^ single_move;
            new_board |= ls(single_move, (-DIRECTIONS[dir] * 3));

            buffer[pointer] = new_board;
            pointer ++;

            for (int e = 0; e < teams.size(); e++) {
                if (e == team_id) {
                    continue;
                }
                if ((single_move & ls(teams[e].board, (DIRECTIONS[dir] * 3))) > (uint128_t)0) {
                    new_board = teams[e].board ^ ls(single_move, (-DIRECTIONS[dir] * 3));
                    new_board |= ls(single_move, (-DIRECTIONS[dir] * 4));
                    break;
                }
            }

            buffer[pointer] = new_board;
            pointer++;
        }

        // 3 units bumping 2.
        move_grid = team->board & ls(team->board, DIRECTIONS[dir]) & ls(team->board, DIRECTIONS[dir] * 2) & ls(enemies, DIRECTIONS[dir] * 3) & ls(enemies, DIRECTIONS[dir] * 4) & ls(~taken, DIRECTIONS[dir] * 5);
        while (move_grid > (uint128_t)0) {
            uint128_t next_move_grid = move_grid & (move_grid - 1);
            uint128_t single_move = move_grid ^ next_move_grid;
            move_grid = next_move_grid;

            int enemy_id = -1;
            for (int e = 0; e < teams.size(); e++) {
                if (((ls(single_move, (-DIRECTIONS[dir] * 3)) & teams[e].board) > 0) && ((ls(single_move, (-DIRECTIONS[dir] * 4)) & teams[e].board) > 0)) {
                    enemy_id = e;
                }
            }

            if (enemy_id == -1) {
                continue;
            }

            // Set board.
            new_board = team->board ^ single_move;
            new_board |= ls(single_move, (-DIRECTIONS[dir] * 3));

            buffer[pointer] = new_board;
            pointer ++;

            // Set enemy board.
            new_board = teams[enemy_id].board ^ ls(single_move, (-DIRECTIONS[dir] * 3));
            new_board |= ls(single_move, (-DIRECTIONS[dir] * 5));

            buffer[pointer] = new_board;
            pointer++;
        }


    }

    return pointer;
}

int Controller::getMoves(Team* team, uint128_t* buffer, const int buffer_len) {
    int team_id = team - &teams[0];
    int enemy_id = team_id == 0 ? 1 : 0;

    for (int i = 0; i < buffer_len / 2; i++) {
        buffer[i * 2 + 1] = teams[enemy_id].board;
    }

    int pointer = 0;
    uint128_t taken = 0;
    for (int t = 0; t < teams.size(); t++) {
        taken |= teams[t].board;
    }
    uint128_t move_grid;
    uint128_t new_board;
    uint128_t taken_with_borders = taken | EDGES | ((uint128_t)0b1111111 << 121);

    for (int dir = 0; dir < 6; dir++) {

        // Collect one moves
        move_grid = team->board & ls(~taken_with_borders, DIRECTIONS[dir]);

        while (move_grid > (uint128_t)0) {
            uint128_t next_move_grid = move_grid & (move_grid - 1);
            uint128_t single_move = move_grid ^ next_move_grid;
            move_grid = next_move_grid;

            new_board = team->board ^ single_move;
            new_board |= ls(single_move, -DIRECTIONS[dir]);

            buffer[pointer] = new_board;
            pointer += 2;
        }


        // Collect double moves.
        move_grid = team->board & ls(team->board, DIRECTIONS[dir]) & ls(~taken_with_borders, (DIRECTIONS[dir] * 2));
        while (move_grid > (uint128_t)0) {
            uint128_t next_move_grid = move_grid & (move_grid - 1);
            uint128_t single_move = move_grid ^ next_move_grid;
            move_grid = next_move_grid;
            new_board = team->board ^ single_move;

            new_board |= ls(single_move, -DIRECTIONS[dir]) | ls(single_move, (-DIRECTIONS[dir]*2));
            buffer[pointer] = new_board;
            pointer += 2;
        }



        // Collect tripple moves.
        move_grid = team->board & ls(team->board, DIRECTIONS[dir]) & ls(team->board, (DIRECTIONS[dir] * 2)) & ~ls(taken_with_borders, (DIRECTIONS[dir] * 3));
        while (move_grid > (uint128_t)0) {
            uint128_t next_move_grid = move_grid & (move_grid - 1);
            uint128_t single_move = move_grid ^ next_move_grid;
            move_grid = next_move_grid;
            new_board = team->board ^ single_move;

            new_board |= ls(single_move, (-DIRECTIONS[dir])) | ls(single_move, (-DIRECTIONS[dir] * 2)) | ls(single_move, (-DIRECTIONS[dir] * 3));
            buffer[pointer] = new_board;
            pointer += 2;
        }

        if (dir > 2) {
            continue;
        }



        // Collect sideways double move.
        for (int dir2 = dir + 1; (dir2 % 6) != dir; dir2 = (dir2 + 1) % 6) {
            move_grid = team->board & ls(team->board, DIRECTIONS[dir]) & ls(~taken_with_borders, DIRECTIONS[dir2]) & ls(~taken_with_borders, (DIRECTIONS[dir] + DIRECTIONS[dir2]));


            while (move_grid > (uint128_t)0) {
                uint128_t next_move_grid = move_grid & (move_grid - 1);
                uint128_t single_move = move_grid ^ next_move_grid;
                move_grid = next_move_grid;

                new_board = team->board ^ single_move;
                new_board ^= ls(single_move, -DIRECTIONS[dir]);
                new_board |= ls(single_move, -DIRECTIONS[dir2]) | ls(single_move, (-DIRECTIONS[dir] - DIRECTIONS[dir2]));
                buffer[pointer] = new_board;

                pointer += 2;
            }
        }


        // Collect sideways tripple moves.
        for (int dir2 = dir + 1; (dir2 % 6) != dir; (++dir2)%=6) {


            move_grid = team->board & ls(team->board, DIRECTIONS[dir]) & ls(team->board, (DIRECTIONS[dir] * 2)) & ls(~taken_with_borders, DIRECTIONS[dir2]) & ls(~taken_with_borders, (DIRECTIONS[dir] + DIRECTIONS[dir2])) & ls(~taken_with_borders, (DIRECTIONS[dir] * 2 + DIRECTIONS[dir2]));
            while (move_grid > (uint128_t)0) {
                uint128_t next_move_grid = move_grid & (move_grid - 1);
                uint128_t single_move = move_grid ^ next_move_grid;
                move_grid = next_move_grid;

                new_board = team->board ^ (single_move | ls(single_move, -DIRECTIONS[dir]) | ls(single_move, (-DIRECTIONS[dir] * 2)));
                new_board |= ls(single_move, -DIRECTIONS[dir2]) | ls(single_move, (-DIRECTIONS[dir] - DIRECTIONS[dir2])) | ls(single_move, (-(DIRECTIONS[dir] * 2) - DIRECTIONS[dir2]));

                buffer[pointer] = new_board;
                pointer += 2;
            }
        }
    }

    return pointer;
}

void Controller::printBoard(uint128_t board) {
    char offset = 0;

    for (int y = 0; y < 11; y++) {
        for (int s = 0; s < offset; s++) {
            std::cerr << " ";
        }

        for (int x = 0; x < 11; x++) {
            std::cerr << ((board & ((uint128_t)1 << ((10 - y) * 11 + (10 - x)))) > 0 ? "X " : "O ");
        }
        std::cerr << std::endl;
        offset++;
    }
}

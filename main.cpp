#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>

#include <SFML/Graphics.hpp>
#include "board.h"
#include "move.h"
#include "ai.h"

std::string uint128_str(__uint128_t v) {
    uint64_t high = v>>64;
    uint64_t low = v&(0xFFFFFFFFFFFFFFFF);
    std::ostringstream oss;
    oss << high << " " << low;
    return oss.str();
}

__uint128_t str_uint128(std::string v) {
    std::stringstream ss;
    ss << v;
    __uint128_t res = 0;
    uint64_t high, low;
    ss >> high >> low;
    res = high;
    return (res<<64) | low;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");

    Controller controller = Controller();

    controller.addTeam(sf::Color::White);
    controller.addTeam(sf::Color::Black);

    controller.setup();

    AI ai = AI();

    uint128_t moves[1000];
    //std::cout << "number of possible moves: " << controller.getMoves(&controller.teams[0], moves, 1000) << std::endl;



    bool next_move = false;
    int turn = 0;

    int p1_score = 0;
    int p2_score = 0;

    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    next_move = true;
                }
            }
        }

        if (next_move) {
            std::cerr << "start calculating move." << std::endl;
            if (turn == 0) {
                uint128_t best_p1;
                uint128_t result_p2;


                ai.bestmove(&controller.teams[0], &controller.teams[1], &controller, p1_score, p2_score, &best_p1, &result_p2);
                p1_score += controller.checkScoreAndCleanEdges(&result_p2) ? 1 : 0;
                controller.teams[0].board = best_p1;
                controller.teams[1].board = result_p2;
                next_move = true;
                std::cout << uint128_str(best_p1) << std::endl;
                std::cout << uint128_str(result_p2) << std::endl;

            } else {
                // We should read the move from the std in
                std::cerr << "Starting to receive stuff" << std::endl;
                std::string line;
                std::getline(std::cin, line);

                uint128_t p0_board = str_uint128(line);

                std::getline(std::cin, line);
                uint128_t p1_board = str_uint128(line);

                std::cerr << "Received boards: " << std::endl;

                controller.teams[0].board = p0_board;
                controller.teams[1].board = p1_board;


//                uint128_t best_p2;
//                uint128_t result_p1;
//                ai.bestmove(&controller.teams[1], &controller.teams[0], &controller, p2_score, p1_score, &best_p2, &result_p1);
//                p2_score += controller.checkScoreAndCleanEdges(&result_p1) ? 1 : 0;
//                controller.teams[1].board = best_p2;
//                controller.teams[0].board = result_p1;
//                next_move = false;
            }
            turn ^= 1;
        }


        window.clear(sf::Color(100, 100, 255));
        controller.draw(&window);
        window.display();
        //sleep(1);
//
//        for (int x = 0; x < 900000; x++) {
//            std::cout << "DELAY" << std::endl;
//        };


    }

    return 0;
}

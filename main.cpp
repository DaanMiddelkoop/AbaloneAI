#include <iostream>
#include <unistd.h>

#include <SFML/Graphics.hpp>
#include "board.h"
#include "move.h"
#include "ai.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");

    Controller controller = Controller();

    controller.addTeam(sf::Color::White);
    controller.addTeam(sf::Color::Black);

    controller.setup();

    AI ai = AI();

    controller.printBoard(controller.teams[0].board);

    uint128_t moves[1000];
    std::cout << "number of possible moves: " << controller.getMoves(&controller.teams[0], moves, 1000) << std::endl;



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
            std::cout << "start calculating move." << std::endl;
            if (turn == 0) {
                uint128_t best_p1;
                uint128_t result_p2;
                ai.bestmove(&controller.teams[0], &controller.teams[1], &controller, p1_score, p2_score, &best_p1, &result_p2);
                p1_score += controller.checkScoreAndCleanEdges(&result_p2) ? 1 : 0;
                controller.teams[0].board = best_p1;
                controller.teams[1].board = result_p2;

            } else {
                uint128_t best_p2;
                uint128_t result_p1;
                ai.bestmove(&controller.teams[1], &controller.teams[0], &controller, p2_score, p1_score, &best_p2, &result_p1);
                p2_score += controller.checkScoreAndCleanEdges(&result_p1) ? 1 : 0;
                controller.teams[1].board = best_p2;
                controller.teams[0].board = result_p1;
            }

            std::cout << "calculated move." << std::endl;

            turn ^= 1;
            next_move = false;
        }


        window.clear(sf::Color(100, 100, 255));
        controller.draw(&window);
        window.display();
        sleep(1);
//
//        for (int x = 0; x < 900000; x++) {
//            std::cout << "DELAY" << std::endl;
//        };


    }

    return 0;
}

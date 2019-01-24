#include <iostream>
#include <cstring>

#include "State.h"
#include "Turn.h"


int main(int argc, char* argv[]) {
    State* state = new State();
    Turn* turn = new Turn();

    while (true) {
        // build list of valid turns
        state->buildListValidTurns();

        // print board
        state->print();

        // get current players
        char player = state->getPlayer();

        // check if game over
        int winnerCode = state->getWinnerCode();
        if (winnerCode > 0) {
            std::cout << "\n-- player " << player << " wins\n" << std::endl;
            return winnerCode;
        }

        // wait for player input
        std::cout << "\nplayer '" << player << "'> ";
        std::cin.clear();
        std::cin.getline(turn->data(), Turn::dataLength);
        std::cout << std::endl;

        // check if matching one valid turn
        if (state->checkMatchingValidTurn(turn)) {
            // decode player input into a list of coordinates
            turn->decode();

            // operate the turn
            state->move(turn);

            // player takes turn
            state->setNextPlayer();
        }
        else {
            // invalid input
            std::cout << "-- invalid turn\n" << std::endl;
            state->setInvalidTurn();
        }
    }

    return 0;
}

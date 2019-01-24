//
// Expressing board state and current player.
//

#ifndef CHECKERS_STATE_H
#define CHECKERS_STATE_H
#include <vector>
#include "Turn.h"
#include "Coord.h"

class Turn;


class State {
public:
    const static int boardSize = 8;
    const static int idIndex = boardSize * boardSize;
    const static int dataLength = idIndex + 1 + 1;

    State();
    ~State() {}

    void buildListValidTurns();
    void print() const;

    bool checkMatchingValidTurn(Turn *turn) const;
    void move(Turn *turn);
    void setNextPlayer();
    void setInvalidTurn();

    char getPlayer() const;
    int getWinnerCode() const;

    char* data() { return data_; }

private:
    const static int manKingDiff = 'a' - 'A';
    const static char legalSpace = '.';
    const static char illegalSpace = ' ';
    const static char validPlayerO = 'o';
    const static char validPlayerX = 'x';
    const static char invalidPlayerO = 'p';
    const static char invalidPlayerX = 'y';
    const static char winningPlayerO = 'O';
    const static char winningPlayerX = 'X';

    void checkValidTurns(Coord *coord);

    bool checkValidJumpTurns(
            Coord *pre_coord, Coord *new_coord, std::vector<Coord *> coords, bool king);
    void checkValidMoveTurns(Coord *coord);

    void addValidJumpTurn(const std::vector<Coord *> coords);
    void addValidMoveTurn(Coord *pre_coord, Coord *new_coord);

    bool isValidCoord(const Coord *coord) const;
    bool isOwnPiece(const Coord *coord) const;
    bool isOpponentPiece(const Coord *coord) const;
    bool isLegal(const Coord *coord) const;
    bool isKing(const Coord *coord) const;

    bool isValidDirection(Coord *pre_coord, Coord *new_coord) const;
    bool isMoveable(Coord *pre_coord, Coord *new_coord) const;
    bool isJumpable(Coord *pre_coord, Coord *new_coord, bool king) const;

    char getOpponent() const;

    void setPlayer(const char player);

    void setPiece(const Coord *coord, const char piece);
    char getPiece(const Coord *coord) const;

    char data_[dataLength];
    std::vector<Turn*> validTurns;
    bool forcedCapture;
    int oCount;
    int xCount;
};
#endif //CHECKERS_STATE_H

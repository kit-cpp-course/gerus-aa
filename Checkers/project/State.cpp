#include "State.h"

#include <iostream>


State::State() : oCount(0), xCount(0) {
    int index = 0;

    // initialize board
    for (int y = 1; y <= boardSize; ++y) {
        for (int x = 1; x <= boardSize; ++x) {
            if ((x & 1) == (y & 1)) {             // legal space
                if (y <= 3) {                       // player o
                    data_[index] = validPlayerO;
                    ++oCount;
                }
                else if (y >= 6) {                  // player x
                    data_[index] = validPlayerX;
                    ++xCount;
                }
                else {                              // empty space
                    data_[index] = legalSpace;
                }
            }
            else {                                // illegal space
                data_[index] = illegalSpace;
            }
            ++index;
        }
    }

    // player o moves first
    data_[index++] = validPlayerO;

    // end of string
    data_[index] = '\0';
}

void State::buildListValidTurns() {
    // reset
    validTurns.clear();
    forcedCapture = false;

    // check valid turns based on every own piece
    for (int y = 0; y < boardSize; ++y) {
        for (int x = 0; x < boardSize; ++x) {
            Coord* coord = new Coord(x, y);
            if (isOwnPiece(coord)) {
                checkValidTurns(coord);
            }
        }
    }

    // game over
    if (validTurns.size() == 0) {
        char opponent = getOpponent();
        setPlayer(opponent - manKingDiff);
    }

    // forced capture
    if (forcedCapture) {
        auto iterator = validTurns.begin();
        while (iterator != validTurns.end()) {
            if ((*iterator)->capture()) {               // keep jump turn
                ++iterator;
            }
            else {                                      // delete move turn
                iterator = validTurns.erase(iterator);
            }
        }
    }
}

void State::print() const {
    const char kHRuler[] = "  a b c d e f g h";
    std::cout << kHRuler << "\n";
    int index = boardSize * boardSize - boardSize;
    for (int y = boardSize; y > 0; --y) {
        std::cout << y << " ";
        for (int x = 0; x < boardSize; ++x) {
            std::cout << data_[index++] << " ";
        }
        std::cout << y << "\n";
        index -= (boardSize << 1);
    }
    std::cout << kHRuler << std::endl;
}

bool State::checkMatchingValidTurn(Turn *turn) const {
    for (auto valid_turn : validTurns) {
        if (valid_turn->checkMatching(turn)) {
            return true;
        }
    }
    return false;
}

void State::move(Turn *turn) {
    std::vector<Coord*> coords = turn->coords();
    char player = getPlayer();
    char piece = getPiece(coords[0]);
    bool king = isKing(coords[0]);
    int last_coord_index = coords.size() - 1;

    setPiece(coords[0], legalSpace);

    // if jump
    if (turn->capture()) {
        for (int i = 0; i < last_coord_index; ++i) {
            Coord* mid_coord = coords[i]->average(coords[i + 1]);
            setPiece(coords[i], legalSpace);
            setPiece(mid_coord, legalSpace);
        }
    }

    // crowning
    if ((player == validPlayerO && coords[last_coord_index]->y == boardSize - 1)
        || (player == validPlayerX && coords[last_coord_index]->y == 0)) {
        if (!king) {
            piece -= manKingDiff;
        }
    }

    // last coord
    setPiece(coords[last_coord_index], piece);
}

void State::setNextPlayer() {
    if (getPlayer() == validPlayerO) {
        data_[idIndex] = validPlayerX;
    }
    else {
        data_[idIndex] = validPlayerO;
    }
}

void State::setInvalidTurn() {
    if (getPlayer() == validPlayerO) {
        data_[idIndex] = invalidPlayerO;
    }
    else {
        data_[idIndex] = invalidPlayerX;
    }
}

char State::getPlayer() const {
    if (data_[idIndex] == validPlayerO
        || data_[idIndex] == invalidPlayerO
        || data_[idIndex] == winningPlayerO) {
        return validPlayerO;
    }
    else {
        return validPlayerX;
    }
}

int State::getWinnerCode() const {
    if (data_[idIndex] == winningPlayerO) {
        return 1;
    }
    if (data_[idIndex] == winningPlayerX) {
        return 2;
    }
    return 0;
}

void State::checkValidTurns(Coord *coord) {
    std::vector<Coord*> coords;
    coords.push_back(coord);
    bool king = isKing(coord);
    Coord* new_coord;

    // jump
    new_coord = new Coord(coord->x - 2, coord->y - 2);    // left bottom
    checkValidJumpTurns(coord, new_coord, coords, king);
    new_coord = new Coord(coord->x + 2, coord->y - 2);    // right bottom
    checkValidJumpTurns(coord, new_coord, coords, king);
    new_coord = new Coord(coord->x - 2, coord->y + 2);    // left top
    checkValidJumpTurns(coord, new_coord, coords, king);
    new_coord = new Coord(coord->x + 2, coord->y + 2);    // right top
    checkValidJumpTurns(coord, new_coord, coords, king);

    // move
    if (!forcedCapture) {
        checkValidMoveTurns(coord);
    }
}

bool State::checkValidJumpTurns(
        Coord *pre_coord, Coord *new_coord, std::vector<Coord *> coords, bool king) {
    if (isJumpable(pre_coord, new_coord, king)) {
        coords.push_back(new_coord);
        bool child = false;
        Coord* next_coord;

        // advanced jump
        next_coord = new Coord(new_coord->x - 2, new_coord->y - 2);   // left bottom
        if (!pre_coord->equal(next_coord)) {
            child = checkValidJumpTurns(new_coord, next_coord, coords, king) || child;
        }
        next_coord = new Coord(new_coord->x + 2, new_coord->y - 2);   // right bottom
        if (!pre_coord->equal(next_coord)) {
            child = checkValidJumpTurns(new_coord, next_coord, coords, king) || child;
        }
        next_coord = new Coord(new_coord->x - 2, new_coord->y + 2);   // left top
        if (!pre_coord->equal(next_coord)) {
            child = checkValidJumpTurns(new_coord, next_coord, coords, king) || child;
        }
        next_coord = new Coord(new_coord->x + 2, new_coord->y + 2);   // right top
        if (!pre_coord->equal(next_coord)) {
            child = checkValidJumpTurns(new_coord, next_coord, coords, king) || child;
        }

        if (!child) {
            forcedCapture = true;
            addValidJumpTurn(coords);
        }
        return true;
    }
    else {
        return false;
    }
}

void State::checkValidMoveTurns(Coord *coord) {
    Coord* new_coord;

    new_coord = new Coord(coord->x - 1, coord->y - 1);        // left bottom
    if (isMoveable(coord, new_coord)) {
        addValidMoveTurn(coord, new_coord);
    }
    new_coord = new Coord(coord->x + 1, coord->y - 1);        // right bottom
    if (isMoveable(coord, new_coord)) {
        addValidMoveTurn(coord, new_coord);
    }
    new_coord = new Coord(coord->x - 1, coord->y + 1);        // left top
    if (isMoveable(coord, new_coord)) {
        addValidMoveTurn(coord, new_coord);
    }
    new_coord = new Coord(coord->x + 1, coord->y + 1);        // right top
    if (isMoveable(coord, new_coord)) {
        addValidMoveTurn(coord, new_coord);
    }
}

void State::addValidJumpTurn(const std::vector<Coord *> coords) {
    validTurns.push_back(new Turn(coords, true));
}

void State::addValidMoveTurn(Coord *pre_coord, Coord *new_coord) {
    std::vector<Coord*> coords;
    coords.push_back(pre_coord);
    coords.push_back(new_coord);
    validTurns.push_back(new Turn(coords, false));
}

bool State::isValidCoord(const Coord *coord) const {
    return (coord->x >= 0 && coord->x < boardSize
            && coord->y >= 0 && coord->y < boardSize);
}

bool State::isOwnPiece(const Coord *coord) const {
    char player = getPlayer();
    char piece = getPiece(coord);

    if (piece == player || piece == player - manKingDiff) {
        return true;
    }
    else {
        return false;
    }
}

bool State::isOpponentPiece(const Coord *coord) const {
    char opponent = getOpponent();
    char piece = getPiece(coord);

    if (piece == opponent || piece == opponent - manKingDiff) {
        return true;
    }
    else {
        return false;
    }
}

bool State::isLegal(const Coord *coord) const {
    return getPiece(coord) == legalSpace;
}

bool State::isKing(const Coord *coord) const {
    char piece = getPiece(coord);
    return piece >= 'A' && piece <= 'Z';
}

bool State::isValidDirection(Coord *pre_coord, Coord *new_coord) const {
    char player = getPlayer();
    int stepY = new_coord->y - pre_coord->y;
    if ((player == validPlayerO && stepY > 0)
        || (player == validPlayerX && stepY < 0)) {
        return true;
    }
    return false;
}

bool State::isMoveable(Coord *pre_coord, Coord *new_coord) const {
    if (isValidCoord(new_coord) && isLegal(new_coord)
        && (isValidDirection(pre_coord, new_coord) || isKing(pre_coord))) {
        return true;
    }
    return false;
}

bool State::isJumpable(Coord *pre_coord, Coord *new_coord, bool king) const {
    if (isValidCoord(new_coord) && isLegal(new_coord)
        && (isValidDirection(pre_coord, new_coord) || king)) {
        Coord* mid_coord = pre_coord->average(new_coord);
        if (isOpponentPiece(mid_coord)) {
            return true;
        }
    }
    return false;
}

char State::getOpponent() const {
    char player = getPlayer();
    if (player == validPlayerO) {
        return validPlayerX;
    }
    else {
        return validPlayerO;
    }
}

void State::setPlayer(const char player) {
    data_[idIndex] = player;
}

void State::setPiece(const Coord *coord, const char piece) {
    data_[coord->y * boardSize + coord->x] = piece;
}

char State::getPiece(const Coord *coord) const {
    return data_[coord->y * boardSize + coord->x];
}

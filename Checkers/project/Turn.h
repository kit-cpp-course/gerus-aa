//
// Expressing coordinates of moving or jumping path
//

#ifndef CHECKERS_TURN_H
#define CHECKERS_TURN_H
#include <vector>

#include "State.h"
#include "Coord.h"

class State;


class Turn {
public:
    const static int dataLength = 30;

    Turn() {}
    Turn(const std::vector<Coord*> coords, const bool capture)
            : coords_(coords), capture_(capture) {
        Encode();
    }
    ~Turn() {}

    void decode();

    bool checkMatching(Turn *turn) const;

    char* data() { return data_; }
    std::vector<Coord*> coords() const { return coords_; }
    bool capture() const { return capture_; }

private:
    const char xMin = 'a';     // min x coordinate
    const char yMin = '1';     // min y coordinate

    void Encode();

    char data_[dataLength];
    std::vector<Coord*> coords_;
    bool capture_;
};
#endif //CHECKERS_TURN_H
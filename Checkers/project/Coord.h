//
// Coordinates of checkers board.
//

#ifndef CHECKERS_COORD_H
#define CHECKERS_COORD_H
struct Coord {
    Coord(int newX = 0, int newY = 0) : x(newX), y(newY) {}
    ~Coord() {}

    Coord* average(Coord *coord) {
        return new Coord((x + coord->x) >> 1, (y + coord->y) >> 1);
    }

    bool equal(Coord *coord) {
        return x == coord->x && y == coord->y;
    }

    int x;
    int y;
};
#endif //CHECKERS_COORD_H

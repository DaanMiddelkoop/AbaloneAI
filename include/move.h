#ifndef MOVE_H
#define MOVE_H


class Move
{
    // Splitted as x, y for 4 tile movements (max 3 of 1 team and a possible bump)

    public:
        Move();
        int old_positions[5];
        int new_positions[5];
        bool score;

    protected:

    private:
};

#endif // MOVE_H

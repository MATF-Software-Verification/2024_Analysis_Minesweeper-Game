#include <iostream>
using namespace std;

enum Status
{
    Locked,
    Unlocked,
    Flagged
};

class Cell
{
private:
    char data;
    Status status;
    int value;

public:
    Cell();
    void DisplayCell();
};

class Board
{
private:
    int size;
    Cell **arr;

public:
    Board(int _size);
    void ShowBoard();
};

class Minesweeper
{
private:
    Board *board;

public:
    Minesweeper();
    void Instructions();
    void Play();
    void GetInput();
};
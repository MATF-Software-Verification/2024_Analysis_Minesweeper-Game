#include <iostream>
using namespace std;

int CharToInt(char c);
char IntToChar(int x);

enum Difficulty
{
    Beginner,
    Intermediate,
    Expert
};

enum PlayerMove
{
    Open,
    Mark
};

enum Status
{
    Locked,
    Unlocked,
    Flagged
};

enum Content
{
    Bomb,
    Number
};

class Cell
{
private:
    char data;
    Status status;
    Content content;

public:
    Cell();
    void DisplayCell();
    void SetBomb();
    void IncreaseValue();
    void SetFlagged();
    void OpenCell();
};

class Board
{
private:
    int size;
    Cell **arr;
    bool boardGenerated;

public:
    Board(int);
    void ShowBoard();
    bool PlayerSelect(int, int, PlayerMove);
    void GenerateBoard(int, int);
};

class Minesweeper
{
private:
    Board *board;
    int size;
    Difficulty difficulty;

public:
    Minesweeper();
    void Instructions();
    void SetDifficulty();
    void CreateBoard();
    void Play();
    void GetInput();
};
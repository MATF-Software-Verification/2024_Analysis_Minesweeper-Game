#include <iostream>
using namespace std;

int CharToInt(char);
char IntToChar(int);
int GetRandom(int, int);

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
    bool TriggerFlag();
    bool OpenCell();
};

class Board
{
private:
    int size;
    Cell **arr;
    bool boardGenerated;
    int mines;

public:
    Board(int, int);
    void ShowBoard();
    bool PlayerSelect(int, int, PlayerMove);
    void GenerateBoard(int, int);
    void SetValuesAroundMine(int, int);
    bool ClickCell(int, int);
};

class Minesweeper
{
private:
    Board *board;
    int size;
    Difficulty difficulty;
    int mines;

public:
    Minesweeper();
    void Instructions();
    void SetDifficulty();
    void CreateBoard();
    void Play();
    void GetInput();
};
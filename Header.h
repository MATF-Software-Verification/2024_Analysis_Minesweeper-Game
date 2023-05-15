#include <iostream>
#include <queue>
using namespace std;

int CharToInt(char);
char IntToChar(int);
int GetRandom(int, int);

enum GameState
{
    Playing,
    Won,
    Lost
};

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
    Number,
    Bomb
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
    int OpenCell();
    bool IsUnlocked();
    bool IsFlagged();
    bool CheckForZero();
    bool CheckIfBomb();
};

class Board
{
private:
    int size;
    Cell **arr;
    bool boardGenerated;
    int mines;
    bool bombClicked;
    vector<int> xCord;
    vector<int> yCord;

public:
    Board(int, int);
    void ShowBoard();
    bool PlayerSelect(int, int, PlayerMove);
    void GenerateBoard(int, int);
    void SetValuesAroundMine(int, int);
    bool ClickCell(int, int);
    void OpenAdjacentCells(int, int, queue<int> *, queue<int> *, vector<int> *, vector<int> *);
    bool GetBombClick();
    void UnlockBombs();
};

class Minesweeper
{
private:
    Board *board;
    int size;
    Difficulty difficulty;
    int mines;
    GameState gameState;

public:
    Minesweeper();
    void Instructions();
    void SetDifficulty();
    void CreateBoard();
    void Play();
    void GetInput();
};
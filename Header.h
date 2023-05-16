#include <iostream>
#include <queue>
using namespace std;
/*
    Macros for board sizes of each difficulty level
*/
#define EASY_SIZE 9
#define MEDIUM_SIZE 15
#define HARD_SIZE 25
/*
    Macros for max mines of each difficulty level
*/
#define EASY_MINES 10
#define MEDIUM_MINES 40
#define HARD_MINES 99
/*
    Utility functions
*/
int CharToInt(char);
char IntToChar(int);
int GetRandom(int, int);
/*
    The current state of the game is represented using this
*/
enum GameState
{
    Playing,
    Won,
    Lost
};
/*
    Used to represent difficulty of the game
*/
enum Difficulty
{
    Beginner,
    Intermediate,
    Expert
};
/*
    The current move of the user is recorded with this
    Open - open the cell
    Mark - flag / unflag the cell
*/
enum PlayerMove
{
    Open,
    Mark
};
/*
    Used to represent the status of the cell
    - Locked
    - Unlocked
    - Flagged
*/
enum Status
{
    Locked,
    Unlocked,
    Flagged
};
/*
    A description of the content inside the cell
    Number - the cell doesn't contain a bomb
    Bomb - the cell contains a bomb
*/
enum Content
{
    Number,
    Bomb
};
/*
    This class represents each cell in the board
*/
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
    int OpenCell();
    bool TriggerFlag();
    bool IsUnlocked();
    bool IsFlagged();
    bool CheckForZero();
    bool CheckIfBomb();
};
/*
    This class represents the entire board
    It is made of a 2D array of Cells.
*/
class Board
{
private:
    int size;
    Cell **arr;
    int mines;
    int cellsOpened;
    int flagCount;
    bool boardGenerated;
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
    int GetCellsOpened();
};
/*
    This class acts as the Game Manager
*/
class Minesweeper
{
private:
    Board *board;
    int size;
    int mines;
    Difficulty difficulty;
    GameState gameState;

public:
    Minesweeper();
    void Instructions();
    void SetDifficulty();
    void CreateBoard();
    void Play();
    void GetInput();
    void CheckWin();
};
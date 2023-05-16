#include "Header.h"
using namespace std;
/*
    Minesweeper class Constructor
*/
Minesweeper::Minesweeper()
{
    Instructions();
    SetDifficulty();
    CreateBoard();
    gameState = Playing;
}
/*
    The main game logic function
*/
void Minesweeper::Play()
{
    do
    {
        board->ShowBoard();
        GetInput();
    } while (gameState == Playing);
    if (gameState == Lost)
    {
        cout << "\nYou've clicked on a bomb!\n";
        board->UnlockBombs();
        board->ShowBoard();
        cout << "Better luck next time!\n";
    }
    else if (gameState == Won)
    {
        cout << "\nCongratulations! You've won!\n";
        board->UnlockBombs();
        board->ShowBoard();
        cout << "Thanks for playing!\n";
    }
}
/*
    Displays information for player
*/
void Minesweeper::Instructions()
{
    cout << "\nWELCOME TO THE MINESWEEPER GAME!\n";
    cout << "\nThese are the rules: \n";
    cout << "\n- The board is divided into cells, with mines randomly distributed.\n- To win, you need to open all the cells. \n- The number on a cell shows the number of mines adjacent to it. Using this information, you can determine cells that are safe, and cells that contain mines.\n- Interact, evolve and enjoy!\n\n";
}
/*
    Gets player input to set Difficulty level
    Three Difficulty Levels
        - Beginner
        - Intermediate
        - Expert
*/
void Minesweeper::SetDifficulty()
{
    int x = 1;
    do
    {
        if (x < 1 || x > 3)
        {
            cout << "\nPlease enter valid input!\n";
        }
        cout << "\nEnter difficulty level ( 1 - Beginner | 2 - Intermediate | 3 - Expert ) : ";
        cin >> x;
    } while (x < 1 || x > 3);
    difficulty = (Difficulty)(x - 1);
}
/*
    Initializes and allocates the Board class object depending upon difficulty level
*/
void Minesweeper::CreateBoard()
{
    switch (difficulty)
    {
    case Beginner:
        size = EASY_SIZE;
        mines = EASY_MINES;
        break;
    case Intermediate:
        size = MEDIUM_SIZE;
        mines = MEDIUM_MINES;
        break;
    case Expert:
        size = HARD_SIZE;
        mines = HARD_MINES;
        break;
    default:
        break;
    }
    board = new Board(size, mines);
}
/*
    Get player input for position of cell and move to perform
    Position of cell - row and column
    Player move - open or mark
*/
void Minesweeper::GetInput()
{
    int row = 1, col = 1;
    PlayerMove playerMove;
    int input = 1;
    do
    {
        if (board->GetBombClick())
        {
            gameState = Lost;
            return;
        }
        do
        {
            if (row < 1 || row > size)
            {
                cout << "\nPlease enter valid input!\n";
            }
            cout << "\nEnter row (1 - " << size << "): ";
            cin >> row;
        } while (row < 1 || row > size);
        do
        {
            if (col < 1 || col > size)
            {
                cout << "\nPlease enter valid input!\n";
            }
            cout << "\nEnter col (1 - " << size << "): ";
            cin >> col;
        } while (col < 1 || col > size);
        do
        {
            if (input < 1 || input > 2)
            {
                cout << "\nPlease enter valid input!\n";
            }
            cout << "\nSelect operation (1 - Open | 2 - Flag/UnFlag) : ";
            cin >> input;
        } while (input < 1 || input > 2);
        if (input == 1)
            playerMove = Open;
        else
            playerMove = Mark;
    } while (!board->PlayerSelect(row - 1, col - 1, playerMove));
    if (playerMove == Open)
        CheckWin();
}
/*
    Check if number of cells opened is equal to ( total size - number of mines )
*/
void Minesweeper::CheckWin()
{
    int cellsOpened = board->GetCellsOpened();
    if (cellsOpened == (size * size - mines))
        gameState = Won;
}
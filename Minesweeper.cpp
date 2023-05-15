#include "Header.h"
using namespace std;

Minesweeper::Minesweeper()
{
}
void Minesweeper::Play()
{
    Instructions();
    SetDifficulty();
    CreateBoard();
    board->ShowBoard();
}
void Minesweeper::Instructions()
{
    cout << "\nWELCOME TO MINESWEEPER GAME!\n";
    cout << "\nThese are the rules: \n";
    cout << "\n- The board is divided into cells, with mines randomly distributed.\n- To win, you need to open all the cells. \n- The number on a cell shows the number of mines adjacent to it. Using this information, you can determine cells that are safe, and cells that contain mines.\n- Interact, evolve and enjoy!\n\n";
}
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
void Minesweeper::CreateBoard()
{
    switch (difficulty)
    {
    case Beginner:
        size = 9;
        break;
    case Intermediate:
        size = 15;
        break;
    case Expert:
        size = 25;
        break;
    default:
        break;
    }
    board = new Board(size);
}
void Minesweeper::GetInput()
{
    int row = 1, col = 1;
    PlayerMove playerMove;
    do
    {
        if (row < 1 || row > size)
        {
            cout << "\nPlease enter valid input!\n";
        }
        cout << "\nEnter row (1 - 9): ";
        cin >> row;
    } while (row < 1 || row > size);
    do
    {
        if (col < 1 || col > size)
        {
            cout << "\nPlease enter valid input!\n";
        }
        cout << "\nEnter col (1 - 9): ";
        cin >> col;
    } while (col < 1 || col > size);

    int input = 1;
    do
    {
        if (input < 1 || input > 2)
        {
            cout << "\nPlease enter valid input!\n";
        }
        cout << "\nSelect operation (1 - Open | 2 - Flag) : ";
        cin >> input;
    } while (input < 1 || input > 2);
    if (input == 1)
        playerMove = Open;
    else
        playerMove = Mark;
    board->PlayerSelect(row, col, playerMove);
}
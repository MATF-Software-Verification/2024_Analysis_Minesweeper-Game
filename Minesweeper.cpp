#include "Header.h"
using namespace std;

Minesweeper::Minesweeper()
{
    Instructions();
    SetDifficulty();
    CreateBoard();
    gameState = Playing;
}
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
void Minesweeper::Instructions()
{
    cout << "\nWELCOME TO THE MINESWEEPER GAME!\n";
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
        mines = 10;
        break;
    case Intermediate:
        size = 15;
        mines = 40;
        break;
    case Expert:
        size = 25;
        mines = 99;
        break;
    default:
        break;
    }
    board = new Board(size, mines);
}
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
void Minesweeper::CheckWin()
{
    int cellsOpened = board->GetCellsOpened();
    if (cellsOpened == (size * size - mines))
        gameState = Won;
}
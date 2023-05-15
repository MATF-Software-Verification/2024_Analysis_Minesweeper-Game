#include "Header.h"
#include "vector"
using namespace std;

Board::Board(int _size)
{
    size = _size;
    arr = new Cell *[size];
    for (int i = 0; i < size; i++)
    {
        arr[i] = new Cell[size]();
    }
}
void Board::ShowBoard()
{
    cout << "\n     ";
    for (int i = 0; i < size; i++)
    {
        if (size > 9)
        {
            if (i < 9)
            {
                cout << " 0" << i + 1 << " ";
            }
            else
                cout << " " << i + 1 << " ";
        }
        else
            cout << i + 1 << " ";
    }
    cout << "\n\n";
    for (int i = 0; i < size; i++)
    {
        if (size > 9)
        {
            if (i < 9)
                cout << "0" << i + 1 << "   ";
            else
                cout << i + 1 << "   ";
        }
        else
            cout << i + 1 << "   ";
        if (size > 9)
            cout << "| ";
        else
            cout << "|";
        for (int j = 0; j < size; j++)
        {
            arr[i][j].DisplayCell();
            if (size > 9)
                cout << " | ";
            else
                cout << "|";
        }
        cout << "\n";
    }
    cout << "\n";
}
bool Board::PlayerSelect(int row, int col, PlayerMove playerMove)
{
    if (!boardGenerated && playerMove == Open)
    {
        GenerateBoard(row, col);
        boardGenerated = true;
    }
    if (playerMove == Mark)
    {
        // Set data to "F"
    }
    else if (playerMove == Open)
    {
        /*
        If cell already unlocked then return false
        Else change state to unlocked
        */
    }
    return true;
}
void Board::GenerateBoard(int row, int col)
{
    /*
        1. Set mines at random position (retry random pos if its equal to player input)
        2. Increment adjacent cells next to mines
    */
}
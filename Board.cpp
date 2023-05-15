#include "Header.h"
#include <ctime>
#include <vector>
using namespace std;

Board::Board(int _size, int _mines)
{
    size = _size;
    mines = _mines;
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
bool Board::ClickCell(int row, int col)
{
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
        if (!arr[row][col].TriggerFlag())
        {
            cout << "Cell already open! Try again\n";
            return false;
        }
    }
    else if (playerMove == Open)
    {
        /*
        If cell already unlocked then return false
        Else change state to unlocked
        */
        if (!arr[row][col].OpenCell())
        {
            cout << "Cell already open! Try again\n";
            return false;
        }
        /*
            open adjacent cells if empty
         */
    }
    return true;
}
void Board::SetValuesAroundMine(int row, int col)
{
    int minX, minY;
    int maxX, maxY;

    if (row - 1 >= 0)
        minX = row - 1;
    else
        minX = row;
    if (col - 1 >= 0)
        minY = col - 1;
    else
        minY = col;

    if (row + 1 <= mines - 1)
        maxX = row + 1;
    else
        maxX = row;
    if (col + 1 <= mines - 1)
        maxY = col + 1;
    else
        maxY = col;

    for (int i = minX; i <= maxX; i++)
    {
        for (int j = minY; j <= maxY; j++)
        {
            if (i != row && j != col)
            {
                arr[i][j].IncreaseValue();
            }
        }
    }
}
void Board::GenerateBoard(int row, int col)
{
    /*
        1. Set mines at random position (retry random pos if its equal to player input)
        2. Increment adjacent cells next to mines
    */
    srand(time(0));
    vector<int> xCord(mines);
    vector<int> yCord(mines);

    int tempX, tempY;
    bool exists;

    for (int i = 0; i < mines; i++)
    {
        // generate random row, random col
        // compare with existing values
        do
        {
            tempX = GetRandom(0, size - 1);
            tempY = GetRandom(0, size - 1);

            if (tempX == row && tempY == col)
            {
                exists = true;
                continue;
            }
            for (int i = 0; i < mines; i++)
            {
                if (!xCord.empty() && !yCord.empty())
                {
                    if (tempX == xCord[i] && tempY == yCord[i])
                    {
                        exists = true;
                        break;
                    }
                }
            }
            exists = false;
        } while (exists);
        // Set cell to bomb
        arr[tempX][tempY].SetBomb();
        // push data into vector
        xCord.at(i) = tempX;
        yCord.at(i) = tempY;
    }
    // Increment cells next to mines
    for (int i = 0; i < mines; i++)
    {
        SetValuesAroundMine(xCord[i], yCord[i]);
    }
}
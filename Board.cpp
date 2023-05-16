#include "Header.h"
#include <vector>
#include <queue>
using namespace std;
/*
    Board class Constructor
*/
Board::Board(int _size, int _mines)
{
    cellsOpened = 0;
    size = _size;
    mines = _mines;
    flagCount = mines;
    arr = new Cell *[size];
    for (int i = 0; i < size; i++)
    {
        arr[i] = new Cell[size]();
    }
    bombClicked = false;
}
/*
    Displays the entire board along with rows and columns
*/
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
/*
    Sets all Bomb containing cells to Unlocked
*/
void Board::UnlockBombs()
{
    for (int i = 0; i < mines; i++)
    {
        arr[xCord[i]][yCord[i]].OpenCell();
    }
}
/*
    Performs open operation on a cell of player's choice
*/
bool Board::ClickCell(int row, int col)
{
    if (arr[row][col].IsFlagged())
    {
        cout << "\nCell is flagged already! UnFlag to open it!\n";
        return false;
    }
    if (arr[row][col].IsUnlocked())
    {
        cout << "\nCell is already open!\n";
        return false;
    }
    if (arr[row][col].CheckIfBomb())
    {
        bombClicked = true;
        return false;
    }
    if (!arr[row][col].CheckForZero())
    {
        arr[row][col].OpenCell();
        cellsOpened++;
        return true;
    }
    else
    {
        queue<int> xQueue;
        queue<int> yQueue;
        vector<int> xVector;
        vector<int> yVector;
        OpenAdjacentCells(row, col, &xQueue, &yQueue, &xVector, &yVector);
        while (!xQueue.empty() && !yQueue.empty())
        {
            if (!arr[xQueue.front()][yQueue.front()].IsUnlocked())
            {
                arr[xQueue.front()][yQueue.front()].OpenCell();
                cellsOpened++;
            }
            xQueue.pop();
            yQueue.pop();
        }
    }
    return true;
}
/*
    Opens the cells adjacent (horizontally and vertically) to current cell
    If they dont exist in the vectors, then enqueue the coordinates
*/
void Board::OpenAdjacentCells(int row, int col, queue<int> *xQueue, queue<int> *yQueue, vector<int> *xVector, vector<int> *yVector)
{
    if (arr[row][col].CheckIfBomb() || arr[row][col].IsUnlocked())
        return;
    bool alreadyPresent = false;
    if (!xVector->empty() && !yVector->empty())
    {
        for (int i = 0; i < xVector->size(); i++)
        {
            if (xVector->at(i) == row && yVector->at(i) == col)
            {
                alreadyPresent = true;
                break;
            }
        }
    }
    if (alreadyPresent)
    {
        return;
    }
    // Cell not present in queue
    // Add this cell to queue
    xQueue->push(row);
    yQueue->push(col);
    xVector->push_back(row);
    yVector->push_back(col);
    if (!arr[row][col].CheckForZero())
    {
        return;
    }
    // Opening top, down, left and right cells
    // Check top
    if (row - 1 >= 0)
    {
        OpenAdjacentCells(row - 1, col, xQueue, yQueue, xVector, yVector);
    }
    // Check bottom
    if (row + 1 <= size - 1)
    {
        OpenAdjacentCells(row + 1, col, xQueue, yQueue, xVector, yVector);
    }
    // Check left
    if (col - 1 >= 0)
    {
        OpenAdjacentCells(row, col - 1, xQueue, yQueue, xVector, yVector);
    }
    // Check right
    if (col + 1 <= size - 1)
    {
        OpenAdjacentCells(row, col + 1, xQueue, yQueue, xVector, yVector);
    }
}
/*
    Selecting cell to perform Open or Mark action upon
*/
bool Board::PlayerSelect(int row, int col, PlayerMove playerMove)
{
    if (!boardGenerated && playerMove == Open)
    {
        GenerateBoard(row, col);
        boardGenerated = true;
    }
    if (playerMove == Mark)
    {
        if (arr[row][col].IsUnlocked())
        {
            cout << "Cell already open! Try again\n";
            return false;
        }
        else
        {
            if (arr[row][col].IsFlagged())
            {
                arr[row][col].TriggerFlag();
                flagCount++;
            }
            else
            {
                if (flagCount > 0)
                {
                    arr[row][col].TriggerFlag();
                    flagCount--;
                }
                else
                {
                    cout << "No more flags available!\n";
                    return false;
                }
            }
            return true;
        }
    }
    else if (playerMove == Open)
    {
        if (!ClickCell(row, col))
        {
            return false;
        }
    }
    return true;
}
/*
    Setting values for cells around the mines
*/
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

    if (row + 1 <= size - 1)
        maxX = row + 1;
    else
        maxX = row;
    if (col + 1 <= size - 1)
        maxY = col + 1;
    else
        maxY = col;

    for (int i = minX; i <= maxX; i++)
    {
        for (int j = minY; j <= maxY; j++)
        {
            if (i != row || j != col)
            {
                arr[i][j].IncreaseValue();
            }
        }
    }
}
/*
    Generates the board depending upon the size / difficulty
    1. Set mines at random position (retry random pos if its equal to player input)
    2. Increment adjacent cells next to mines
*/
void Board::GenerateBoard(int row, int col)
{
    int tempX, tempY;
    bool exists;
    for (int i = 0; i < mines; i++)
    {
        do
        {
            exists = false;
            tempX = GetRandom(0, size - 1);
            tempY = GetRandom(0, size - 1);
            if (tempX == row && tempY == col)
            {
                exists = true;
            }
            else if (!xCord.empty() && !yCord.empty())
            {
                for (int i = 0; i < xCord.size(); i++)
                {
                    if (xCord.at(i) == tempX && yCord.at(i) == tempY)
                    {
                        exists = true;
                        break;
                    }
                }
            }
        } while (exists);
        arr[tempX][tempY].SetBomb();
        xCord.push_back(tempX);
        yCord.push_back(tempY);
    }
    for (int i = 0; i < mines; i++)
    {
        SetValuesAroundMine(xCord[i], yCord[i]);
    }
}
/*
    Return if bomb has been clicked on
*/
bool Board::GetBombClick()
{
    return bombClicked;
}
/*
    Returns number of cells that are unlocked
*/
int Board::GetCellsOpened()
{
    return cellsOpened;
}
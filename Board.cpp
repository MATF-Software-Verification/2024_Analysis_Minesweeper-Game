#include "Header.h"
#include <vector>
#include <queue>
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
    bombClicked = false;
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
void Board::UnlockBombs()
{
    for (int i = 0; i < mines; i++)
    {
        arr[xCord[i]][yCord[i]].OpenCell();
    }
}
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
    /*
    open cell
    if cell value is 0 -> open adjacent cells
    */
    cout << "\nCell not flagged nor unlocked!\n";
    cout << "\nUnlocking!\n";
    if (arr[row][col].CheckIfBomb())
    {
        bombClicked = true;
        return false;
    }
    if (!arr[row][col].CheckForZero())
    {
        cout << "\nNo gap!\n";
        arr[row][col].OpenCell();
        return true;
    }
    else
    {
        cout << "\nGap discovered!\n";
        queue<int> xQueue;
        queue<int> yQueue;
        vector<int> xVector;
        vector<int> yVector;
        cout << "\nOpening Adjacent cells!\n";
        OpenAdjacentCells(row, col, &xQueue, &yQueue, &xVector, &yVector);
        // Queue generated for cells to open
        cout << "\nQueue Generated!\n";
        while (!xQueue.empty() && !yQueue.empty())
        {
            if (!arr[xQueue.front()][yQueue.front()].IsUnlocked())
                arr[xQueue.front()][yQueue.front()].OpenCell();
            xQueue.pop();
            yQueue.pop();
        }
    }
    return true;
}
void Board::OpenAdjacentCells(int row, int col, queue<int> *xQueue, queue<int> *yQueue, vector<int> *xVector, vector<int> *yVector)
{

    cout << "\nCurrent Cell : " << row + 1 << " " << col + 1 << "\n";

    if (arr[row][col].CheckIfBomb() || arr[row][col].IsUnlocked())
        return;

    bool alreadyPresent = false;
    if (!xVector->empty() && !yVector->empty())
    {
        for (int i = 0; i < xVector->size(); i++)
        {
            cout << "Checking " << row + 1 << " and " << col + 1 << " with\n";
            cout << "\t" << xVector->at(i) + 1 << " and " << yVector->at(i) + 1 << endl
                 << endl;
            if (xVector->at(i) == row && yVector->at(i) == col)
            {
                cout << "\nMatch found\n";
                cout << xVector->at(i) << " equals " << row << endl;
                cout << yVector->at(i) << " equals " << col << endl;
                alreadyPresent = true;
            }
            if (alreadyPresent)
                break;
        }
    }
    if (alreadyPresent)
    {
        cout << "Cell " << row + 1 << " " << col + 1 << " is already queued to open!\n";
        return;
    }
    // Not present in queue
    // Add this cell to queue
    xQueue->push(row);
    yQueue->push(col);
    xVector->push_back(row);
    yVector->push_back(col);

    cout << "\nX Vector : ";
    for (int i = 0; i < xVector->size(); i++)
    {
        cout << xVector->at(i) + 1 << " ";
    }
    cout << "\nY Vector : ";
    for (int i = 0; i < yVector->size(); i++)
    {
        cout << yVector->at(i) + 1 << " ";
    }
    cout << endl
         << endl;

    cout
        << "Pushin " << row + 1 << " " << col + 1 << " into the queue\n";

    if (!arr[row][col].CheckForZero())
    {
        return;
    }

    // Checking top down left and right

    // check top
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
bool Board::PlayerSelect(int row, int col, PlayerMove playerMove)
{
    if (!boardGenerated && playerMove == Open)
    {
        GenerateBoard(row, col);
        boardGenerated = true;
        cout << "Board Generated!\n";
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
        cout << "\nTrying to open cell!\n";
        if (!ClickCell(row, col))
        {
            return false;
        }
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
void Board::GenerateBoard(int row, int col)
{
    /*
        1. Set mines at random position (retry random pos if its equal to player input)
        2. Increment adjacent cells next to mines
    */

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
        xCord.push_back(tempX);
        yCord.push_back(tempY);
    }
    // Increment cells next to mines
    for (int i = 0; i < mines; i++)
    {
        SetValuesAroundMine(xCord[i], yCord[i]);
    }
    ShowBoard();
}
bool Board::GetBombClick()
{
    return bombClicked;
}
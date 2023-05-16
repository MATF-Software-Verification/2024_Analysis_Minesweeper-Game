#include "Header.h"
using namespace std;
/*
    Cell class Constructor
*/
Cell::Cell()
{
    data = '0';
    status = Locked;
    content = Number;
}
/*
    Prints out the data in the cell
        - If cell is locked then prints empty space
        - If cell is flagged then prints out F
*/
void Cell::DisplayCell()
{
    switch (status)
    {
    case Locked:
        cout << ' ';
        break;
    case Unlocked:
        cout << data;
        break;
    case Flagged:
        cout << 'F';
        break;
    default:
        break;
    }
}
/*
    Opens the Cell
        - Set Cell status to unlocked
        - Returns the value inside the cell
*/
int Cell::OpenCell()
{
    status = Unlocked;
    return CharToInt(data);
}
/*
    Checks if Cell is unlocked
*/
bool Cell::IsUnlocked()
{
    if (status == Unlocked)
        return true;
    else
        return false;
}
/*
    Checks if Cell is flagged
*/
bool Cell::IsFlagged()
{
    if (status == Flagged)
        return true;
    else
        return false;
}
/*
    Checks if Cell contains zero
*/
bool Cell::CheckForZero()
{
    if (data == '0')
        return true;
    else
        return false;
}
/*
    Plants a bomb in the Cell
        - Set Cell content to Bomb
        - Set Cell data to *
*/
void Cell::SetBomb()
{
    content = Bomb;
    data = '*';
}
/*
    Checks if Cell is a bomb
*/
bool Cell::CheckIfBomb()
{
    if (content == Bomb)
        return true;
    else
        return false;
}
/*
    Increase the value of data integer in the Cell
*/
void Cell::IncreaseValue()
{
    if (content != Bomb)
    {
        int x = CharToInt(data);
        x++;
        data = IntToChar(x);
    }
}
/*
    Places / Removes flag in a cell
        - If flag is places it returns true
        - If flag is removed it returns false
*/
bool Cell::TriggerFlag()
{
    if (status == Locked)
    {
        status = Flagged;
        return true;
    }
    else
    {
        status = Locked;
        return false;
    }
}
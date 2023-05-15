#include "Header.h"
using namespace std;

Cell::Cell()
{
    data = '0';
    status = Locked;
    content = Number;
}
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
int Cell::OpenCell()
{
    status = Unlocked;
    return CharToInt(data);
}
bool Cell::IsUnlocked()
{
    if (status == Unlocked)
        return true;
    else
        return false;
}
bool Cell::IsFlagged()
{
    if (status == Flagged)
        return true;
    else
        return false;
}
bool Cell::CheckForZero()
{
    if (data == '0')
        return true;
    else
        return false;
}
void Cell::SetBomb()
{
    content = Bomb;
    data = '*';
}
bool Cell::CheckIfBomb()
{
    if (content == Bomb)
        return true;
    else
        return false;
}
void Cell::IncreaseValue()
{
    if (content != Bomb)
    {
        int x = CharToInt(data);
        x++;
        data = IntToChar(x);
    }
}
bool Cell::TriggerFlag()
{
    if (status == Unlocked)
        return false;
    if (status == Locked)
        status = Flagged;
    else
        status = Locked;
    return true;
}
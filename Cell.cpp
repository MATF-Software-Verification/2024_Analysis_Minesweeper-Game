#include "Header.h"
using namespace std;

Cell::Cell()
{
    data = '0';
    status = Locked;
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
bool Cell::OpenCell()
{
    if (status == Unlocked)
        return false;
    else
        status = Unlocked;
    return true;
}
void Cell::SetBomb()
{
    content = Bomb;
    data = '*';
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
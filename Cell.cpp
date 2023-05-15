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
void Cell::OpenCell()
{
    status = Unlocked;
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
void Cell::SetFlagged()
{
    if (status == Locked)
        status = Flagged;
}
#include "Header.h"
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
    cout << "     ";
    for (int i = 0; i < size; i++)
    {
        cout << i + 1 << " ";
    }
    cout << "\n\n";
    for (int i = 0; i < size; i++)
    {
        cout << i + 1 << "   ";
        cout << "|";
        for (int j = 0; j < size; j++)
        {
            arr[i][j].DisplayCell();
            cout << "|";
        }
        cout << "\n";
    }
    cout << "\n";
}
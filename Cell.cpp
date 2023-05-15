#include "Header.h"
using namespace std;

Cell::Cell()
{
    data = ' ';
    status = Locked;
}
void Cell::DisplayCell()
{
    cout << data;
}
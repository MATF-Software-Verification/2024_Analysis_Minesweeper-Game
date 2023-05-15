#include "Header.h"
using namespace std;

Minesweeper::Minesweeper()
{
    board = new Board(9);
}
void Minesweeper::Play()
{
    Instructions();
    board->ShowBoard();
}
void Minesweeper::Instructions()
{
    cout << "\nWELCOME TO MINESWEEPER GAME!\n";
    cout << "\nThese are the rules: \n";
    cout << "\n- The board is divided into cells, with mines randomly distributed.\n- To win, you need to open all the cells. \n- The number on a cell shows the number of mines adjacent to it. Using this information, you can determine cells that are safe, and cells that contain mines.\n- Interact, evolve and enjoy!\n\n";
}
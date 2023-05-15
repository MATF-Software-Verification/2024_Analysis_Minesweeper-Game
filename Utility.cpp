#include "Header.h"
using namespace std;

int CharToInt(char c)
{
    if ((int)c > 47 && (int)c < 58)
    {
        return (int)c - 48;
    }
    return -1;
}
char IntToChar(int x)
{
    if (x > 0 && x < 5)
    {
        return (char)x + 48;
    }
    return '-';
}
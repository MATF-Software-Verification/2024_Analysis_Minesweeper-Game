#include "Header.h"
#include <random>
using namespace std;
/*
    To convert char into int
*/
int CharToInt(char c)
{
    if ((int)c > 47 && (int)c < 58)
    {
        return (int)c - 48;
    }
    return -1;
}
/*
    To convert int to char
*/
char IntToChar(int x)
{
    if (x > 0 && x < 5)
    {
        return (char)x + 48;
    }
    return '-';
}
random_device rd;
mt19937 gen(rd());
/*
    Random Number Generator
*/
int GetRandom(int min, int max)
{
    uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}
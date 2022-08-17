

#include "Utils.h"
#include <iostream>

using namespace std;

int Utils::seed;
std::mt19937 Utils::rand;

void Utils::initRand(int _seed)
{
    if(_seed == 0)
    {
        std::random_device rd; // obtain a random number from hardware - seed for mt19937
        seed = rd();
    }
    else
        seed = _seed;

    rand  = std::mt19937(seed); // seed the generator
    cout << seed << endl;
}
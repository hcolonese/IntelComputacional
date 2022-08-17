
#ifndef CCP_UTILS_H
#define CCP_UTILS_H

#include <random>


class Utils {
public:
    static int seed;
    static std::mt19937 rand;

    static void initRand(int _seed);

};



#endif //CCP_UTILS_H

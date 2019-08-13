//
// Created by Y on 6/8/2019.
//

#ifndef HW5_COMPI_REG_UTILS_H
#define HW5_COMPI_REG_UTILS_H

using namespace std;

#include <string>
#define MAX_REGS 15
#include <assert.h>

string getRegName(int number)   {
    switch (number) {
        case 0 : return "$s0";
        case 1 : return "$s1";
        case 2 : return "$s2";
        case 3 : return "$s3";
        case 4 : return "$s4";
        case 5 : return "$s5";
        case 6 : return "$s6";
        case 7 : return "$s7";
        case 8 : return "$t0";
        case 9 : return "$t1";
        case 10 : return "$t2";
        case 11 : return "$t3";
        case 12 : return "$t4";
        case 13 : return "$t5";
        case 14 : return "$t6";
        case 15 : return "$t7";
        default: {
            assert(false);      //T0D0 : remove
            return "";
        }
    }
}

class Regs  {
    public:
    int next_available;

    Regs() : next_available(0)  {}

    string allocateReg()    {
        if (next_available > MAX_REGS) {
            assert(false);      //T0D0 : remove
            return "";
        }
        return getRegName(next_available++);
    }

    void releaseReg()   {
        if (next_available > 0)    {
            next_available--;
        }
        else {
            assert(false);      //T0D0 : remove
        }
    }


};

#endif //HW5_COMPI_REG_UTILS_H

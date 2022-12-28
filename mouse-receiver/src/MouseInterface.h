#ifndef MHI
#define MHi

#include <Arduino.h>
#include <TouchScreen.h>
#include "Functions.h"

typedef TSPoint TDCoord;

class MouseInterface
{

public:
    static TDCoord getCommand(String);
};

TDCoord MouseInterface::getCommand(String inp)
{
    if (inp[0] == 'L')
    {
        Serial.println("Click");
        return TDCoord(0, 0, 1);
    }
    else if (inp[0] == 'M')
    {
        String rest = inp.substring(3);
        String x = getValue(rest, ',', 0);
        String y = getValue(rest, ',', 1);
        return TDCoord(x.toInt(), y.toInt(), 0);
    }
    return TDCoord(0, 0, 0);
}

#endif
#include <Arduino.h>

class Timer {
public:
    static unsigned long inXms(int extra);

    static unsigned long inXs(int extra);

    static unsigned long inXm(int extra);

    static bool isPassed(unsigned long date);
};
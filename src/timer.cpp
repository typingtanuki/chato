#include "timer.hpp"

unsigned long Timer::inXms(int extra) {
    unsigned long now = millis();
    return now + extra;
}

unsigned long Timer::inXs(int extra) {
    return inXms(extra * 1000);
}

unsigned long Timer::inXm(int extra) {
    return inXs(extra * 60);
}

bool Timer::isPassed(unsigned long date) {
    return date < millis();
}

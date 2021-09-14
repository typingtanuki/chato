#include <Arduino.h>
#include <MuxShield.h>

#include "constants.hpp"
#include "keypad.hpp"
#include "oni.hpp"

Oni *oni;
MuxShield mux;
Keypad keypad = Keypad(&mux, ANALOG_PIN5, 1, 2);

void setup() {
    Serial.begin(115200);
    randomSeed(analogRead(ANALOG_PIN0));
    oni = new Oni(&keypad);
    oni->init();
}

void loop() {
    while (true) {
        // Delay for a period of time (in milliseconds).
        delay(10);
        keypad.loop();
        oni->loop();
    }
}


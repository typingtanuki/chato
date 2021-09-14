#include "keypad.hpp"
#include "constants.hpp"

void Keypad::initMux() {
    mux->setMode(muxDigitalOutput, DIGITAL_OUT);
    mux->setMode(muxAnalogInput, ANALOG_IN);
    showState();
}

void Keypad::showState() {
    for (int i = 0; i <= KEYPAD_STATE_COUNT; i++) {
        mux->digitalWriteMS(muxDigitalOutput, i, state[i]);
    }
}

Keypad::Keypad(MuxShield *mux, int analogOutput, int digitalInput, int analogInput) {
    this->mux = mux;
    this->analogOutput = analogOutput;
    muxDigitalOutput = digitalInput;
    muxAnalogInput = analogInput;
    state = new bool[KEYPAD_STATE_COUNT];
    for (int i = 0; i < KEYPAD_STATE_COUNT; i++) {
        state[i] = false;
    }
    initMux();
}

void Keypad::loop() {
    unsigned int keypress = analogRead(this->analogOutput);
    speed = 1 + mux->analogReadMS(muxAnalogInput, 0);

    if (keypress >= KEYPAD_TENSION_P1) {
        button = BUTTON_EYE_MOVE_AUTO;
    } else if (keypress >= KEYPAD_TENSION_P2) {
        button = BUTTON_EYE_MOVE_LEFT;
    } else if (keypress >= KEYPAD_TENSION_P3) {
        button = BUTTON_AXE_MOVE_UP;
    } else if (keypress >= KEYPAD_TENSION_P4) {
        button = BUTTON_AXE_LIGHT_AUTO;
    } else if (keypress >= KEYPAD_TENSION_P5) {
        button = BUTTON_AXE_MOVE_AUTO;
    } else if (keypress >= KEYPAD_TENSION_P6) {
        button = BUTTON_NONE;

        lastButton = button;
        prevButton = button;
    } else if (keypress >= KEYPAD_TENSION_P7) {
        button = BUTTON_EYE_LIGHT_MANUAL;
    } else if (keypress >= KEYPAD_TENSION_P8) {
        button = BUTTON_AXE_MOVE_DOWN;
    } else if (keypress >= KEYPAD_TENSION_P9) {
        button = BUTTON_AXE_LIGHT_MANUAL;
    } else if (keypress >= KEYPAD_TENSION_P10) {
        button = BUTTON_EYE_MOVE_RIGHT;
    } else {
        button = BUTTON_EYE_LIGHT_AUTO;
    }

    showState();
}

KeypadButton Keypad::currentKey() {
    lastButton = prevButton;
    prevButton = button;
    return button;
}

int Keypad::currentSpeed() const {
    return speed;
}

KeypadButton Keypad::lastKey() const {
    return lastButton;
}

void Keypad::setState(int stateId, bool enabled) {
    this->state[stateId - 1] = enabled;
}

void Keypad::testMode(int level) {
    for (int i = 0; i < KEYPAD_STATE_COUNT; i++) {
        state[i] = false;
        if (level >= 1) {
            if (i == 0 || i == 4 || i == 6) {
                state[i] = true;
            }
        }
        if (level >= 2) {
            if (i == 1 || i == 5 || i == 7) {
                state[i] = true;
            }
        }
        if (level >= 3) {
            if (i == 2 || i == 8) {
                state[i] = true;
            }
        }
        if (level >= 4) {
            if (i == 3 || i == 9) {
                state[i] = true;
            }
        }
    }
}

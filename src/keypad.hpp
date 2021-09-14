#pragma once
#include <MuxShield.h>


enum KeypadButton {
    BUTTON_NONE,

    BUTTON_EYE_MOVE_AUTO,
    BUTTON_EYE_MOVE_LEFT,
    BUTTON_EYE_MOVE_RIGHT,

    BUTTON_AXE_MOVE_AUTO,
    BUTTON_AXE_MOVE_UP,
    BUTTON_AXE_MOVE_DOWN,

    BUTTON_EYE_LIGHT_AUTO,
    BUTTON_EYE_LIGHT_MANUAL,

    BUTTON_AXE_LIGHT_AUTO,
    BUTTON_AXE_LIGHT_MANUAL,
};

class Keypad {
private:
    KeypadButton button = BUTTON_NONE;
    KeypadButton prevButton = BUTTON_NONE;
    KeypadButton lastButton = BUTTON_NONE;

    bool *state;

    int analogOutput;

    MuxShield *mux;
    int muxDigitalOutput;
    int muxAnalogInput;

    int speed = 50;

    void initMux();

    void showState();

public:
    Keypad(MuxShield *mux, int analogOutput, int digitalInput, int analogInput);

    void loop();

    KeypadButton currentKey();

    int currentSpeed() const;

    KeypadButton lastKey() const;

    void setState(int state, bool enabled);

    void testMode(int level);

};

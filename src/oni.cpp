#include "oni.hpp"

Oni::Oni(Keypad *keypad) : keypad(keypad) {
    this->keypad = keypad;
}

void Oni::init() {
    eye->init();
    eyeMotor->init();
    axe->init();
    nextOniTime = Timer::inXs(0);
}

void Oni::loop() {
    switch (oniState) {
        case INIT:
            if (eyeMotor->test()) {
                // Testing
                return;
            } else if (Timer::isPassed(nextOniTime)) {
                switch (eyeState) {
                    case OFF:
                        Serial.println("Testing SPIN");
                        eyeState = SPIN;
                        keypad->testMode(1);
                        break;
                    case SPIN:
                        Serial.println("Testing FIRE");
                        eyeState = FIRE;
                        keypad->testMode(2);
                        break;
                    case FIRE:
                        Serial.println("Testing RAINBOW");
                        keypad->testMode(3);
                        eyeState = RAINBOW;
                        break;
                    case RAINBOW:
                        Serial.println("Testing RAINBOW_RED");
                        keypad->testMode(4);
                        eyeState = RAINBOW_RED;
                        break;
                    case RAINBOW_RED:
                    default:
                        Serial.println("Testing finished");
                        keypad->testMode(0);
                        oniState = IDLE;
                        eyeMotor->setMode(AUTO);

                        autoEyeControl();
                        autoAxeControl();

                        autoEyeLightControl();
                        autoAxeLightControl();

                        nextOniTime = Timer::inXs(30);
                        return;
                }
                nextOniTime = Timer::inXs(10);
            }
            break;
        case IDLE:
            if (Timer::isPassed(nextOniTime)) {
                oniState = newOniState();
                nextOniTime = Timer::inXm(1);
            }
            if (Timer::isPassed(nextEyeTime)) {
                newEyeState();
            }
            if (Timer::isPassed(nextAxeTime)) {
                newAxeState();
            }
            break;
        case MOVE:
            if (Timer::isPassed(nextOniTime)) {
                oniState = IDLE;
                nextOniTime = Timer::inXs(30);
            }

            if (autoAxeState) {
                axeState = FIRE;
            }
            if (autoEyeState) {
                eyeState = SPIN;
            }
            break;
        default:
            break;
    }

    eye->loop(eyeState, keypad->currentSpeed());
    axe->loop(axeState, keypad->currentSpeed());

    if (Timer::isPassed(nextEyeMoveTime)) {
        eyeMotor->randTarget();
        nextEyeMoveTime = Timer::inXs(2);
    }
    switch (keypad->currentKey()) {
        case BUTTON_EYE_MOVE_AUTO:
            autoEyeControl();
            break;
        case BUTTON_EYE_MOVE_LEFT:
            eyeMotor->setMode(MANUAL);
            eyeMotor->turnLeft();
            keypad->setState(1, false);
            keypad->setState(5, true);
            keypad->setState(7, true);
            break;
        case BUTTON_EYE_MOVE_RIGHT:
            eyeMotor->setMode(MANUAL);
            eyeMotor->turnRight();
            keypad->setState(1, false);
            keypad->setState(5, true);
            keypad->setState(7, true);
            break;
        case BUTTON_AXE_MOVE_AUTO:
            autoAxeControl();
            break;
        case BUTTON_AXE_MOVE_UP:
            axeMotor->setMode(MANUAL);
            axeMotor->turnLeft();
            keypad->setState(2, false);
            keypad->setState(6, true);
            keypad->setState(8, true);
            break;
        case BUTTON_AXE_MOVE_DOWN:
            axeMotor->setMode(MANUAL);
            axeMotor->turnRight();
            keypad->setState(2, false);
            keypad->setState(6, true);
            keypad->setState(8, true);
            break;
        case BUTTON_EYE_LIGHT_AUTO:
            autoEyeLightControl();
            break;
        case BUTTON_EYE_LIGHT_MANUAL:
            if (!autoEyeState && keypad->lastKey() != 9) {
                eyeState = nextLightPattern(eyeState);
            }
            autoEyeState = false;
            keypad->setState(3, false);
            keypad->setState(9, true);
            break;
        case BUTTON_AXE_LIGHT_AUTO:
            autoAxeLightControl();
            break;
        case BUTTON_AXE_LIGHT_MANUAL:
            if (!autoAxeState && keypad->lastKey() != 10) {
                axeState = nextLightPattern(axeState);
            }
            autoAxeState = false;
            keypad->setState(4, false);
            keypad->setState(10, true);
            break;
        default:
            break;
    }
    eyeMotor->loop();
    keypad->loop();
}

void Oni::autoEyeControl() {
    eyeMotor->setMode(AUTO);
    keypad->setState(1, true);
    keypad->setState(5, false);
    keypad->setState(7, false);
}

void Oni::autoAxeControl() {
    axeMotor->setMode(AUTO);
    keypad->setState(2, true);
    keypad->setState(6, false);
    keypad->setState(8, false);
}

void Oni::autoEyeLightControl() {
    autoEyeState = true;
    keypad->setState(3, true);
    keypad->setState(9, false);
}

void Oni::autoAxeLightControl() {
    autoAxeState = true;
    keypad->setState(4, true);
    keypad->setState(10, false);
}

void Oni::newEyeState() {
    if (!autoEyeState) {
        return;
    }

    switch (eyeState) {
        case OFF:
            eyeState = pickEyeState();
            nextEyeTime = Timer::inXs(12);
            break;
        default:
            eyeState = OFF;
            nextEyeTime = Timer::inXs(5);
    }
}

LightState Oni::nextLightPattern(LightState current) {
    int next = current + 1;
    if (next > RAINBOW_RED) {
        return OFF;
    }
    return (LightState) next;
}

LightState Oni::pickEyeState() {
    switch (random(0, 4)) {
        case 1:
            return SPIN;
        case 2:
            return RAINBOW;
        case 3:
            return RAINBOW_RED;
        default:
            return FIRE;
    }
}

void Oni::newAxeState() {
    if (!autoAxeState) {
        return;
    }

    switch (eyeState) {
        case OFF:
            axeState = pickAxeState();
            nextAxeTime = Timer::inXs(20);
            break;
        default:
            axeState = OFF;
            nextAxeTime = Timer::inXs(10);
    }
}

LightState Oni::pickAxeState() {
    switch (random(0, 3)) {
        case 1:
            return SPIN;
        case 2:
            return RAINBOW;
        default:
            return RAINBOW_RED;
    }
}

OniState Oni::newOniState() {
    int test = (int) random(0, 10);
    if (test < 2) {
        return IDLE;
    }
    return MOVE;
}

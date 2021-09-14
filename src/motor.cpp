#include "motor.hpp"

Motor::Motor(int pin) {
    this->pin = pin;
}

void Motor::init() {
    servo.attach(pin);
}

bool Motor::test() {
    if (tested) {
        return false;
    }
    pos++;
    Serial.println(pos);
    servo.write(pos);
    tested = pos >= SERVO_MAX;
    return !tested;
}

void Motor::loop() {
    int diff = target - pos;
    if (diff > SERVO_SPEED) {
        diff = SERVO_SPEED;
    } else if (diff < -SERVO_SPEED) {
        diff = -SERVO_SPEED;
    }
    if (diff < -1 || diff > 1) {
        pos = pos + diff / 2;
        servo.write(pos);
    }
}

void Motor::setMode(bool isAuto) {
    mode = isAuto;
}

void Motor::randTarget() {
    if (mode == MANUAL) {
        return;
    }

    target = (int) random(SERVO_MIN, SERVO_MAX);
}

void Motor::turnLeft() {
    target -= 1;
    if (target < SERVO_MIN) {
        target = SERVO_MIN;
    }
}

void Motor::turnRight() {
    target += 1;
    if (target > SERVO_MAX) {
        target = SERVO_MAX;
    }
}

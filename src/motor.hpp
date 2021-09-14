#include <Arduino.h>
#include <Servo.h>

#define AUTO true
#define MANUAL false

class Motor {
private:
    int pin;

    bool tested = false;
    bool mode = false;

    int pos = 0;
    int target = 0;
    Servo servo;

    int SERVO_MIN = 0;
    int SERVO_MAX = 179;
    int SERVO_SPEED = 6;

public:
    explicit Motor(int pin);

    void init();

    bool test();

    void loop();

    void randTarget();

    void setMode(bool isAuto);

    void turnLeft();

    void turnRight();
};

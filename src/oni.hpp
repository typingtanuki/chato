#include "constants.hpp"
#include "light.hpp"
#include "motor.hpp"
#include "timer.hpp"
#include "keypad.hpp"

enum OniState {
    INIT,
    IDLE,
    MOVE
};

class Oni {
private:
    Light *eye = new Light(DIGITAL_PIN3, 12, 24);
    Motor *eyeMotor = new Motor(DIGITAL_PIN9);
    Motor *axeMotor = new Motor(DIGITAL_PIN13);
    Light *axe = new Light(DIGITAL_PIN5, 60, 60);
    Keypad *keypad;

    bool autoEyeState = false;
    bool autoAxeState = false;

    unsigned long nextEyeTime = 0;
    unsigned long nextAxeTime = 0;
    unsigned long nextEyeMoveTime = 0;
    unsigned long nextOniTime = 0;

    LightState eyeState = OFF;
    LightState axeState = OFF;
    OniState oniState = INIT;

    void autoEyeControl();

    void autoAxeControl();

    void autoEyeLightControl();

    void autoAxeLightControl();

    void newEyeState();

    static LightState nextLightPattern(LightState current);

    static LightState pickEyeState();

    void newAxeState();

    static LightState pickAxeState();

    static OniState newOniState();

public:
    explicit Oni(Keypad *keypad);

    void init();

    void loop();
};

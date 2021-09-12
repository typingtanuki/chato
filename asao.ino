#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <MuxShield.h>
#include <Servo.h>

#define PIN0_R 0 // Random seed
#define PIN1 1
#define PIN2_R 2 // Mux shield
#define PIN3 3   // Eyes
#define PIN4_R 4 // Mux shield
#define PIN5 5   // Axe
#define PIN6_R 6 // Mux shield
#define PIN7_R 7 // Mux shield
#define PIN8_R 8 // Mux shield
#define PIN9 9 // Motor
#define PIN10_R 10 // Mux shield
#define PIN11_R 11 // Mux shield
#define PIN12_R 12 // Mux shield
#define PIN13 13

#define MAX_BRIGHTNESS 0.3


#define KEYPAD_PIN 5 // Keypad analog
#define KEYPAD_STATE_COUNT 10 // Number of LEDs on keypad

#define BUTTON_1 876
#define BUTTON_5 767
#define BUTTON_6 682
#define BUTTON_4 614
#define BUTTON_2 558
#define BUTTON_NONE 512
#define BUTTON_9 465
#define BUTTON_8 409
#define BUTTON_10 340
#define BUTTON_7 254
#define BUTTON_3 145

#define AUTO true
#define MANUAL false

class Keypad {
private:
    unsigned short button = 0;
    unsigned short prevButton = 0;
    unsigned short lastButton = 0;

    unsigned int p1 = 0;
    unsigned int p2 = 0;
    unsigned int p3 = 0;
    unsigned int p4 = 0;
    unsigned int p5 = 0;
    unsigned int p6 = 0;
    unsigned int p7 = 0;
    unsigned int p8 = 0;
    unsigned int p9 = 0;
    unsigned int p10 = 0;

    bool *state;

    MuxShield *mux;
    int muxDigitalOutput;
    int muxAnalogInput;
    int speed;

    void initMux() {
        speed = 51;
        mux->setMode(muxDigitalOutput, DIGITAL_OUT);
        mux->setMode(muxAnalogInput, ANALOG_IN);
        showState();
    }

    void showState() {
        for (int i = 0; i <= KEYPAD_STATE_COUNT; i++) {
            mux->digitalWriteMS(muxDigitalOutput, i, state[i]);
        }
    }

public:
    Keypad(MuxShield *mux, int digitalInput, int analogInput) {
        this->mux = mux;
        muxDigitalOutput = digitalInput;
        muxAnalogInput = analogInput;
        state = new bool[KEYPAD_STATE_COUNT];
        for (int i = 0; i < KEYPAD_STATE_COUNT; i++) {
            state[i] = false;
        }
        initMux();

        p1 = (BUTTON_1 + BUTTON_5) / 2;
        p2 = (BUTTON_5 + BUTTON_6) / 2;
        p3 = (BUTTON_6 + BUTTON_4) / 2;
        p4 = (BUTTON_4 + BUTTON_2) / 2;
        p5 = (BUTTON_2 + BUTTON_NONE) / 2;
        p6 = (BUTTON_NONE + BUTTON_9) / 2;
        p7 = (BUTTON_9 + BUTTON_8) / 2;
        p8 = (BUTTON_8 + BUTTON_10) / 2;
        p9 = (BUTTON_10 + BUTTON_7) / 2;
        p10 = (BUTTON_7 + BUTTON_3) / 2;
    }

    void loop() {
        int keypress = analogRead(KEYPAD_PIN);
        speed = 1 + mux->analogReadMS(muxAnalogInput, 0) / 20;

        if (keypress >= p1) {
            button = 1;
        } else if (keypress >= p2) {
            button = 5;
        } else if (keypress >= p3) {
            button = 6;
        } else if (keypress >= p4) {
            button = 4;
        } else if (keypress >= p5) {
            button = 2;
        } else if (keypress >= p6) {
            button = 0;
            lastButton = button;
            prevButton = button;
        } else if (keypress >= p7) {
            button = 9;
        } else if (keypress >= p8) {
            button = 8;
        } else if (keypress >= p9) {
            button = 10;
        } else if (keypress >= p10) {
            button = 7;
        } else {
            button = 3;
        }

        showState();
    }

    int currentKey() {
        lastButton = prevButton;
        prevButton = button;
        return button;
    }

    int currentSpeed() {
        return speed;
    }

    int lastKey() {
        return lastButton;
    }

    void setState(int state, bool enabled) {
        this->state[state - 1] = enabled;
    }

    void testMode(int level) {
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
};


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
    explicit Motor(int pin) {
        this->pin = pin;
    }

    void init() {
        servo.attach(pin);
    }

    boolean test() {
        if (tested) {
            return false;
        }
        pos++;
        Serial.println(pos);
        servo.write(pos);
        tested = pos >= SERVO_MAX;
        return !tested;
    }

    void loop() {
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

    void randTarget() {
        if (mode == MANUAL) {
            return;
        }

        target = (int) random(SERVO_MIN, SERVO_MAX);
    }

    void setMode(boolean isAuto) {
        mode = isAuto;
    }

    void turnLeft() {
        target -= 1;
        if (target < SERVO_MIN) {
            target = SERVO_MIN;
        }
    }

    void turnRight() {
        target += 1;
        if (target > SERVO_MAX) {
            target = SERVO_MAX;
        }
    }
};

class Timer {
public:
    static unsigned long inXms(int extra) {
        unsigned long now = millis();
        return now + extra;
    }

    static unsigned long inXs(int extra) {
        return inXms(extra * 1000);
    }

    static unsigned long inXm(int extra) {
        return inXs(extra * 60);
    }

    static bool isPassed(unsigned long date) {
        return date < millis();
    }
};

enum LightState {
    OFF, SPIN, FIRE, RAINBOW, RAINBOW_RED
};

enum OniState {
    INIT,
    IDLE,
    MOVE
};

class Light {
private:
    Adafruit_NeoPixel *neo;

    uint32_t *color;
    uint32_t *tmpColor;
    int *buffer;

    int length;
    int fullLength;
    int position1;
    int position2;
    int position3;

public:
    Light(int pin, int stripLength, int fullLength) {
        length = stripLength;
        this->fullLength = fullLength;
        color = new uint32_t[length];
        tmpColor = new uint32_t[length];
        buffer = new int[length];
        neo = new Adafruit_NeoPixel(fullLength, pin, NEO_GRB + NEO_KHZ800);

        position1 = 0;
        position2 = length / 3;
        position3 = position2 * 2;
    }

    ~Light() {
        delete[] color;
        delete[] tmpColor;
        delete[] buffer;
        delete neo;
    }

    void init() {
        neo->begin();
        turnOff();
    }

    void loop(LightState state, int simulationSpeed) {
        unsigned long steps = simulationSpeed;

        Serial.println("Simulation:");
        Serial.print("- simulationSpeed:");
        Serial.println(simulationSpeed);
        Serial.print("- steps:");
        Serial.println(steps);

        switch (state) {
            case SPIN:
                spinLoop(steps);
                break;
            case FIRE:
                fireLoop(steps);
                break;
            case RAINBOW:
                rainbowLoop(false, steps);
                break;
            case RAINBOW_RED:
                rainbowLoop(true, steps);
                break;
            default:
                turnOff();
                break;
        }

        apply();
    }

    void spinLoop(int steps) {
        for (int i = 0; i < steps; i++) {
            doDecay();
            moveCenter();
        }
    }

    void doDecay() {
        for (int i = 0; i < length; i++) {
            uint32_t current = color[i];
            int r = computeDecay(unpackR(current));
            int g = computeDecay(unpackG(current));
            int b = computeDecay(unpackB(current));
            color[i] = Adafruit_NeoPixel::Color(r, g, b);
        }
    }

    static int computeDecay(int value) {
        int out = value - (int) random(0, 12);
        if (out > 0) {
            out = out + (int) random(-5, 5);
        }
        if (out < 0) {
            out = 0;
        }
        if (out > 255) {
            out = 255;
        }
        return out;
    }

    static int unpackR(uint32_t color) {
        return (int) ((color >> 16) & 0xFF);
    }

    static int unpackG(uint32_t color) {
        return (int) ((color >> 8) & 0xFF);
    }

    static int unpackB(uint32_t color) {
        return (int) (color & 0xFF);
    }

    void moveCenter() {
        position1++;
        position2++;
        position3++;

        while (position1 >= length) {
            position1 -= length;
        }
        while (position2 >= length) {
            position2 -= length;
        }
        while (position3 >= length) {
            position3 -= length;
        }

        uint32_t lead = Adafruit_NeoPixel::Color(200, 0, 0);
        // Lead
        color[position1] = lead;
        color[position2] = lead;
        color[position3] = lead;
    }

    void fireLoop(int steps) {
        for (int i = 0; i < steps; i++) {
            color[length / 2] = Adafruit_NeoPixel::Color(random(0, 255), 0, 0);

            for (int i = 0; i < length / 2; i++) {
                int left = remapLeft(i);
                int leftPrev = remapLeft(i - 1);
                int right = remapRight(i);
                int rightPrev = remapRight(i - 1);

                tmpColor[left] = Adafruit_NeoPixel::Color(
                        (unpackR(color[left]) + unpackR(color[leftPrev])) / random(2, 4),
                        0,
                        0);
                tmpColor[right] = Adafruit_NeoPixel::Color(
                        (unpackR(color[right]) + unpackR(color[rightPrev])) / random(2, 4),
                        0,
                        0);
            }
            uint32_t *tmp = color;
            color = tmpColor;
            tmpColor = tmp;
        }
    }

    void rainbowLoop(bool redOnly, int steps) {
        for (int i = 0; i < steps; i++) {
            for (int i = 0; i < length; i++) {
                position1++;
                int r = rainbow(position1 % 256);
                int g = 0;
                int b = 0;
                if (!redOnly) {
                    g = rainbow((position1 + 85) % 256);
                    b = rainbow((position1 + 170) % 256);
                }
                color[i] = Adafruit_NeoPixel::Color(r, g, b);
            }
            position1 -= length - 3;
        }
    }

    static int rainbow(byte WheelPos) {
        if (WheelPos < 85) {
            return WheelPos * 3;
        }
        if (WheelPos < 170) {
            WheelPos -= 85;
            return 255 - WheelPos * 3;
        }

        return 0;
    }

    int remapRight(int index) const {
        int val = length / 2 + index + 1;
        int over = val - length;
        if (over >= 0) {
            val = over;
        }
        return val;
    }

    int remapLeft(int index) const {
        int val = length / 2 - index;
        if (val < 0) {
            val = length + val;
        }
        return val;
    }

    void turnOff() {
        for (int i = 0; i < length; i++) {
            color[i] = Adafruit_NeoPixel::Color(0, 0, 0);
        }
    }

    void apply() {
        for (int i = 0; i < length; i++) {
            int r = brightness(unpackR(color[i]));
            int g = brightness(unpackG(color[i]));
            int b = brightness(unpackB(color[i]));
            neo->setPixelColor(i, Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::Color(r, g, b)));
            if (length + i < fullLength) {
                neo->setPixelColor(i + length, Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::Color(r, g, b)));
            }
        }
        neo->show();
    }

    static int brightness(int color) {
        return (int) (color * MAX_BRIGHTNESS);
    }
};


class Oni {
private:
    Light *eye = new Light(PIN3, 12, 24);
    Motor *eyeMotor = new Motor(PIN9);
    Motor *axeMotor = new Motor(PIN13);
    Light *axe = new Light(PIN5, 60, 60);
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

public:
    void init(Keypad *keypad) {
        this->keypad = keypad;

        eye->init();
        eyeMotor->init();
        axe->init();
        nextOniTime = Timer::inXs(0);
    }

    void loop() {
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
            case 1:
                autoEyeControl();
                break;
            case 5:
                eyeMotor->setMode(MANUAL);
                eyeMotor->turnLeft();
                keypad->setState(1, false);
                keypad->setState(5, true);
                keypad->setState(7, true);
                break;
            case 7:
                eyeMotor->setMode(MANUAL);
                eyeMotor->turnRight();
                keypad->setState(1, false);
                keypad->setState(5, true);
                keypad->setState(7, true);
                break;
            case 2:
                autoAxeControl();
                break;
            case 6:
                axeMotor->setMode(MANUAL);
                axeMotor->turnLeft();
                keypad->setState(2, false);
                keypad->setState(6, true);
                keypad->setState(8, true);
                break;
            case 8:
                axeMotor->setMode(MANUAL);
                axeMotor->turnRight();
                keypad->setState(2, false);
                keypad->setState(6, true);
                keypad->setState(8, true);
                break;
            case 3:
                autoEyeLightControl();
                break;
            case 9:
                if (!autoEyeState && keypad->lastKey() != 9) {
                    eyeState = nextLightPattern(eyeState);
                }
                autoEyeState = false;
                keypad->setState(3, false);
                keypad->setState(9, true);
                break;
            case 4:
                autoAxeLightControl();
                break;
            case 10:
                if (!autoEyeState && keypad->lastKey() != 10) {
                    axeState = nextLightPattern(axeState);
                }
                autoAxeState = false;
                axeMotor->turnLeft();
                keypad->setState(4, false);
                keypad->setState(10, true);
                break;
        }
        eyeMotor->loop();
        keypad->loop();
    }

    void autoEyeControl() {
        eyeMotor->setMode(AUTO);
        keypad->setState(1, true);
        keypad->setState(5, false);
        keypad->setState(7, false);
    }

    void autoAxeControl() {
        axeMotor->setMode(AUTO);
        keypad->setState(2, true);
        keypad->setState(6, false);
        keypad->setState(8, false);
    }

    void autoEyeLightControl() {
        autoEyeState = true;
        keypad->setState(3, true);
        keypad->setState(9, false);
    }

    void autoAxeLightControl() {
        autoAxeState = true;
        keypad->setState(4, true);
        keypad->setState(10, false);
    }

    void newEyeState() {
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

    static LightState nextLightPattern(LightState current) {
        int next = current + 1;
        if (next > RAINBOW_RED) {
            return OFF;
        }
        return (LightState) next;
    }

    static LightState pickEyeState() {
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

    void newAxeState() {
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

    static LightState pickAxeState() {
        switch (random(0, 3)) {
            case 1:
                return SPIN;
            case 2:
                return RAINBOW;
            default:
                return RAINBOW_RED;
        }
    }

    static OniState newOniState() {
        int test = (int) random(0, 10);
        if (test < 2) {
            return IDLE;
        }
        return MOVE;
    }
};

Oni oni;
MuxShield mux;
Keypad keypad = Keypad(&mux, 1, 2);

void setup() {
    Serial.begin(115200);
    randomSeed(analogRead(PIN0_R));
    oni.init(&keypad);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void loop() {
    while (true) {
        // Delay for a period of time (in milliseconds).
        delay(10);
        keypad.loop();
        oni.loop();
    }
}

#pragma clang diagnostic pop

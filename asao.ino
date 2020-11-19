#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define PIN0_R 0 // Random seed
#define PIN1 1
#define PIN2_R 2 // Mux shield
#define PIN3 3   // Eyes
#define PIN4_R 4 // Mux shield
#define PIN5 5   // Belly
#define PIN6_R 6 // Mux shield
#define PIN7_R 7 // Mux shield
#define PIN8_R 8 // Mux shield
#define PIN9 9 // Axe
#define PIN10_R 10 // Mux shield
#define PIN11_R 11 // Mux shield
#define PIN12_R 12 // Mux shield
#define PIN13 13

#define MAX_BRIGHTNESS 0.3

class Motor {
private:
    int pin;
    int pos = 0;
    Servo servo;

    int SERVO_MIN = 45;
    int SERVO_MAX = 135;
    int SERVO_MID = (SERVO_MAX - SERVO_MIN) / 2;
    int SERVO_LOW = (SERVO_MAX - SERVO_MIN) / 4;
    int SERVO_HIGH = 3 * SERVO_LOW;

public:
    explicit Motor(int pin) {
        this->pin = pin;
    }

    void init() {
        servo.attach(pin);
    }

    void loop() {
        servo.write((int) random(45, 135));
    }

    void move() {
        servo.write((int) random(SERVO_MIN, SERVO_MAX));
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
    int position1;
    int position2;
    int position3;

public:
    Light(int pin, int stripLength) {
        length = stripLength;
        color = new uint32_t[length];
        tmpColor = new uint32_t[length];
        buffer = new int[length];
        neo = new Adafruit_NeoPixel(length, pin, NEO_GRB + NEO_KHZ800);

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

    void loop(LightState state) {
        switch (state) {
            case SPIN:
                doDecay();
                moveCenter();
                break;
            case FIRE:
                fireLoop();
                break;
            case RAINBOW:
                rainbowLoop(false);
                break;
            case RAINBOW_RED:
                rainbowLoop(true);
                break;
            default:
                turnOff();
                break;
        }

        apply();
    }

    void doDecay() {
        for (int i = 0; i < length; i++) {
            uint32_t current = color[i];
            int r = doDecay(unpackR(current));
            int g = doDecay(unpackG(current));
            int b = doDecay(unpackB(current));
            color[i] = Adafruit_NeoPixel::Color(r, g, b);
        }
    }

    static int doDecay(int value) {
        int out = value - (int) random(0, 120);
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

        // Lead
        color[position1] = Adafruit_NeoPixel::Color(200, 0, 0);
        color[position2] = Adafruit_NeoPixel::Color(200, 0, 0);
        color[position3] = Adafruit_NeoPixel::Color(200, 0, 0);
    }

    void fireLoop() {
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

    void rainbowLoop(bool redOnly) {
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
        }
        neo->show();
    }

    static int brightness(int color) {
        return (int) (color * MAX_BRIGHTNESS);
    }
};

class Oni {
private:
    Light *eye = new Light(PIN3, 12);
    Motor *eyeMotor = new Motor(PIN2_R);
    Light *belly = new Light(PIN5, 60);

    unsigned long nextEyeTime = 0;
    unsigned long nextBellyTime = 0;
    unsigned long nextEyeMoveTime = 0;
    unsigned long nextOniTime = 0;

    LightState eyeState = OFF;
    LightState bellyState = OFF;
    OniState oniState = INIT;

public:
    void init() {
        eye->init();
        eyeMotor->init();
        belly->init();
        nextOniTime = Timer::inXs(0);
    }

    void loop() {
        switch (oniState) {
            case INIT:
                if (Timer::isPassed(nextOniTime)) {
                    switch (eyeState) {
                        case OFF:
                            Serial.println("Testing SPIN");
                            eyeState = SPIN;
                            break;
                        case SPIN:
                            Serial.println("Testing FIRE");
                            eyeState = FIRE;
                            break;
                        case FIRE:
                            Serial.println("Testing RAINBOW");
                            eyeState = RAINBOW;
                            break;
                        case RAINBOW:
                            Serial.println("Testing RAINBOW_RED");
                            eyeState = RAINBOW_RED;
                            break;
                        case RAINBOW_RED:
                        default:
                            Serial.println("Testing finished");
                            oniState = IDLE;
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
                    switch (eyeState) {
                        case OFF:
                            eyeState = newEyeState();
                            nextEyeTime = Timer::inXs(12);
                            break;
                        default:
                            eyeState = OFF;
                            nextEyeTime = Timer::inXs(5);
                    }
                }
                if (Timer::isPassed(nextBellyTime)) {
                    switch (eyeState) {
                        case OFF:
                            bellyState = newBellyState();
                            nextBellyTime = Timer::inXs(20);
                            break;
                        default:
                            bellyState = OFF;
                            nextBellyTime = Timer::inXs(10);
                    }
                }
                break;
            case MOVE:
                if (Timer::isPassed(nextOniTime)) {
                    oniState = IDLE;
                    nextOniTime = Timer::inXs(30);
                }

                bellyState = FIRE;
                eyeState = SPIN;
                break;
            default:
                break;
        }

        eye->loop(eyeState);
        belly->loop(bellyState);

        if (Timer::isPassed(nextEyeMoveTime)) {
            eyeMotor->move();
            nextEyeMoveTime = Timer::inXs(2);
        }
    }

    static LightState newEyeState() {
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

    static LightState newBellyState() {
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

void setup() {
    Serial.begin(115200);
    randomSeed(analogRead(PIN0_R));
    oni.init();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void loop() {
    while (true) {
        // Delay for a period of time (in milliseconds).
        delay(100);
        oni.loop();
    }
}

#pragma clang diagnostic pop

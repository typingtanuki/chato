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
#define PIN9 9 // Motor
#define PIN10_R 10 // Mux shield
#define PIN11_R 11 // Mux shield
#define PIN12_R 12 // Mux shield
#define PIN13 13

#define MAX_BRIGHTNESS 0.3


#define KEYPAD 5 // Keypad analog

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

class Keypad {
private:
    unsigned short button = 0;
    unsigned short last = 0;
    bool cleared = false;

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

public:
    void init() {
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
        int keypress = analogRead(5);
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

        if (last != button) {
            Serial.print("Key: ");
            Serial.println(button);
            last = button;
            cleared = false;
        }
    }

    int currentKey() {
        if (cleared) {
            return 0;
        }
        cleared = true;
        return button;
    }
};


class Motor {
private:
    int pin;
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
        pos++;
        servo.write(pos);
        return pos >= SERVO_MAX;
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

            Serial.print("Motor moving:");
            Serial.print(pos);
            Serial.print("/");
            Serial.println(target);
        }
    }

    void randTarget() {
        target = (int) random(SERVO_MIN, SERVO_MAX);
        Serial.print("Motor target moved to:");
        Serial.println(target);
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
    Motor *eyeMotor = new Motor(PIN9);
    Light *belly = new Light(PIN5, 60);
    Keypad *keypad;

    unsigned long nextEyeTime = 0;
    unsigned long nextBellyTime = 0;
    unsigned long nextEyeMoveTime = 0;
    unsigned long nextOniTime = 0;

    LightState eyeState = OFF;
    LightState bellyState = OFF;
    OniState oniState = INIT;

public:
    void init(Keypad *keypad) {
        this->keypad = keypad;

        eye->init();
        eyeMotor->init();
        belly->init();
        nextOniTime = Timer::inXs(0);
    }

    void loop() {
        switch (oniState) {
            case INIT:
                if (eyeMotor->test()) {
                    // Testing
                } else if (Timer::isPassed(nextOniTime)) {
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
            eyeMotor->randTarget();
            nextEyeMoveTime = Timer::inXs(2);
        }
        eyeMotor->loop();
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
Keypad keypad;

void setup() {
    Serial.begin(115200);
    randomSeed(analogRead(PIN0_R));
    keypad.init();
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
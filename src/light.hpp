#include <Adafruit_NeoPixel.h>

#define MAX_BRIGHTNESS 0.3

enum LightState {
    OFF, SPIN, FIRE, RAINBOW, RAINBOW_RED
};

class Light {
private:
    Adafruit_NeoPixel *neo;

    uint32_t *color;
    uint32_t *tmpColor;

    uint32_t black = Adafruit_NeoPixel::Color(0, 0, 0);
    uint32_t red = Adafruit_NeoPixel::Color(200, 0, 0);

    unsigned long lastRefresh = 0;

    int *buffer;

    int length;
    int fullLength;
    int position1;
    int position2;
    int position3;

public:
    Light(int pin, int stripLength, int fullLength);

    ~Light();

    void init();

    void loop(LightState state, int simulationSpeed);

    void spinLoop(int steps);

    int boundary(int position) const;

    void fireLoop(int steps);

    void rainbowLoop(bool redOnly, int steps);

    static int rainbow(byte WheelPos);

    int remapRight(int index) const;

    int remapLeft(int index) const;

    void turnOff();

    void apply();

    static int unpackR(uint32_t color);

    static int unpackG(uint32_t color);

    static int unpackB(uint32_t color);

    static int brightness(int color);
};
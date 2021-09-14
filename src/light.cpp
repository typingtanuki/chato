#include "light.hpp"

Light::Light(int pin, int stripLength, int fullLength) {
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

Light::~Light() {
    delete[] color;
    delete[] tmpColor;
    delete[] buffer;
    delete neo;
}

void Light::init() {
    neo->begin();
    turnOff();
}

void Light::loop(LightState state, int simulationSpeed) {
    int steps = simulationSpeed;

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

void Light::spinLoop(int steps) {
    turnOff();

    position1 = boundary(position1 + steps);
    position2 = boundary(position2 + steps);
    position3 = boundary(position3 + steps);

    int trail = (length / 3) - 2;
    int colorStep = 200 / trail;

    uint32_t lead = red;

    // Lead
    color[position1] = lead;
    color[position2] = lead;
    color[position3] = lead;

    // Trail
    for (int i = 0; i < trail; i++) {
        uint32_t dot = Adafruit_NeoPixel::Color(200 - i * colorStep, 0, 0);
        color[boundary(position1 - i)] = dot;
        color[boundary(position2 - i)] = dot;
        color[boundary(position3 - i)] = dot;
    }
}

int Light::boundary(int position) const {
    while (position < 0) {
        position = position + length;
    }

    if (position > length) {
        return position % length;
    }

    return position;
}

void Light::fireLoop(int steps) {
    for (int j = 0; j < steps; j++) {
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

void Light::rainbowLoop(bool redOnly, int steps) {
    position1 += steps - 1;
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

int Light::rainbow(byte WheelPos) {
    if (WheelPos < 85) {
        return WheelPos * 3;
    }
    if (WheelPos < 170) {
        WheelPos -= 85;
        return 255 - WheelPos * 3;
    }

    return 0;
}

int Light::remapRight(int index) const {
    int val = length / 2 + index + 1;
    int over = val - length;
    if (over >= 0) {
        val = over;
    }
    return val;
}

int Light::remapLeft(int index) const {
    int val = length / 2 - index;
    if (val < 0) {
        val = length + val;
    }
    return val;
}

void Light::turnOff() {
    for (int i = 0; i < length; i++) {
        color[i] = black;
    }
}

void Light::apply() {
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

int Light::unpackR(uint32_t color) {
    return (int) ((color >> 16) & 0xFF);
}

int Light::unpackG(uint32_t color) {
    return (int) ((color >> 8) & 0xFF);
}

int Light::unpackB(uint32_t color) {
    return (int) (color & 0xFF);
}

int Light::brightness(int color) {
    return (int) (color * MAX_BRIGHTNESS);
}

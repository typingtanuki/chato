#pragma once

#define DIGITAL_PIN0_R 0 // Random seed
#define DIGITAL_PIN1 1
#define DIGITAL_PIN2_R 2 // Mux shield
#define DIGITAL_PIN3 3   // Eyes
#define DIGITAL_PIN4_R 4 // Mux shield
#define DIGITAL_PIN5 5   // Axe
#define DIGITAL_PIN6_R 6 // Mux shield
#define DIGITAL_PIN7_R 7 // Mux shield
#define DIGITAL_PIN8_R 8 // Mux shield
#define DIGITAL_PIN9 9 // Eyes Motor
#define DIGITAL_PIN10_R 10 // Mux shield
#define DIGITAL_PIN11_R 11 // Mux shield
#define DIGITAL_PIN12_R 12 // Mux shield
#define DIGITAL_PIN13 13 // Axe Motor

#define ANALOG_PIN0 0
#define ANALOG_PIN1 1
#define ANALOG_PIN2 2
#define ANALOG_PIN3 3
#define ANALOG_PIN4 4
#define ANALOG_PIN5 5   // Keypad button press

#define KEYPAD_STATE_COUNT 10 // Number of LEDs on keypad

#define KEYPAD_BUTTON_NONE 512
#define KEYPAD_BUTTON_1 876
#define KEYPAD_BUTTON_2 558
#define KEYPAD_BUTTON_3 145
#define KEYPAD_BUTTON_4 614
#define KEYPAD_BUTTON_5 767
#define KEYPAD_BUTTON_6 682
#define KEYPAD_BUTTON_7 254
#define KEYPAD_BUTTON_8 409
#define KEYPAD_BUTTON_9 465
#define KEYPAD_BUTTON_10 340

enum {
    KEYPAD_TENSION_P1 = (KEYPAD_BUTTON_1 + KEYPAD_BUTTON_5) / 2,
    KEYPAD_TENSION_P2 = (KEYPAD_BUTTON_5 + KEYPAD_BUTTON_6) / 2,
    KEYPAD_TENSION_P3 = (KEYPAD_BUTTON_6 + KEYPAD_BUTTON_4) / 2,
    KEYPAD_TENSION_P4 = (KEYPAD_BUTTON_4 + KEYPAD_BUTTON_2) / 2,
    KEYPAD_TENSION_P5 = (KEYPAD_BUTTON_2 + KEYPAD_BUTTON_NONE) / 2,
    KEYPAD_TENSION_P6 = (KEYPAD_BUTTON_NONE + KEYPAD_BUTTON_9) / 2,
    KEYPAD_TENSION_P7 = (KEYPAD_BUTTON_9 + KEYPAD_BUTTON_8) / 2,
    KEYPAD_TENSION_P8 = (KEYPAD_BUTTON_8 + KEYPAD_BUTTON_10) / 2,
    KEYPAD_TENSION_P9 = (KEYPAD_BUTTON_10 + KEYPAD_BUTTON_7) / 2,
    KEYPAD_TENSION_P10 = (KEYPAD_BUTTON_7 + KEYPAD_BUTTON_3) / 2,
};
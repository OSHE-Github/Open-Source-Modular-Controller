/**
 * Simple HID gamepad, 32 buttons + 2x 3 axis + 8 position hat
 * author: chegewara
 */
#define PIN_Rx 1
#define PIN_Ry 2
#define Lx 4
#define Ly 5

#define R1 17
#define L1 18
#define R2 3
#define L2 6
#define R3 35
#define L3 36

#define A 7
#define B 8
#define X 9
#define Y 10

#define up 11
#define down 12
#define left 13
#define right 14

#define START 15
#define SELECT 16
#define HOME 21

// On-board RGB LED (ESP32-S3 Dev Module usually GPIO48 or board specific)
#define LED_PIN 38
#define LED_COUNT 1

#include "hidgamepad.h"
#include <Adafruit_NeoPixel.h>

#if CFG_TUD_HID
HIDgamepad gamepad;
Adafruit_NeoPixel playerLED(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int playerNumber = 1;

void setup()
{
    pinMode(PIN_Ry, INPUT);
    pinMode(PIN_Rx, INPUT);
    pinMode(Ly, INPUT);
    pinMode(Lx, INPUT);

    pinMode(R1,  INPUT_PULLUP); 
    pinMode(L1,  INPUT_PULLUP); 
    pinMode(R2,  INPUT_PULLUP); 
    pinMode(L2,  INPUT_PULLUP);   
    pinMode(R3,  INPUT_PULLUP); 
    pinMode(L3,  INPUT_PULLUP);

    pinMode(A,  INPUT_PULLUP);
    pinMode(B,  INPUT_PULLUP);
    pinMode(X,  INPUT_PULLUP); 
    pinMode(Y,  INPUT_PULLUP);

    pinMode(up,  INPUT_PULLUP); 
    pinMode(down,  INPUT_PULLUP); 
    pinMode(left,  INPUT_PULLUP); 
    pinMode(right,  INPUT_PULLUP);
 
    pinMode(START,  INPUT_PULLUP); 
    pinMode(SELECT,  INPUT_PULLUP); 
    pinMode(HOME,  INPUT_PULLUP); 

    Serial.begin(115200);
    gamepad.begin();
    playerLED.begin();
    playerLED.show(); // Initialize off
}


void setPlayerLED(int player) {
    uint32_t color = 0;
    switch (player) {
        case 1: color = playerLED.Color(255, 0, 0); break;   // Red
        case 2: color = playerLED.Color(0, 255, 0); break;   // Green
        case 3: color = playerLED.Color(0, 0, 255); break;   // Blue
        case 4: color = playerLED.Color(255, 255, 0); break; // Yellow
        default: color = playerLED.Color(255, 255, 255); break; // White
    }
    playerLED.setPixelColor(0, color);
    playerLED.show();
}


void loop()
{

    // Example: cycle player number on START button press
    if (!digitalRead(START)) {
        delay(200); // debounce
        playerNumber++;
        if (playerNumber > 4) playerNumber = 1;
        setPlayerLED(playerNumber);
    }

    // -------------------
    // Read analog joysticks
    // -------------------
    // Map ADC values (0–4095 on ESP32) to HID range (-127..127)
    int lxVal = map(analogRead(Lx), 0, 4095, -127, 127);
    int lyVal = map(analogRead(Ly), 0, 4095, -127, 127);
    int rxVal = map(analogRead(PIN_Rx), 0, 4095, -127, 127);
    int ryVal = map(analogRead(PIN_Ry), 0, 4095, -127, 127);

    // Triggers (if you want them analog, otherwise treat as buttons)
    
    int l2Val = !digitalRead(L2) ? 127 : 0;
    int r2Val = !digitalRead(R2) ? 127 : 0;

    // -------------------
    // Read buttons
    // -------------------
    uint32_t buttons = 0;

    if (!digitalRead(A))     buttons |= (1 << 0);
    if (!digitalRead(B))     buttons |= (1 << 1);
    if (!digitalRead(X))     buttons |= (1 << 2);
    if (!digitalRead(Y))     buttons |= (1 << 3);

    if (!digitalRead(R1))    buttons |= (1 << 4);
    if (!digitalRead(L1))    buttons |= (1 << 5);
    if (!digitalRead(R3))    buttons |= (1 << 6);
    if (!digitalRead(L3))    buttons |= (1 << 7);

    if (!digitalRead(START)) buttons |= (1 << 8);
    if (!digitalRead(SELECT))buttons |= (1 << 9);
    if (!digitalRead(HOME))  buttons |= (1 << 10);

    // -------------------
    // Read D-pad → hat
    // -------------------
    uint8_t hat = 0; // default center
    if (!digitalRead(up) && !digitalRead(right))  hat = 2; // up-right
    else if (!digitalRead(up) && !digitalRead(left))  hat = 8; // up-left
    else if (!digitalRead(down) && !digitalRead(right)) hat = 4; // down-right
    else if (!digitalRead(down) && !digitalRead(left))  hat = 6; // down-left
    else if (!digitalRead(up))    hat = 1;
    else if (!digitalRead(right)) hat = 3;
    else if (!digitalRead(down))  hat = 5;
    else if (!digitalRead(left))  hat = 7;

    // -------------------
    // Send state to HID
    // -------------------
    gamepad.sendAll(
        buttons,   // 32 buttons bitmask
        lxVal,     // LX
        lyVal,     // LY
        l2Val,     // L2 (treated as analog axis)
        rxVal,     // RX
        ryVal,     // RY
        r2Val,     // R2 (treated as analog axis)
        hat        // D-pad hat
    );

    delay(10); // Small poll delay
}

#endif

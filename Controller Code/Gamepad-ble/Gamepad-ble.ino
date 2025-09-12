/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 *
 * At the moment we are using the default settings, but they can be canged using a BleGamepadConfig instance as parameter for the begin function.
 *
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_16
 * (16 buttons by default. Library can be configured to use up to 128)
 *
 * Possible DPAD/HAT switch position values are:
 * DPAD_CENTERED, DPAD_UP, DPAD_UP_RIGHT, DPAD_RIGHT, DPAD_DOWN_RIGHT, DPAD_DOWN, DPAD_DOWN_LEFT, DPAD_LEFT, DPAD_UP_LEFT
 * (or HAT_CENTERED, HAT_UP etc)
 *
 * bleGamepad.setAxes sets all axes at once. There are a few:
 * (x axis, y axis, z axis, rx axis, ry axis, rz axis, slider 1, slider 2)
 *
 * Alternatively, bleGamepad.setHIDAxes sets all axes at once. in the order of:
 * (x axis, y axis, z axis, rZ axis, rX axis, rY axis, slider 1, slider 2)  <- order HID report is actually given in
 *
 * Library can also be configured to support up to 5 simulation controls
 * (rudder, throttle, accelerator, brake, steering), but they are not enabled by default.
 *
 * Library can also be configured to support different function buttons
 * (start, select, menu, home, back, volume increase, volume decrease, volume mute)
 * start and select are enabled by default
 */
//joystick anolog pins
#define PIN_Rx 1
#define PIN_Ry 2
#define Lx 4
#define Ly 5

//sholders,trigers, and joystick buttons
#define R1 17
#define L1 18
#define R2 3
#define L2 6
#define R3 15
#define L3 16

//action buttons
#define A 7
#define B 8
#define X 9
#define Y 10
//d-pad
#define hup 11
#define hdown 12
#define hleft 13
#define hright 14
//special buttions
#define START 35
#define SELECT 36
#define HOME 21

#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad bleGamepad;


void setup() {
  pinMode(PIN_Ry, INPUT);
  pinMode(PIN_Rx, INPUT);
  pinMode(Ly, INPUT);
  pinMode(Lx, INPUT);

  pinMode(R1, INPUT_PULLUP);
  pinMode(L1, INPUT_PULLUP);
  pinMode(R2, INPUT_PULLUP);
  pinMode(L2, INPUT_PULLUP);
  pinMode(R3, INPUT_PULLUP);
  pinMode(L3, INPUT_PULLUP);

  pinMode(A, INPUT_PULLUP);
  pinMode(B, INPUT_PULLUP);
  pinMode(X, INPUT_PULLUP);
  pinMode(Y, INPUT_PULLUP);

  pinMode(hup, INPUT_PULLUP);
  pinMode(hdown, INPUT_PULLUP);
  pinMode(hleft, INPUT_PULLUP);
  pinMode(hright, INPUT_PULLUP);

  pinMode(START, INPUT_PULLUP);
  pinMode(SELECT, INPUT_PULLUP);
  pinMode(HOME, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleGamepad.begin();
  // The default bleGamepad.begin() above enables 16 buttons, all axes, one hat, and no simulation controls or special buttons

}

void loop() {
  if (bleGamepad.isConnected()) {
    // --- Joysticks ---
    int lxVal = analogRead(Lx);
    int lyVal = analogRead(Ly);
    int rxVal = analogRead(PIN_Rx);
    int ryVal = analogRead(PIN_Ry);

    // Map 0–4095 ADC to -32767–32767 (centered at ~2048)
    int lx = map(lxVal, 0, 4095, -127, 127);
    int ly = map(lyVal, 0, 4095, -127, 127);  // invert Y
    int rx = map(rxVal, 0, 4095, -127, 127);
    int ry = map(ryVal, 0, 4095, -127, 127);

    // Update axes (X, Y, Z, RX, RY, RZ, slider1, slider2)
    bleGamepad.setAxes(lx, ly, rx, ry, 0, 0, 0, 0);

    // --- Buttons ---
    uint32_t buttons = 0;
    // Buttons 1-10: A, B, X, Y, R1, L1, R2, L2, R3, L3
    if (!digitalRead(A)) {
      bleGamepad.press(BUTTON_1);
      buttons |= (1 << 0);
    } else bleGamepad.release(BUTTON_1);
    if (!digitalRead(B)) {
      bleGamepad.press(BUTTON_2);
      buttons |= (1 << 1);
    } else bleGamepad.release(BUTTON_2);
    if (!digitalRead(X)) {
      bleGamepad.press(BUTTON_3);
      buttons |= (1 << 2);
    } else bleGamepad.release(BUTTON_3);
    if (!digitalRead(Y)) {
      bleGamepad.press(BUTTON_4);
      buttons |= (1 << 3);
    } else bleGamepad.release(BUTTON_4);

    if (!digitalRead(R1)) {
      bleGamepad.press(BUTTON_5);
      buttons |= (1 << 4);
    } else bleGamepad.release(BUTTON_5);
    if (!digitalRead(L1)) {
      bleGamepad.press(BUTTON_6);
      buttons |= (1 << 5);
    } else bleGamepad.release(BUTTON_6);
    if (!digitalRead(R2)) {
      bleGamepad.press(BUTTON_7);
      buttons |= (1 << 6);
    } else bleGamepad.release(BUTTON_7);
    if (!digitalRead(L2)) {
      bleGamepad.press(BUTTON_8);
      buttons |= (1 << 7);
    } else bleGamepad.release(BUTTON_8);

    if (!digitalRead(R3)) {
      bleGamepad.press(BUTTON_9);
      buttons |= (1 << 8);
    } else bleGamepad.release(BUTTON_9);
    if (!digitalRead(L3)) {
      bleGamepad.press(BUTTON_10);
      buttons |= (1 << 9);
    } else bleGamepad.release(BUTTON_10);

    // --- Special buttons ---
    if (!digitalRead(START)) {
      bleGamepad.pressStart();
      buttons |= (1 << 10);
    } else bleGamepad.releaseStart();
    if (!digitalRead(SELECT)) {
      bleGamepad.pressSelect();
      buttons |= (1 << 11);
    } else bleGamepad.releaseSelect();

    if (!digitalRead(HOME)) {
      bleGamepad.pressHome();
      buttons |= (1 << 12);
    } else
      bleGamepad.releaseHome();

    // --- Hat switch (DPAD) ---
    String hat = "Center";  // default center
    if (!digitalRead(hup) && !digitalRead(hright)) {
      bleGamepad.setHat1(HAT_UP_RIGHT);
      hat = "Up Right";
    } else if (!digitalRead(hup) && !digitalRead(hleft)) {
      bleGamepad.setHat1(HAT_UP_LEFT);
      hat = "Up Left";
    } else if (!digitalRead(hdown) && !digitalRead(hright)) {
      bleGamepad.setHat1(HAT_DOWN_RIGHT);
      hat = "Down Right";
    } else if (!digitalRead(hdown) && !digitalRead(hleft)) {
      bleGamepad.setHat1(HAT_DOWN_LEFT);
      hat = "Down Left";
    } else if (!digitalRead(hup)) {
      bleGamepad.setHat1(HAT_UP);
      hat = "Up";
    } else if (!digitalRead(hdown)) {
      bleGamepad.setHat1(HAT_DOWN);
      hat = "Down";
    } else if (!digitalRead(hleft)) {
      bleGamepad.setHat1(HAT_LEFT);
      hat = "Left";
    } else if (!digitalRead(hright)) {
      bleGamepad.setHat1(HAT_RIGHT);
      hat = "Right";
    } else bleGamepad.setHat1(HAT_CENTERED);

    delay(10);  // small debounce/update delay
    Serial.printf("Joystick position /n Left y: %f x: %f /n Right y: %f x: %f /n ", ly, lx, ry, rx);
    Serial.printf("Button State A B X Y R1 L1 R2 L2 R3 L3 /n %f /n", buttons);
    Serial.printf("Hat position: %f", hat);
  }
}

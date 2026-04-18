const int DEADZONE = 9;    // Set deadzone
const int polldelay = 20;  // Set 100Hz polling rate
const bool test = false;

// Set module identifiers for readability
#define MODULE_RECEIVER 0x01
#define MODULE_BUTTON 0x04
#define MODULE_MASTER 0x05
#define MODULE_JOYSTICK 0x09
// Set button module identifiers for readability
#define BUTTON_DPAD 0x30
#define BUTTON_ABXY 0x20
#define BUTTON_TRIGGER 0x10

#include "USB.h"
#include "USBHIDGamepad.h"
#include "Wire.h"
#include "Adafruit_ADS1X15.h"

#if CFG_TUD_HID
USBHIDGamepad gamepad;

// FIXING ARRAYS

uint8_t deviceAddresses[7] = { 0 };
uint8_t deviceLocation[7] = { 0 };

// Make arrays to store information abnout modules
bool moduleIn[7] = { false };
uint8_t moduleType[7];
uint8_t moduleEEPROMaddr[7] = { 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56 };
uint8_t moduleADCaddr[7] = { 0x48, 0x49, 0x4A, 0x4B, 0x00, 0x00, 0x00 };
bool moduleUsesADC[7] = { false };
bool adsInit[7] = { false };
uint16_t adcvalue[32];
int adsSize = sizeof(adsInit) / sizeof(bool);

static uint8_t modulesInUse = 0;
// uint32_t i2cScanCount = 0;

uint8_t moduleIOEaddr[7] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x2f };
uint8_t moduleButtonType[7] = { 0 };

// Create ISR flag and assign interrupt pin
volatile bool moduleChangePending = false;
int INT_PIN = 11;

// Create 4 objects for each potential adc
Adafruit_ADS1115 ads0;
Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;
Adafruit_ADS1115 ads3;
Adafruit_ADS1115 ads4;

// Declare function
// void ARDUINO_ISR_ATTR moduleSwap();
// void scanModules();

void setup() {
  Serial.begin(115200);
  // Initialize BUS1 I2C communication on pins 21 (SDA) and 22 (SCL)
  Wire.begin();
  Wire1.begin(7, 8);

  // Sets clock frequency to 100kHz
  Wire.setClock(100000);
  Wire1.setClock(100000);

  Wire.setTimeout(10);

  // Theres only one INT pin, scan every module for detection
  // Attach ISR to the pin which
  // pinMode(INT_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(INT_PIN), moduleSwap, CHANGE);

  // Set LED pins for output


  delay(10);
  gamepad.begin();
}

void loop() {
  fastScanModules();

  // I2C Addressing: Top left is default, top right is +1,
  // bottom left is +2, bottom right is +3. Addresses increment in Z shape

  // I2C IO Expanders have addresses 0x20-0x27
  // They store the data of the 8 pins in a single byte
  // Writing 1 to a bit enables the pin for input

  int LxVal = 0;
  int LyVal = 0;
  int RxVal = 0;
  int RyVal = 0;
  int L3Val = 0;
  int R3Val = 0;
  uint16_t Lx = 0;
  uint16_t Ly = 0;
  uint16_t Rx = 0;
  uint16_t Ry = 0;
  uint16_t L3 = 0;
  uint16_t R3 = 0;
  int RTrigger = 0;
  int LTrigger = 0;
  uint8_t hat = 0;
  uint32_t buttons = 0;
  bool leftJoystickPresent = false;
  bool rightJoystickPresent = false;

  // If ISR was tripped this will run
  // if (moduleChangePending) {
  //   moduleChangePending = false;
  //   scanModules();
  // }


  // Create inside loop for obtaining data from each module
  for (int i = 0; i < 7; i++) {
    uint8_t addr = moduleEEPROMaddr[i];
    // If module slot is empty go to next slot
    if (moduleIn[i] != true) {
      continue;
    }

    // If the module is a joystick
    if (moduleType[i] == MODULE_JOYSTICK) {

      // guard
      if (!moduleUsesADC[i]) {
        continue;
      }

      uint8_t addr = moduleADCaddr[i];

      // recheck device before reading
      Wire.beginTransmission(addr);
      if (Wire.endTransmission() != 0) {
        moduleIn[i] = false;
        moduleType[i] = 0;
        moduleUsesADC[i] = false;
        continue;
      }

      // Find the module index
      int index = addr - 0x48;

      switch (index) {
        case 0:
          break;
        case 1:
          // If the index is 0, then the joystick is on right side
          // read adc values into Rx and Ry for proper conversion
          // Subtract R values from 65535 to account for upside orientation
          leftJoystickPresent = true;
          Ly = (ads1.readADC_SingleEnded(0));
          Lx = (ads1.readADC_SingleEnded(1));
          L3 = (ads1.readADC_SingleEnded(2));
          break;
        case 2:
          // If the index is even, then joystick is on left side
          // read adc values into Lx and Ly for proper conversion
          rightJoystickPresent = true;
          Ry = ads2.readADC_SingleEnded(0);
          Rx = ads2.readADC_SingleEnded(1);
          R3 = ads2.readADC_SingleEnded(2);
          break;
        case 3:
          // If the index is 0, then the joystick is on right side
          // read adc values into Rx and Ry for proper conversion
          // Subtract R values from 65535 to account for upside orientation
          leftJoystickPresent = true;
          Ly = (ads3.readADC_SingleEnded(0));
          Lx = (ads3.readADC_SingleEnded(1));
          L3 = (ads3.readADC_SingleEnded(2));
          break;
        case 4:
          // If the index is even, then joystick is on left side
          // read adc values into Lx and Ly for proper conversion
          rightJoystickPresent = true;
          Ry = ads4.readADC_SingleEnded(0);
          Rx = ads4.readADC_SingleEnded(1);
          R3 = ads4.readADC_SingleEnded(2);
          break;
        default:
          continue;
      }

      // Map each read adc variable to appropriate controller variable
      // and value for proper function, create deadzone to avoid stick drift
      if (leftJoystickPresent) {
        // x-axis for left joystick
        LxVal = map(Lx, 0, 17000, 127, -127);
        if (abs(LxVal) < DEADZONE) {
          LxVal = 0;
        }
        if (LxVal < -127) LxVal = -127;
        // y-axis for left joystick
        LyVal = map(Ly, 0, 17000, -127, 127);
        if (abs(LyVal) < DEADZONE) {
          LyVal = 0;
        }
        if (LyVal > 127) LyVal = 127;

        // Add L3 to the bitmask, maybe not here?
        if ((L3 > 60000) || (L3 < 1000)) buttons |= (1 << 10);
      }

      if (rightJoystickPresent) {
        // x-axis for right joystick
        RxVal = map(Rx, 0, 17000, -127, 127);
        if (abs(RxVal) < DEADZONE) {
          RxVal = 0;
        }
        if (RxVal > 127) RxVal = 127;
        // y-axis for right joystick
        RyVal = map(Ry, 0, 17000, 127, -127);
        if (abs(RyVal) < DEADZONE) {
          RyVal = 0;
        }
        if (RyVal < -127) RyVal = -127;

        // Add R3 to the bitmask, maybe not here?
        if ((R3 > 60000) || (R3 < 1000)) buttons |= (1 << 11);
      }
      // Go to next module and read

      continue;
    }

    // If module is buttons
    if (moduleType[i] == MODULE_BUTTON) {
      // Request all byte data from buttons
      Wire.beginTransmission(moduleIOEaddr[i]);
      uint8_t retval = Wire.endTransmission();

      // Serial.printf("RETURN TEST FROM BUTTON: %d\n", retval);

      Wire.requestFrom(moduleIOEaddr[i], 1);
      uint8_t data = Wire.read();

      // Serial.printf("DATA TEST FROM BUTTON: %d\n", data);


      // for (int k = 0; k < 6; k++){
      //   Serial.printf("button type 0x%X\n", moduleButtonType[k]);
      // }

      switch (moduleButtonType[i]) {
        // In this case, the button module is the dpad
        case BUTTON_DPAD:
          {

            // All pins are high, LOW = button pressed
            // Following code assumes bit 0, 2, 4, 6 are up down left right respectively
            const uint8_t upByte = 0xFD;     // 11111101
            const uint8_t downByte = 0xF7;   // 11110111
            const uint8_t leftByte = 0xFB;   // 11111011
            const uint8_t rightByte = 0xFE;  // 11111110

            // Switch statement to determine direction
            switch (data) {
              // Up right diagonal
              case (upByte & rightByte):
                buttons |= (1 << 12);
                buttons |= (1 << 15);
                hat = 2;
                break;
              // Up left diagonal
              case (upByte & leftByte):
                buttons |= (1 << 12);
                buttons |= (1 << 14);
                hat = 8;
                break;
              // Down right diagonal
              case (downByte & rightByte):
                buttons |= (1 << 13);
                buttons |= (1 << 15);
                hat = 4;
                break;
              // Down left diagonal
              case (downByte & leftByte):
                buttons |= (1 << 13);
                buttons |= (1 << 14);
                hat = 6;
                break;
              // Only up
              case (upByte):
                buttons |= (1 << 12);
                hat = 1;
                break;
              // Only down
              case (downByte):
                buttons |= (1 << 13);
                hat = 5;
                break;
              // Only right
              case (rightByte):
                buttons |= (1 << 15);
                hat = 3;
                break;
              // Only left
              case (leftByte):
                buttons |= (1 << 14);
                hat = 7;
                break;
              // Nothing
              default: hat = 0;
            }
            break;
          }

          // In this case, button module is ABXY
        case BUTTON_ABXY:
          {
            // Serial.println("jew");
            // All pins are high, LOW = button pressed
            // Following code assumes bit 0, 2, 4, 6 are up (Y) down (A) left (X) right (B) respectively
            // A: 11111011 B: 10111111 X: 11101111 Y: 11111110

            // Use if statements to assign data to button bitmask for multi inputs
            // If the data byte has 0 at bit 2, A is pressed
            if (!(data & (1 << 1))) buttons |= (1 << 0);  // A
            if (!(data & (1 << 2))) buttons |= (1 << 1);  // B
            if (!(data & (1 << 3))) buttons |= (1 << 3);  // Y
            if (!(data & (1 << 0))) buttons |= (1 << 2);  // X

            break;
          }

          // In this case, button module is trigger/bumper
        case BUTTON_TRIGGER:
          {
            // 0x24 address means left side
            if (moduleIOEaddr[i] == 0x20) {
              // Pin 0 is bumper, Pin 7 is trigger
              // Bit 5 in bitmask is the Left bumper
              // Left trigger has its own variable value
              if (!(data & (1 << 1))) buttons |= (1 << 4);
              if (!(data & (1 << 0))) {
                LTrigger = 127;
                buttons |= (1 << 6);
              }

            } else {
              // If not on left, its on right, Pins swap because upside down
              if (!(data & (1 << 0))) buttons |= (1 << 5);
              if (!(data & (1 << 1))) {
                RTrigger = 127;
                buttons |= (1 << 7);
              }
            }
            break;
          }
      }
      continue;
    }

    if (moduleType[i] == MODULE_MASTER) {
      Wire.requestFrom(moduleIOEaddr[i], 1);
      uint8_t data = Wire.read();
      uint8_t version = 1;
      if (version == 1) {
        // Following code assumings start, select, home button are bits 0 3 7
        if (!(data & (1 << 0))) buttons |= (1 << 16);
        if (!(data & (1 << 1))) buttons |= (1 << 8);
        if (!(data & (1 << 2))) buttons |= (1 << 9);
      } else if (version == 2) {
        if (!(data & (1 << 1))) buttons |= (1 << 9);   // A
        if (!(data & (1 << 2))) buttons |= (1 << 17);  // B
        if (!(data & (1 << 3))) buttons |= (1 << 8);   // Y
        if (!(data & (1 << 0))) buttons |= (1 << 16);  // X
      }
    }
  }

  // conditional to see testing data or run program
  if (!test) {
    // -------------------
    // Send state to HID
    // -------------------
    gamepad.send(
      LxVal,     // LX
      LyVal,     // LY
      RxVal,     // RX
      LTrigger,  // L2 (treated as analog axis)
      RyVal,     // RY
      RTrigger,  // R2 (treated as analog axis)
      hat,       // D-pad hat
      buttons    // 32 buttons bitmask
    );

    static uint32_t testcount = 0;
    Serial.println("Joystick position");
    Serial.printf("Left y: %d x: %d trigger: %d \nRight y: %d x: %d trigger: %d \n", LyVal, LxVal, LTrigger, RyVal, RxVal, RTrigger);
    Serial.printf("Button State A B X Y R1 L1 R2 L2 R3 L3 \n");
    Serial.println(buttons, BIN);
    Serial.printf("Hat position: %d \n", hat);
    Serial.printf("Count: %d \n", testcount++);

  } else {

    for (int i = 0; i < 5; i++) {
      Serial.printf("Module %d slot %d: 0x%X\n", moduleIn[i], i, moduleType[i]);
    }

    Serial.println("Scanning Wire bus...");
    int found = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
      Wire.beginTransmission(addr);
      uint8_t err = Wire.endTransmission();
      if (err == 0) {
        Serial.printf("Found device at 0x%02X\n", addr);
        found++;
      }
    }
    if (found == 0) {
      Serial.println("No devices found - check pins or swap SDA/SCL");
    } else {
      Serial.printf("Scan done, %d device(s) found\n", found);
    }
    if (found > 5) {
      static uint32_t testcount = 0;
      Serial.println("Joystick position");
      Serial.printf("Left y: %d x: %d trigger: %d \nRight y: %d x: %d trigger: %d \n", LyVal, LxVal, LTrigger, RyVal, RxVal, RTrigger);
      Serial.printf("Button State A B X Y R1 L1 R2 L2 R3 L3 \n");
      Serial.println(buttons, BIN);
      Serial.printf("Hat position: %d \n", hat);
      Serial.printf("Count: %d \n", testcount++);
    }
    delay(1000);
  }

  delay(polldelay);  // Small poll delay
}
/**
void i2cScan() {
  modulesInUse = 0;

  memset(deviceLocation, 0, sizeof(deviceLocation));
  memset(deviceAddresses, 0, sizeof(deviceAddresses));

  uint8_t address, error;
  int devices = 0;
  int deviceArrayCount = 0;
  for(address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      // Serial.print("I2C device found at address 0x");
      devices++;
      if (address<16) {
        // Serial.print("0");
        // Serial.print(address,HEX);
        // Serial.println("  !");
      } else {
        // Serial.print(address, HEX);
        // Serial.println("  !");
      }

      if ( (address > 0x57) || (address < 0x50) ) {
        deviceAddresses[deviceArrayCount++] = address;
        modulesInUse++;
        // Serial.println("Device added to array");
      }
    }

  }


  if (devices == 0) {
    // Serial.println("No I2C devices found\n");
  } else {
    // Serial.println("done\n");
    for (int l = 0; l < deviceArrayCount; l++) {
      uint8_t addr = deviceAddresses[l];
      if ( addr < 0x48) {
        // If lower than 0x48, IOExpander, -0x20 to get slot
        deviceLocation[addr - 0x20] = addr;
      } else {
        deviceLocation[addr - 0x48] = addr;
      }
    }

    for (int l = 0; l < 7; l++) {
      Serial.printf("Slot %d: 0x%X\n", l, deviceLocation[l]);
    }
  }

}

Define interrupt function for automatic module detection
void ARDUINO_ISR_ATTR moduleSwap() {
  // When interrupt is tripped, set flag
  moduleChangePending = true;
  Serial.println("Change detected!");
}

Function that scans modules, not using anymore
void scanModules() {
  // Scan devices on i2c bus
  i2cScan();

  for (int i = 0; i < 8; i++) {
    // If on the extra iteration of loop, do lights and then break
    if (i == 7) {
      // Check status of 4 face modules
      for (int j = 0; j < 4; j++) {
        if (moduleIn[j] == false) {
          // LED pins are 9-12, turn the lights on if module is missing
          digitalWrite(j+9, 1);
        } else {
          // Turn the lights off if module is there
          digitalWrite(j+9, 0);
        }
      }
      Serial.println("End of scan!");
      break;
    }


    if (deviceLocation[i] == 0) {
      // If 0 at that slot index, module doesnt exist
      moduleIn[i] = false;
      moduleType[i] = 0;
      moduleUsesADC[i] = false;
      continue;
    }

    // If this is reached, module exists. Read and store data
    moduleIn[i] = true;

    // Calculate possible joystick addresses
    uint8_t addr = deviceLocation[i];

    if ((addr >= 0x48) && (addr <= 0x4B)) {
      Serial.printf("Joystick scanned in slot %d\n", i);
      moduleType[i] = MODULE_JOYSTICK;
      moduleADCaddr[i] = addr;
      // Capped at 4 ADCs, calculate index so not to break data array
      // Index is 0, 1, 2, 3 based on address
      uint8_t index = addr - 0x48;

      // If ADC at this address is not initialized, initialize
      if (!adsInit[index]) {
        // Initialize the ads to read from
        switch (index) {
          case 0: ads0.begin(0x48);
          break;
          case 1: ads1.begin(0x49);
          break;
          case 2: ads2.begin(0x4A);
          break;
          case 3: ads3.begin(0x4B);
          break;
        }
        // Set ads at this index as initialized so we don't reinitialize later
        adsInit[index] = true;
      }
      // Set that an ADC has been initialized in this location so we can read from it
      moduleUsesADC[i] = true;
      continue;
    } else {
      // Module isn't a joystick so there is no ADC at this location, actually turning ADS
      // off is too much work so use this value to disable it's use
      moduleUsesADC[i] = false;
    }

    // If module is a set of buttons, setup to be read properly
    if ((addr >= 0x20) && (addr <= 0x25)  ) {
      Serial.printf("Button scanned in slot %d\n", i);
      uint8_t addrOffset = addr - 0x20;
      uint8_t eepromAddr = 0x50 + addrOffset;

      moduleType[i] = MODULE_BUTTON;
      moduleIOEaddr[i] = addr;
      uint8_t buttonClarifier = 0;

      if ((addrOffset == 0) || (addrOffset == 2) ) {
        // If module on right side of controller it is abxy
        buttonClarifier = BUTTON_ABXY;
      } else if ((addrOffset == 3) || (addrOffset == 4)) {
        // DPAD on the left side
        buttonClarifier = BUTTON_DPAD;
      } else {
        // Triggers on the back
        buttonClarifier = BUTTON_TRIGGER;
      }

      // Wire.beginTransmission(eepromAddr);
      // Wire.write(0x00);
      // uint8_t errorT = Wire.endTransmission();

      // Serial.printf("endTransmission return: %d\n", errorT);

      // Wire.requestFrom(eepromAddr, 2);
      // uint8_t moduleIndicator = Wire.read();
      // uint8_t buttonClarifier = Wire.read();
      // Serial.printf("Module Type 0x%X\n", buttonClarifier);


      // Wire.read() another line from the eeprom to determine the type
      // of button module that it is, write 1s to all pins on IOE to make them readable

      switch (buttonClarifier) {
        // Trigger identifer is 0x10
        case BUTTON_TRIGGER:
          moduleButtonType[i] = BUTTON_TRIGGER;
          Wire.beginTransmission(addr);
          Wire.write(0xFF);
          Wire.endTransmission();
          break;

        // ABXY identifier is 0x20
        case BUTTON_ABXY:
          moduleButtonType[i] = BUTTON_ABXY;
          Wire.beginTransmission(addr);
          Wire.write(0xFF);
          Wire.endTransmission();
          break;

        // DPAD identifier is 0x30
        case BUTTON_DPAD:
          moduleButtonType[i] = BUTTON_DPAD;
          Wire.beginTransmission(addr);
          Wire.write(0xFF);
          Wire.endTransmission();
          break;
      }
      continue;
    }

    // This is for the center module which has LEDs and home/start/select
    if (deviceLocation[i] == 0x26) {
      uint8_t buttonAddr = deviceLocation[i];

      // Write 1s to all IOExpander bits for reading
      Wire.beginTransmission(buttonAddr);
      Wire.write(0xFF);
      Wire.endTransmission();

    }
  }
}
*/
/*
function to scan and detect modules
*/
void fastScanModules() {
  // Reset module state every loop
  for (int i = 0; i < 7; i++) {
    moduleIn[i] = false;
    moduleType[i] = 0;
    moduleUsesADC[i] = false;
  }

  // Scan IO Expanders (buttons)
  for (uint8_t addr = 0x20; addr <= 0x26; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      int slot = addr - 0x20;

      moduleIn[slot] = true;
      moduleType[slot] = MODULE_BUTTON;
      moduleIOEaddr[slot] = addr;

      // Set button type
      if ((slot == 4) || (slot == 2)) {
        moduleButtonType[slot] = BUTTON_ABXY;
      } else if ((slot == 3) || (slot == 1)) {
        moduleButtonType[slot] = BUTTON_DPAD;
      } else if (slot == 6) {
        moduleType[slot] = MODULE_MASTER;
      } else if ((slot == 5) || (slot == 0)) {
        moduleButtonType[slot] = BUTTON_TRIGGER;
      }

      // Enable inputs
      Wire.beginTransmission(addr);
      Wire.write(0xFF);
      Wire.endTransmission();
    }
  }

  // Scan ADCs (joysticks)
  for (uint8_t addr = 0x48; addr <= 0x4C; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {

      int slot = addr - 0x48;

      moduleIn[slot] = true;
      moduleType[slot] = MODULE_JOYSTICK;
      moduleADCaddr[slot] = addr;
      moduleUsesADC[slot] = true;

      if (!adsInit[slot]) {
        switch (slot) {
          case 0:
            break;
          case 1:
            ads1.begin(addr);
            break;
          case 2:
            ads2.begin(addr);
            break;
          case 3:
            ads3.begin(addr);
            break;
          case 4:
            ads4.begin(addr);
            break;
        }
        adsInit[slot] = true;
      }

    } else {
      // If removed, reset init
      int slot = addr - 0x48;
      adsInit[slot] = false;
    }
  }


  // Code for LEDs, turns them on but wont trn them off
  if (moduleIn[6]) {

    uint8_t ledByte = 0x07;  // inputs stay HIGH

    // Set HIGH only when module is present (LED OFF)
    if (moduleIn[2]) ledByte |= (1 << 4);
    if (moduleIn[4]) ledByte |= (1 << 5);
    if (moduleIn[3]) ledByte |= (1 << 6);
    if (moduleIn[0]) ledByte |= (1 << 7);

    Wire.beginTransmission(moduleIOEaddr[6]);
    Wire.write(ledByte);
    Wire.endTransmission();
  }
}

#endif

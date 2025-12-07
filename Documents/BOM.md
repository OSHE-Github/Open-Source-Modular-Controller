# Bill of Materials (BOM)

This Bill of Materials summarizes all components required to assemble the Open Source Modular Controller.

---

## MCU Carrier Board

| Part Name                | Description                      | Qty | Total Cost | Link            |
| ------------------------ | -------------------------------- | --- | ---------- | --------------- |
| ESP32-S3-WROOM-1U-N4     | ESP32-S3 module                  | 1   | $5.06      | `[placeholder]` |
| 1N5819WS                 | Reverse-current protection diode | 1   | $0.10      | `[placeholder]` |
| AZ1117CH-3.3             | 3.3 V LDO regulator              | 1   | $0.16      | `[placeholder]` |
| PRTR5V0U2X               | USB ESD protection               | 1   | $0.47      | `[placeholder]` |
| USB-CB16HTBTR            | USB-C connector                  | 1   | $1.05      | `[placeholder]` |
| 10 µF capacitors (0603)  | Decoupling capacitors            | 3   | $0.30      | `[placeholder]` |
| 0.1 µF capacitors (0603) | Decoupling capacitors            | 4   | $0.32      | `[placeholder]` |
| SML-D12U1WT86            | Red LED                          | 1   | $0.12      | `[placeholder]` |
| SKQGABE010               | Button                           | 1   | $0.31      | `[placeholder]` |
| 5.1 kΩ resistors (0603)  | Pull-up resistors                | 2   | $0.32      | `[placeholder]` |
| 1 kΩ resistor (0603)     | LED resistor                     | 1   | $0.10      | `[placeholder]` |
| 10 kΩ resistor (0603)    | Reset/logic pull-up              | 1   | $0.10      | `[placeholder]` |
| DF40C-20DP-0.4V          | Board connector plug             | 2   | $1.38      | `[placeholder]` |

**Subtotal:** $9.79

---

## MCU Interposer

| Part Name                | Description           | Qty | Total Cost | Link            |
| ------------------------ | --------------------- | --- | ---------- | --------------- |
| DF40C-20DS-0.4V          | Board receptacle      | 2   | $1.16      | `[placeholder]` |
| 10 µF capacitors (0603)  | Decoupling capacitors | 2   | $0.20      | `[placeholder]` |
| 0.1 µF capacitors (0603) | Decoupling capacitors | 2   | $0.16      | `[placeholder]` |
| SKQGABE010               | Button                | 3   | $0.93      | `[placeholder]` |
| LTST-C193TBKT            | Blue LED              | 4   | $0.72      | `[placeholder]` |
| 1 kΩ resistors (0603)    | LED resistors         | 4   | $0.40      | `[placeholder]` |

**Subtotal:** $3.57

---

## Dev Board Interposer

| Part Name          | Description             | Qty | Total Cost | Link            |
| ------------------ | ----------------------- | --- | ---------- | --------------- |
| ESP32-S3-DevKitC-1 | ESP32-S3 dev board      | 1   | $15.00     | `[placeholder]` |
| 10 µF capacitors   | Decoupling capacitors   | 2   | $0.20      | `[placeholder]` |
| 0.1 µF capacitors  | Decoupling capacitors   | 2   | $0.16      | `[placeholder]` |
| SKQGABE010         | Button                  | 3   | $0.93      | `[placeholder]` |
| LTST-C193TBKT      | Blue LED                | 4   | $0.72      | `[placeholder]` |
| 1 kΩ resistors     | LED resistors           | 4   | $0.40      | `[placeholder]` |
| PPTC221LFBN        | 1×22 female pin headers | 2   | $2.40      | `[placeholder]` |

**Subtotal:** $19.81

---

## Input Boards

| Part Name              | Description          | Qty | Total Cost | Link            |
| ---------------------- | -------------------- | --- | ---------- | --------------- |
| 1 µF capacitors (0603) | Filtering capacitors | 4   | $0.40      | `[placeholder]` |
| 9032 Joysticks         | Dual-axis joysticks  | 2   | $10.50     | `[placeholder]` |
| B3F-1000 Buttons       | Momentary buttons    | 12  | $2.88      | `[placeholder]` |
| PRPC040SACN            | Header pins (40-pin) | 2   | $1.68      | `[placeholder]` |

**Subtotal:** $15.46

---

## Receiver Boards

| Part Name   | Description             | Qty | Total Cost | Link            |
| ----------- | ----------------------- | --- | ---------- | --------------- |
| PPPC041LFBN | 1×4 pin header sockets  | 16  | $6.08      | `[placeholder]` |
| PRPC040SBAN | Right-angle header pins | —   | $0.00      | `[placeholder]` |
| 26 AWG Wire | Stranded silicone wire  | 1   | $9.99      | `[placeholder]` |

**Subtotal:** $16.07

---

## PCB Fabrication

| Part Name                  | Description   | Qty Used | Cost  | Link            |
| -------------------------- | ------------- | -------- | ----- | --------------- |
| Custom Main Controller PCB | JLCPCB, 5 pcs | 1 used   | $5.00 | `[placeholder]` |
| MC Carrier PCB             | JLCPCB, 5 pcs | 1 used   | $5.00 | `[placeholder]` |
| Dev Board Carrier PCB      | JLCPCB, 5 pcs | 1 used   | $5.00 | `[placeholder]` |
| Button Module PCB          | JLCPCB, 5 pcs | 2 used   | $5.00 | `[placeholder]` |
| Joystick Module PCB        | JLCPCB, 5 pcs | 2 used   | $5.00 | `[placeholder]` |
| Receiver Module PCB        | JLCPCB, 5 pcs | 4 used   | $5.00 | `[placeholder]` |

**Subtotal:** $30.00

---

## Shell / Enclosure

| Part Name        | Description                | Qty | Total Cost | Link            |
| ---------------- | -------------------------- | --- | ---------- | --------------- |
| PLA Filament     | 3D-printed shell material  | 1   | $2.50      | `[placeholder]` |
| M3 Brass Inserts | Heat-set inserts (150 pcs) | 12  | $5.69      | `[placeholder]` |
| M3 Screws        | Assorted screw kit         | 12  | $6.99      | `[placeholder]` |

**Subtotal:** $15.18

---

## Total Summary

* **Total Parts:** 103
* **Total Cost:** **$109.88** (approx.)


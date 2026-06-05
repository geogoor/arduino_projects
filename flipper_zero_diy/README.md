# Flipper Zero DIY

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

> **📐 Design & reference — NOT a built PoC.**
> This project is documented as a design/architecture reference. It was **not** physically built, because it needs parts I don't have: an **IR LED** (transmitter, for replay) + 100Ω, and the optional **RC522 (RFID)**, **CC1101/HC-12 (Sub-GHz)** and an **Arduino Leonardo (BadUSB)**. The code and wiring are complete and correct — only the build/photo is pending until the parts are available.
>
> **📐 Σχεδίαση & αναφορά — ΟΧΙ built PoC.** Τεκμηριωμένο ως σχεδίαση/αρχιτεκτονική. **Δεν** χτίστηκε, γιατί θέλει εξαρτήματα που δεν έχω: **IR LED** (πομπό, για replay) + 100Ω, και προαιρετικά **RC522 (RFID)**, **CC1101/HC-12 (Sub-GHz)** και **Arduino Leonardo (BadUSB)**. Ο κώδικας & η συνδεσμολογία είναι πλήρη και σωστά — εκκρεμεί μόνο το χτίσιμο/φωτο μέχρι να υπάρξουν τα εξαρτήματα.

📄 **Code / Κώδικας:** [`flipper_zero_diy.ino`](flipper_zero_diy.ino)

> 📸 **Proof of Concept:** N/A — concept/design only (δεν χτίστηκε).

> ⚠️ For educational use / authorized testing only.

---

## English

A simplified Flipper Zero with an Arduino — a multi-tool that **captures and replays IR signals**, ships an **IR library** of common TV/AC codes, and has info screens for BadUSB / Sub-GHz / RFID (which need extra modules). Navigation is done with a **4×4 keypad**.

### Keys

| Key | Action |
|---|---|
| `A` | Next mode |
| `B` | Action for the current mode (capture / send) |
| `C` | Next item in the IR library |
| `1`–`6` | Jump straight to a mode |

Modes: IR Capture · IR Replay · IR Library · BadUSB info · Sub-GHz info · RFID info.

### Materials

| Part | Qty |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| IR receiver (TSOP38238) | 1 |
| IR LED | 1 |
| Resistor 100Ω | 1 |
| Keypad 4×4 | 1 |
| Green / Red LED | 1 each |
| Resistor 220Ω | 2 |
| Passive buzzer | 1 |
| Breadboard + jumpers | ~20 |

**Libraries:** `LiquidCrystal_I2C`, `IRremote` (v4.x), `Keypad`

> Optional add-ons: RC522 (RFID), HC-12/CC1101 (Sub-GHz), Arduino Leonardo (BadUSB).

### Wiring

```
IR receiver  OUT -> pin 11
IR LED       pin 3 -> 100Ω -> IR LED anode -> cathode -> GND
Keypad 4×4   ROW1-4 -> 2,4,5,7    COL1-4 -> 10,12,13,A0
Green LED    pin 8 -> 220Ω -> GND
Red LED      pin 9 -> 220Ω -> GND
Buzzer       + -> pin 6
LCD I2C      SDA -> A4   SCL -> A5
```

---

## Ελληνικά

Απλοποιημένο Flipper Zero με Arduino — multi-tool που **διαβάζει και αναπαράγει IR σήματα**, έχει **βιβλιοθήκη IR** με κοινούς κωδικούς TV/AC, και info οθόνες για BadUSB / Sub-GHz / RFID (που θέλουν επιπλέον modules). Η πλοήγηση γίνεται με **keypad 4×4**.

### Πλήκτρα

| Πλήκτρο | Ενέργεια |
|---|---|
| `A` | Επόμενο mode |
| `B` | Action του τρέχοντος mode (capture / send) |
| `C` | Επόμενο στοιχείο στη βιβλιοθήκη IR |
| `1`–`6` | Άμεσο πήδημα σε mode |

Modes: IR Capture · IR Replay · IR Library · BadUSB info · Sub-GHz info · RFID info.

### Υλικά

| Υλικό | Ποσότητα |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| IR receiver (TSOP38238) | 1 |
| IR LED | 1 |
| Αντίσταση 100Ω | 1 |
| Keypad 4×4 | 1 |
| LED πράσινο / κόκκινο | 1 το καθένα |
| Αντίσταση 220Ω | 2 |
| Passive buzzer | 1 |
| Breadboard + jumpers | ~20 |

**Libraries:** `LiquidCrystal_I2C`, `IRremote` (v4.x), `Keypad`

> Προαιρετικά: RC522 (RFID), HC-12/CC1101 (Sub-GHz), Arduino Leonardo (BadUSB).

### Συνδεσμολογία

```
IR receiver  OUT -> pin 11
IR LED       pin 3 -> 100Ω -> IR LED anode -> cathode -> GND
Keypad 4×4   ROW1-4 -> 2,4,5,7    COL1-4 -> 10,12,13,A0
LED πράσινο  pin 8 -> 220Ω -> GND
LED κόκκινο  pin 9 -> 220Ω -> GND
Buzzer       + -> pin 6
LCD I2C      SDA -> A4   SCL -> A5
```

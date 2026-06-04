# Smart Home Hub

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![Smart Home Hub](smart_home_hub.jpg)

> _Add your build photo here as `smart_home_hub.jpg` — Πρόσθεσε τη φωτογραφία της κατασκευής σου ως `smart_home_hub.jpg`._

📄 **Code / Κώδικας:** [`smart_home_hub.ino`](smart_home_hub.ino)

---

## English

A single board that ties four subsystems together: **auto lights** (PIR + LDR), **climate alert** (TMP36), **security alarm** (arm/disarm with a 4×4 keypad PIN) and **door control** (servo). Everything shows up live on the LCD. The "vertical slice" project that proves you can orchestrate several sensors/actuators with one state machine.

### Keypad commands

| Key(s) | Action |
|---|---|
| `1234` + `#` | Arm / Disarm (toggle) — default PIN |
| `A` | Open door (servo 90°) |
| `B` | Close door (servo 0°) |
| `C` | Manual light on/off |
| `*` | Clear PIN entry |

### How it works

- **Auto lights:** when the PIR sees motion **and** the LDR reads dark, the light turns on for `LIGHT_TIMEOUT` (8 s) then off. Disabled while armed.
- **Climate:** the yellow LED turns on when temperature exceeds `TEMP_LIMIT` (28 °C).
- **Security:** correct PIN + `#` toggles armed. While armed, PIR motion triggers the alarm (buzzer + flashing red) until you disarm.
- **Door:** `A` / `B` drive the servo open/closed.

### Materials

| Part | Qty |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| Keypad 4×4 | 1 |
| PIR sensor (HC-SR501) | 1 |
| TMP36 temperature sensor | 1 |
| LDR | 1 |
| Servo SG90 | 1 |
| Green / Yellow / Red LED | 1 each |
| Passive buzzer | 1 |
| Resistor 220Ω | 3 |
| Resistor 10KΩ | 1 (LDR divider) |
| Breadboard + jumpers | ~30 |

**Libraries:** `LiquidCrystal_I2C`, `Keypad`, `Servo`

### Wiring

```
Keypad 4×4   ROW1-4 -> 2,3,4,5    COL1-4 -> 6,7,8,9
PIR          OUT -> pin 10
Servo SG90   SIGNAL -> pin 11
Buzzer       + -> pin 12
Yellow LED   anode -> pin 13 -> 220Ω -> GND
TMP36        OUT -> A0
LDR          5V -> LDR -> A1 -> 10KΩ -> GND
Green LED    anode -> A2 -> 220Ω -> GND
Red LED      anode -> A3 -> 220Ω -> GND
LCD I2C      SDA -> A4   SCL -> A5
```

> ⚡ The SG90 can draw enough current on fast moves to reset the Arduino — power the servo from external 5V with a common ground if that happens.

### Future ideas

- HC-05 for remote arm/disarm + push alerts on the phone.
- EEPROM to store a user-changeable PIN.
- Duress code: a second hidden PIN that unlocks normally but sends a silent alert.

---

## Ελληνικά

Ένα κεντρικό «hub» που ενώνει τέσσερα subsystems σε μία πλακέτα: **αυτόματα φώτα** (PIR + LDR), **alert θερμοκρασίας** (TMP36), **συναγερμό ασφαλείας** (arm/disarm με PIN στο keypad 4×4) και **έλεγχο πόρτας** (servo). Όλα με live status στο LCD. Το project που δείχνει ότι ξέρεις να ενορχηστρώνεις πολλούς αισθητήρες/actuators μαζί με μία state machine.

### Εντολές keypad

| Πλήκτρο(α) | Ενέργεια |
|---|---|
| `1234` + `#` | Arm / Disarm (toggle) — προεπιλεγμένος PIN |
| `A` | Άνοιγμα πόρτας (servo 90°) |
| `B` | Κλείσιμο πόρτας (servo 0°) |
| `C` | Manual on/off φωτός |
| `*` | Καθαρισμός εισαγωγής PIN |

### Πώς δουλεύει

- **Auto φώτα:** όταν ο PIR δει κίνηση **και** το LDR δείχνει σκοτάδι, το φως ανάβει για `LIGHT_TIMEOUT` (8 δευτ.) και σβήνει. Ανενεργό όταν είναι armed.
- **Climate:** το κίτρινο LED ανάβει όταν η θερμοκρασία ξεπεράσει το `TEMP_LIMIT` (28 °C).
- **Ασφάλεια:** σωστός PIN + `#` κάνει toggle το armed. Όσο είναι armed, κίνηση στον PIR ενεργοποιεί τον συναγερμό (buzzer + κόκκινο που αναβοσβήνει) μέχρι να κάνεις disarm.
- **Πόρτα:** `A` / `B` κινούν τον servo.

### Υλικά

| Υλικό | Ποσότητα |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| Keypad 4×4 | 1 |
| PIR (HC-SR501) | 1 |
| TMP36 | 1 |
| LDR | 1 |
| Servo SG90 | 1 |
| LED πράσινο / κίτρινο / κόκκινο | 1 το καθένα |
| Passive buzzer | 1 |
| Αντίσταση 220Ω | 3 |
| Αντίσταση 10KΩ | 1 (LDR divider) |
| Breadboard + jumpers | ~30 |

**Libraries:** `LiquidCrystal_I2C`, `Keypad`, `Servo`

### Συνδεσμολογία

```
Keypad 4×4   ROW1-4 -> 2,3,4,5    COL1-4 -> 6,7,8,9
PIR          OUT -> pin 10
Servo SG90   SIGNAL -> pin 11
Buzzer       + -> pin 12
LED κίτρινο  anode -> pin 13 -> 220Ω -> GND
TMP36        OUT -> A0
LDR          5V -> LDR -> A1 -> 10KΩ -> GND
LED πράσινο  anode -> A2 -> 220Ω -> GND
LED κόκκινο  anode -> A3 -> 220Ω -> GND
LCD I2C      SDA -> A4   SCL -> A5
```

> ⚡ Ο SG90 σε απότομες κινήσεις τραβάει ρεύμα· αν το Arduino κάνει reset, τροφοδότησε τον servo από εξωτερικά 5V με κοινό GND.

### Ιδέες για το μέλλον

- HC-05 για remote arm/disarm + push alerts στο κινητό.
- EEPROM για αποθήκευση PIN που αλλάζει ο χρήστης.
- Duress code: δεύτερος κρυφός PIN που ξεκλειδώνει κανονικά αλλά στέλνει silent alert.

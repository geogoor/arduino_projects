# Motion Security System

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![Motion Security System](motion_security_system.jpg)

> _Add your build photo here as `motion_security_system.jpg` — Πρόσθεσε τη φωτογραφία της κατασκευής σου ως `motion_security_system.jpg`._

📄 **Code / Κώδικας:** [`motion_security_system.ino`](motion_security_system.ino)

---

## English

A burglar alarm with **double motion confirmation** (PIR + HC-SR04 for fewer false positives), arm/disarm with a **4×4 keypad PIN**, **Bluetooth push alerts** to a phone (HC-05), and entry/exit delays. The defensive counterpart to the offensive projects.

### Why two sensors?

A PIR catches motion/heat but fires on drafts and sunlight. The HC-SR04 confirms something actually entered range. The alarm only triggers when **both agree** within a 2 s window — far fewer false alarms (sensor fusion).

### State machine

```
DISARMED → PIN + # → (exit delay 10s) → ARMED
ARMED → PIR + HC-SR04 agree → ENTRY (8s to disarm)
ENTRY → no disarm → ALARM (buzzer + red + BT "$ALERT,INTRUSION#")
ALARM → PIN + # → DISARMED
```

### Bluetooth protocol (to the phone)

```
$STATUS,ARMED#        state changes
$ALERT,INTRUSION#     when the alarm fires
```

### Materials

| Part | Qty |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| Keypad 4×4 | 1 |
| PIR (HC-SR501) | 1 |
| HC-SR04 ultrasonic | 1 |
| HC-05 Bluetooth | 1 |
| Green / Red LED | 1 each |
| Passive buzzer | 1 |
| Resistor 220Ω | 2 |
| Resistor 1KΩ + 2KΩ | 1 each (HC-05 RX divider) |
| Breadboard + jumpers | ~32 |

**Libraries:** `LiquidCrystal_I2C`, `Keypad`, `SoftwareSerial`

### Wiring

```
Keypad 4×4   ROW1-4 -> 2,3,4,5    COL1-4 -> 6,7,8,9
PIR          OUT -> pin 10
HC-SR04      TRIG -> pin 11   ECHO -> pin 12
Buzzer       + -> pin 13
Green LED    anode -> A2 -> 220Ω -> GND
Red LED      anode -> A3 -> 220Ω -> GND
HC-05        TXD -> A0    RXD -> divider (1KΩ/2KΩ) -> A1
LCD I2C      SDA -> A4    SCL -> A5
```

---

## Ελληνικά

Συναγερμός με **διπλή επιβεβαίωση κίνησης** (PIR + HC-SR04 για λιγότερα false positives), arm/disarm με **PIN στο keypad 4×4**, **push alerts μέσω Bluetooth** στο κινητό (HC-05), και entry/exit delays. Το defensive αντίστοιχο των offensive projects.

### Γιατί δύο αισθητήρες;

Ο PIR πιάνει κίνηση/θερμότητα αλλά δίνει false positives (ρεύματα αέρα, ήλιος). Το HC-SR04 επιβεβαιώνει ότι κάτι όντως μπήκε στην εμβέλεια. Συναγερμός μόνο όταν **συμφωνούν** εντός παραθύρου 2 δευτ. — πολύ λιγότερα ψεύτικα alarms (sensor fusion).

### State machine

```
DISARMED → PIN + # → (exit delay 10s) → ARMED
ARMED → PIR + HC-SR04 συμφωνούν → ENTRY (8s για disarm)
ENTRY → χωρίς disarm → ALARM (buzzer + κόκκινο + BT "$ALERT,INTRUSION#")
ALARM → PIN + # → DISARMED
```

### Bluetooth protocol (προς το κινητό)

```
$STATUS,ARMED#        αλλαγές κατάστασης
$ALERT,INTRUSION#     όταν χτυπά ο συναγερμός
```

### Υλικά

| Υλικό | Ποσότητα |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| Keypad 4×4 | 1 |
| PIR (HC-SR501) | 1 |
| HC-SR04 ultrasonic | 1 |
| HC-05 Bluetooth | 1 |
| LED πράσινο / κόκκινο | 1 το καθένα |
| Passive buzzer | 1 |
| Αντίσταση 220Ω | 2 |
| Αντίσταση 1KΩ + 2KΩ | 1 το καθένα (HC-05 RX divider) |
| Breadboard + jumpers | ~32 |

**Libraries:** `LiquidCrystal_I2C`, `Keypad`, `SoftwareSerial`

### Συνδεσμολογία

```
Keypad 4×4   ROW1-4 -> 2,3,4,5    COL1-4 -> 6,7,8,9
PIR          OUT -> pin 10
HC-SR04      TRIG -> pin 11   ECHO -> pin 12
Buzzer       + -> pin 13
LED πράσινο  anode -> A2 -> 220Ω -> GND
LED κόκκινο  anode -> A3 -> 220Ω -> GND
HC-05        TXD -> A0    RXD -> divider (1KΩ/2KΩ) -> A1
LCD I2C      SDA -> A4    SCL -> A5
```

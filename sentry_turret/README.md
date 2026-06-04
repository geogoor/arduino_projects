# Sentry Turret

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![Sentry Turret](sentry_turret.jpg)

> _Add your build photo here as `sentry_turret.jpg` — Πρόσθεσε τη φωτογραφία της κατασκευής σου ως `sentry_turret.jpg`._

📄 **Code / Κώδικας:** [`sentry_turret.ino`](sentry_turret.ino)

---

## English

A servo-mounted HC-SR04 that **sweeps the area** in an arc. When the PIR catches motion, the turret **stops and locks onto the target**, lights a red LED + buzzer alarm, and prints the target's angle + distance over serial (target lock). Idle = green LED + slow sweep. The most photogenic project for a PoC video (it moves and beeps).

### How it works

```
IDLE (SCANNING): servo 0°→180°→0°, HC-SR04 measures each step (radar over serial), green LED
PIR motion → TARGET LOCK: stop, fast sweep, keep the angle with the smallest distance (= the target),
             turn there, red LED + buzzer, hold 3s, then back to SCANNING
```

**Target acquisition:** when the PIR fires, the turret does a fast sweep, keeps the angle with the **minimum distance** (the closest object = the intruder) and turns there — a simple scanning-radar / auto-targeting idea.

### Mounting

Attach the HC-SR04 to the servo arm (sensor "eyes" facing forward). The servo rotates the sensor = it scans the area. The PIR stays fixed on the base.

### Materials

| Part | Qty |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| Servo SG90 (pan) | 1 |
| HC-SR04 ultrasonic | 1 |
| PIR (HC-SR501) | 1 |
| Green / Red LED | 1 each |
| Passive buzzer | 1 |
| Resistor 220Ω | 2 |
| Breadboard + jumpers | ~20 |

**Libraries:** `LiquidCrystal_I2C`, `Servo`

### Wiring

```
Servo SG90   SIGNAL -> pin 9
HC-SR04      TRIG -> pin 5    ECHO -> pin 6     (mounted on the servo)
PIR          OUT -> pin 2                       (on the base)
Green LED    anode -> pin 7 -> 220Ω -> GND
Red LED      anode -> pin 8 -> 220Ω -> GND
Buzzer       + -> pin 4
LCD I2C      SDA -> A4   SCL -> A5
```

> ⚡ The servo draws current while moving — if the Arduino resets, give the servo external 5V with a common ground.

### Future ideas

- A Processing/Python radar that reads the `scan ...` serial and draws the sweep.
- A second servo (tilt) for a 2-axis turret.
- Continuous tracking (PID on the angle) instead of a 3 s lock.

---

## Ελληνικά

Servo-mounted HC-SR04 που **σαρώνει** τον χώρο σε τόξο. Όταν ο PIR πιάσει κίνηση, ο πύργος **σταματά και «κλειδώνει» στον στόχο**, ανάβει κόκκινο LED + buzzer alarm και τυπώνει στο serial τη γωνία + απόσταση του στόχου (target lock). Σε ηρεμία: πράσινο LED + αργό sweep. Το πιο θεαματικό project για PoC video (κινείται + ηχεί).

### Πώς δουλεύει

```
IDLE (SCANNING): servo 0°→180°→0°, HC-SR04 μετράει σε κάθε βήμα (radar στο serial), πράσινο LED
PIR κίνηση → TARGET LOCK: stop, γρήγορο sweep, κράτα τη γωνία με τη μικρότερη απόσταση (= στόχος),
             στρίψε εκεί, κόκκινο LED + buzzer, κράτα 3s, μετά πίσω σε SCANNING
```

**Target acquisition:** όταν ενεργοποιηθεί ο PIR, ο πύργος κάνει γρήγορο σάρωμα, κρατάει τη γωνία με την **ελάχιστη απόσταση** (το πιο κοντινό αντικείμενο = ο εισβολέας) και στρέφεται εκεί — απλή εκδοχή scanning radar / auto-targeting.

### Mounting

Στερέωσε το HC-SR04 πάνω στον βραχίονα του servo (τα «μάτια» κοιτούν μπροστά). Ο servo περιστρέφει τον αισθητήρα = σαρώνει τον χώρο. Ο PIR μένει σταθερός στη βάση.

### Υλικά

| Υλικό | Ποσότητα |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| Servo SG90 (pan) | 1 |
| HC-SR04 ultrasonic | 1 |
| PIR (HC-SR501) | 1 |
| LED πράσινο / κόκκινο | 1 το καθένα |
| Passive buzzer | 1 |
| Αντίσταση 220Ω | 2 |
| Breadboard + jumpers | ~20 |

**Libraries:** `LiquidCrystal_I2C`, `Servo`

### Συνδεσμολογία

```
Servo SG90   SIGNAL -> pin 9
HC-SR04      TRIG -> pin 5    ECHO -> pin 6     (πάνω στον servo)
PIR          OUT -> pin 2                       (στη βάση)
LED πράσινο  anode -> pin 7 -> 220Ω -> GND
LED κόκκινο  anode -> pin 8 -> 220Ω -> GND
Buzzer       + -> pin 4
LCD I2C      SDA -> A4   SCL -> A5
```

> ⚡ Ο servo σε κίνηση τραβάει ρεύμα· αν το Arduino κάνει reset, δώσε εξωτερικά 5V με κοινό GND.

### Ιδέες για το μέλλον

- Processing/Python radar που διαβάζει το serial `scan ...` και ζωγραφίζει το sweep.
- Δεύτερος servo (tilt) για 2-axis turret.
- Continuous tracking (PID στη γωνία) αντί για 3s lock.

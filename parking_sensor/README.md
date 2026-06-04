# Parking Sensor

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![Parking Sensor](parking_sensor.jpg)

> _Add your build photo here as `parking_sensor.jpg` — Πρόσθεσε τη φωτογραφία της κατασκευής σου ως `parking_sensor.jpg`._

📄 **Code / Κώδικας:** [`parking_sensor.ino`](parking_sensor.ino)

---

## English

A car-style parking assistant. The closer you get to an obstacle, the faster the beeping, until it becomes continuous and tells you to stop. Distance is measured with an HC-SR04, and an IR sensor on top confirms that something is actually in front (the ultrasonic sometimes bounces off angled surfaces).

My favourite part is the potentiometer: I set the STOP threshold live (5–30 cm) by turning it, without re-uploading code, and I see it change on the screen instantly.

### How it works

Everything comes down to the HC-SR04 distance compared to the threshold set by the potentiometer (`stopDist`):

- **Above ~150 cm (or nothing ahead):** `SPACE AVAILABLE`, green LED, silent.
- **Approaching:** bar graph on the LCD + beeping that speeds up as you get closer. Below 50 cm the red LED turns on, otherwise green.
- **Below the threshold (`stopDist`):** `!! STOP !!`, solid red, continuous tone.

The second line always shows the current potentiometer threshold plus `IR: Y/N` (whether the IR sensor sees an object).

### Materials

| Part | Notes | Qty |
|---|---|---|
| Arduino Uno | + USB | 1 |
| HC-SR04 | ultrasonic distance | 1 |
| IR sensor (FlyingFish) | presence confirmation | 1 |
| Potentiometer 10KΩ | sets STOP threshold | 1 |
| LCD 16x2 I2C (QAPASS) | display | 1 |
| Passive buzzer | sound | 1 |
| Red LED | STOP | 1 |
| Green LED | free / OK | 1 |
| Resistor 1KΩ | for the LEDs | 2 |
| Breadboard (half) | | 1 |
| Jumper wires M-M | | ~13 |

### Wiring

```
HC-SR04          TRIG -> pin 7   ECHO -> pin 6   VCC -> 5V   GND -> GND
IR sensor        OUT  -> pin 2   VCC  -> 5V      GND -> GND
Potentiometer    wiper -> A0     ends -> 5V / GND
LCD 16x2 I2C     SDA  -> A4      SCL  -> A5      VCC -> 5V   GND -> GND
Passive buzzer   +    -> pin 8   -    -> GND
Red LED          anode -> 1KΩ -> pin 12   cathode -> GND
Green LED        anode -> 1KΩ -> pin 13   cathode -> GND
```

### On the LCD

```
[#######...] 68cm        !!   STOP   !!
Stop@:15cm IR:N          Dist:12cm IR:Y
```

### Future ideas

- Median filter on the HC-SR04 readings (it throws the odd crazy value).
- Send the distance to a phone over HC-05 (same idea as the weather dashboard).
- A servo "garage bar" that opens once a stable close distance is detected.

---

## Ελληνικά

Βοηθός παρκαρίσματος, σαν αυτούς που έχουν τα αμάξια πίσω: όσο πλησιάζεις σε εμπόδιο, το beep πυκνώνει, μέχρι που γίνεται συνεχόμενο και σου λέει «φτάνει, σταμάτα». Η απόσταση μετριέται με HC-SR04 και ένας IR sensor από πάνω επιβεβαιώνει ότι όντως υπάρχει κάτι μπροστά (ο υπέρηχος καμιά φορά γκελάρει σε λοξές επιφάνειες).

Το κομμάτι που μου άρεσε πιο πολύ είναι το ποτενσιόμετρο: ρυθμίζω ζωντανά το όριο του STOP (5–30 cm) γυρνώντας το, χωρίς να ξαναανεβάζω κώδικα. Το βλέπω αμέσως να αλλάζει στην οθόνη.

### Πώς δουλεύει

Όλο το παιχνίδι είναι η απόσταση από το HC-SR04 σε σχέση με το όριο που έχω βάλει στο ποτενσιόμετρο (`stopDist`):

- **Πάνω από ~150 cm (ή τίποτα μπροστά):** `SPACE AVAILABLE`, πράσινο, σιωπή.
- **Πλησιάζει:** bar graph στην οθόνη + beep που πυκνώνει όσο κοντεύεις. Κάτω από 50 cm ανάβει κόκκινο, αλλιώς πράσινο.
- **Κάτω από το όριο (`stopDist`):** `!! STOP !!`, κόκκινο σταθερό, συνεχόμενος ήχος.

Στη δεύτερη γραμμή δείχνω πάντα το όριο του ποτενσιόμετρου εκείνη τη στιγμή, μαζί με το `IR: Y/N`.

### Υλικά

| Υλικό | Σημείωση | Ποσότητα |
|---|---|---|
| Arduino Uno | + USB | 1 |
| HC-SR04 | απόσταση (υπέρηχος) | 1 |
| IR sensor (FlyingFish) | επιβεβαίωση παρουσίας | 1 |
| Potentiometer 10KΩ | ρύθμιση ορίου STOP | 1 |
| LCD 16x2 I2C (QAPASS) | οθόνη | 1 |
| Passive buzzer | ήχος | 1 |
| LED κόκκινο | STOP | 1 |
| LED πράσινο | ελεύθερο / OK | 1 |
| Αντίσταση 1KΩ | για τα LED | 2 |
| Breadboard (half) | | 1 |
| Jumper M-M | | ~13 |

### Συνδεσμολογία

```
HC-SR04          TRIG -> pin 7   ECHO -> pin 6   VCC -> 5V   GND -> GND
IR sensor        OUT  -> pin 2   VCC  -> 5V      GND -> GND
Potentiometer    μεσαίο -> A0    άκρα -> 5V / GND
LCD 16x2 I2C     SDA  -> A4      SCL  -> A5      VCC -> 5V   GND -> GND
Passive buzzer   +    -> pin 8   -    -> GND
LED κόκκινο      anode -> 1KΩ -> pin 12   cathode -> GND
LED πράσινο      anode -> 1KΩ -> pin 13   cathode -> GND
```

### Ιδέες για το μέλλον

- Median filter στις μετρήσεις του HC-SR04 (πετάει κανα-δυο τρελές τιμές).
- Αποστολή της απόστασης στο κινητό με HC-05 (ίδιο σκεπτικό με τον μετεωρολογικό σταθμό).
- Servo «μπάρα γκαράζ» που ανοίγει όταν σταθεροποιηθεί κοντινή απόσταση.

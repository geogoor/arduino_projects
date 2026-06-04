# Weather Dashboard

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![Weather Dashboard](weather_dashboard.jpg)

> _Add your build photo here as `weather_dashboard.jpg` — Πρόσθεσε τη φωτογραφία της κατασκευής σου ως `weather_dashboard.jpg`._

📄 **Code / Κώδικας:** [`weather_dashboard.ino`](weather_dashboard.ino)

---

## English

A standalone weather station with temperature (TMP36) and light (LDR), shown on the LCD with **trend arrows (↑↓→)**, **min/max tracking**, a **3-LED brightness bar** and **CSV logging** over serial. Two buttons toggle °C/°F and reset the min/max.

### How it works

- **LCD line 1:** temperature + trend arrow + max.
- **LCD line 2:** light % + an 8-character bar graph.
- **LED bar:** 3 LEDs light up by brightness (low / medium / high).
- **Button 1:** switch °C / °F. **Button 2:** reset min/max.
- **Serial:** prints a CSV row every 5 s — open the Serial Monitor or paste into Excel/Sheets for a chart.

```
timestamp_ms,temp_c,temp_f,light_pct,temp_min,temp_max
5000,22.45,72.41,63,22.45,22.45
10000,22.61,72.70,58,22.45,22.61
```

### Materials

| Part | Qty |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| TMP36 | 1 |
| LDR | 1 |
| Green / Yellow / Red LED | 1 each |
| Push button | 2 |
| Resistor 220Ω | 3 |
| Resistor 10KΩ | 1 (LDR divider) |
| Breadboard + jumpers | ~18 |

**Library:** `LiquidCrystal_I2C`

### Wiring

```
TMP36        OUT -> A0
LDR          5V -> LDR -> A1 -> 10KΩ -> GND
Green LED    pin 5 -> 220Ω -> GND
Yellow LED   pin 6 -> 220Ω -> GND
Red LED      pin 7 -> 220Ω -> GND
Button 1     pin 2 -> GND   (°C/°F, INPUT_PULLUP)
Button 2     pin 3 -> GND   (reset min/max, INPUT_PULLUP)
LCD I2C      SDA -> A4   SCL -> A5
```

---

## Ελληνικά

Αυτόνομος σταθμός καιρού με θερμοκρασία (TMP36) και φωτεινότητα (LDR), στο LCD με **βελάκια τάσης (↑↓→)**, **min/max**, **bar graph 3 LED** και **CSV logging** στο serial. Δύο κουμπιά κάνουν toggle °C/°F και reset το min/max.

### Πώς δουλεύει

- **LCD γραμμή 1:** θερμοκρασία + trend + max.
- **LCD γραμμή 2:** φωτεινότητα % + bar graph 8 χαρακτήρων.
- **LED bar:** 3 LED ανάβουν ανάλογα με τη φωτεινότητα (low / med / high).
- **Κουμπί 1:** εναλλαγή °C / °F. **Κουμπί 2:** reset min/max.
- **Serial:** μία γραμμή CSV κάθε 5 δευτ. — άνοιξε το Serial Monitor ή αντέγραψε σε Excel/Sheets για γράφημα.

### Υλικά

| Υλικό | Ποσότητα |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| TMP36 | 1 |
| LDR | 1 |
| LED πράσινο / κίτρινο / κόκκινο | 1 το καθένα |
| Push button | 2 |
| Αντίσταση 220Ω | 3 |
| Αντίσταση 10KΩ | 1 (LDR divider) |
| Breadboard + jumpers | ~18 |

**Library:** `LiquidCrystal_I2C`

### Συνδεσμολογία

```
TMP36        OUT -> A0
LDR          5V -> LDR -> A1 -> 10KΩ -> GND
LED πράσινο  pin 5 -> 220Ω -> GND
LED κίτρινο  pin 6 -> 220Ω -> GND
LED κόκκινο  pin 7 -> 220Ω -> GND
Κουμπί 1     pin 2 -> GND   (°C/°F, INPUT_PULLUP)
Κουμπί 2     pin 3 -> GND   (reset min/max, INPUT_PULLUP)
LCD I2C      SDA -> A4   SCL -> A5
```

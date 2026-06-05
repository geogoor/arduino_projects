# IoT Weather Dashboard (Bluetooth + Android App)

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![IoT Weather Dashboard](iot_weather_dashboard_bluetooth.jpg)

> _Add your build photo here as `iot_weather_dashboard_bluetooth.jpg` — Πρόσθεσε τη φωτογραφία της κατασκευής σου ως `iot_weather_dashboard_bluetooth.jpg`._

### 📸 Proof of Concept — notes on the photos

The photos capture **different phases of operation**, not just a static build:

- **Sensor manipulation:** covering the LDR with a finger/hand to **simulate night** — you can see the condition flip to `DARK`/`NIGHT` and the light % drop, both on the LCD and live on the phone.
- **LCD screens:** the on-board button (SW1) cycles the display, so the photos show the **4 different LCD pages** — `Temp °C/°F`, `Light % + condition`, `Max/Min`, `Uptime + BT Active`.
- **App ↔ hardware in sync:** shots showing the phone dashboard mirroring the same readings as the LCD at that moment (proves the live Bluetooth link).
- **Alert state:** (optional) warming the TMP36 to push temperature high enough to trigger the red **alert card** on the app + the red LED/buzzer.

> _Οι φωτογραφίες δείχνουν **διαφορετικές φάσεις λειτουργίας**, όχι απλώς στατική κατασκευή:_
> - _**Χειρισμός αισθητήρα:** κρύβω τον LDR με το χέρι για να **προσομοιώσω νύχτα** — φαίνεται η κατάσταση να γίνεται `DARK`/`NIGHT` και το φως % να πέφτει, και στην LCD και ζωντανά στο κινητό._
> - _**Οθόνες LCD:** το κουμπί SW1 αλλάζει σελίδα, οπότε στις φωτο φαίνονται οι **4 διαφορετικές οθόνες** — `Temp °C/°F`, `Light % + κατάσταση`, `Max/Min`, `Uptime + BT Active`._
> - _**App ↔ hardware συγχρονισμένα:** λήψεις όπου ο πίνακας στο κινητό δείχνει τις ίδιες μετρήσεις με την LCD τη συγκεκριμένη στιγμή (αποδεικνύει τη ζωντανή σύνδεση Bluetooth)._
> - _**Κατάσταση alert:** (προαιρετικά) ζεσταίνοντας το TMP36 ώστε να ανέβει η θερμοκρασία και να ενεργοποιηθεί η κόκκινη **κάρτα alert** στην app + το κόκκινο LED/buzzer._

📄 **Arduino code:** [`iot_weather_dashboard_bluetooth.ino`](iot_weather_dashboard_bluetooth.ino)
📱 **Android app:** [`android/MainActivity.kt`](android/MainActivity.kt) · [`android/activity_main.xml`](android/activity_main.xml)

---

## English

A full **IoT stack**: TMP36 + LDR → Arduino → HC-05 Bluetooth → a native Android app (Kotlin).

```
[Sensors] -> [Arduino Uno] -> [HC-05 BT] -> [Android App]
 TMP36/LDR     protocol         RFCOMM        Kotlin/XML
```

**Portfolio value:** custom serial protocol, native Android Bluetooth RFCOMM, dark-theme dashboard with live data.

### Data protocol

Sent every 2 seconds:

```
$W,23.5,74.3,78,SUNNY,25.1,18.2,3600,0#
```

| Index | Value | Meaning |
|---|---|---|
| 0 | `23.5` | °C |
| 1 | `74.3` | °F |
| 2 | `78` | light % |
| 3 | `SUNNY` | NIGHT / DARK / CLOUDY / BRIGHT / SUNNY |
| 4 | `25.1` | max °C |
| 5 | `18.2` | min °C |
| 6 | `3600` | uptime (s) |
| 7 | `0` | alert: 0=OK, 1=HIGH, 2=LOW |

`$W,` = start marker · `#` = end marker (framing for clean parsing).

### Materials

- Arduino Uno
- HC-05 or HC-06 Bluetooth module
- LCD 16x2 I2C (QAPASS, 0x27)
- TMP36 temperature sensor
- LDR
- Passive buzzer
- Red / Yellow / Green LED
- 2× push button
- Resistors: 3× 220Ω, 1× 1kΩ, 1× 2kΩ, 1× 10kΩ
- Breadboard + jumpers
- Android phone (API 21+)

### Wiring

```
HC-05  TXD -> pin 10              (SoftwareSerial RX, direct)
       RXD -> divider -> pin 11   (pin 11 -> 1kΩ -> RXD, and RXD -> 2kΩ -> GND)
LCD    SDA -> A4   SCL -> A5      VCC -> 5V   GND -> GND
TMP36  OUT -> A0
LDR    one end -> 5V   other -> A1 and 10kΩ -> GND
LEDs   Red -> pin 4   Yellow -> pin 5   Green -> pin 6   (each via 220Ω -> GND)
Buzzer + -> pin 8
Button 1 (cycle LCD) -> pin 2 -> GND     (INPUT_PULLUP)
Button 2 (reset min/max) -> pin 3 -> GND (INPUT_PULLUP)
```

> The HC-05 RX is 3.3V logic — keep the 1kΩ/2kΩ divider so the 5V TX from the Arduino does not damage it.

> **Build note (improvisation):** I had run out of resistors for the **green LED**, so I improvised with a **10kΩ potentiometer** wired in series as a variable current limiter — turned up enough to protect the LED while keeping it visible. A fixed 220Ω is the proper part; the pot just got the build working with what I had on hand.

### Android app setup

1. **Android Studio → New Project → Empty Views Activity**, language **Kotlin**, package `com.weather.dashboard`, min SDK **21**.
2. Enable ViewBinding in `app/build.gradle`:
   ```gradle
   android { buildFeatures { viewBinding true } }
   dependencies { implementation 'androidx.cardview:cardview:1.0.0' }
   ```
3. Add Bluetooth permissions to `AndroidManifest.xml`:
   ```xml
   <uses-permission android:name="android.permission.BLUETOOTH" android:maxSdkVersion="30" />
   <uses-permission android:name="android.permission.BLUETOOTH_ADMIN" android:maxSdkVersion="30" />
   <uses-permission android:name="android.permission.BLUETOOTH_CONNECT" />
   ```
4. Replace `MainActivity.kt` and `res/layout/activity_main.xml` with the files in [`android/`](android/).
5. **Pair** the HC-05 from Android Settings (PIN `1234` or `0000`), run the app, tap **Connect**, pick the device.

### How to use it

1. **Power the Arduino.** The LCD shows `IoT Weather / Starting...`, then live readings. The Bluetooth module starts blinking fast (not yet connected).
2. **Pair once** (first time only): phone **Settings → Bluetooth → HC-05 → Pair**, PIN `1234`/`0000`.
3. **Open the app**, grant the Bluetooth/Location permissions, tap **Connect** and pick the paired module. Status turns **Connected** and the dashboard fills in within ~2 s (data arrives every 2 s).
4. **Read the dashboard:** temperature °C/°F, light %, condition (NIGHT → SUNNY), max/min, uptime. The **alert card** pops up automatically — red when `≥ 35 °C`, blue when `≤ 5 °C`.
5. **On-board buttons** (on the hardware, not the app):
   - **SW1 (pin 2):** cycle the LCD screen — Temp → Light → Max/Min → Uptime.
   - **SW2 (pin 3):** reset the max/min record.
6. **Quick live test:** touch the TMP36 with a finger → temperature climbs on the phone. Cover the LDR → light % drops and the condition turns DARK/NIGHT.
7. **Disconnect:** tap the button again (now **Disconnect**) or close the app — the socket closes itself (`onDestroy`).

### Troubleshooting

| Problem | Cause | Fix |
|---|---|---|
| "Connection failed" | baud mismatch | HC-05 default 38400 → set `btSerial.begin(38400)` |
| Not in the list | not paired | Settings → Bluetooth → Pair |
| No data | parsing | check the Serial Monitor format |
| Crash on launch | missing permission | confirm `BLUETOOTH_CONNECT` in the manifest |

---

## Ελληνικά

Πλήρες **IoT stack**: TMP36 + LDR → Arduino → HC-05 Bluetooth → native Android app (Kotlin).

```
[Αισθητήρες] -> [Arduino Uno] -> [HC-05 BT] -> [Android App]
 TMP36/LDR        protocol         RFCOMM        Kotlin/XML
```

**Αξία για portfolio:** custom serial protocol, native Android Bluetooth RFCOMM, dark-theme dashboard με live δεδομένα.

### Πρωτόκολλο δεδομένων

Αποστολή κάθε 2 δευτερόλεπτα:

```
$W,23.5,74.3,78,SUNNY,25.1,18.2,3600,0#
```

| Index | Τιμή | Σημασία |
|---|---|---|
| 0 | `23.5` | °C |
| 1 | `74.3` | °F |
| 2 | `78` | φως % |
| 3 | `SUNNY` | NIGHT / DARK / CLOUDY / BRIGHT / SUNNY |
| 4 | `25.1` | max °C |
| 5 | `18.2` | min °C |
| 6 | `3600` | uptime (δευτ.) |
| 7 | `0` | alert: 0=OK, 1=ΥΨΗΛΗ, 2=ΧΑΜΗΛΗ |

`$W,` = start marker · `#` = end marker (framing για σωστό parsing).

### Υλικά

- Arduino Uno
- HC-05 ή HC-06 Bluetooth module
- LCD 16x2 I2C (QAPASS, 0x27)
- TMP36
- LDR
- Passive buzzer
- LED κόκκινο / κίτρινο / πράσινο
- 2× push button
- Αντιστάσεις: 3× 220Ω, 1× 1kΩ, 1× 2kΩ, 1× 10kΩ
- Breadboard + jumpers
- Android κινητό (API 21+)

### Συνδεσμολογία

```
HC-05  TXD -> pin 10              (SoftwareSerial RX, απευθείας)
       RXD -> divider -> pin 11   (pin 11 -> 1kΩ -> RXD, και RXD -> 2kΩ -> GND)
LCD    SDA -> A4   SCL -> A5      VCC -> 5V   GND -> GND
TMP36  OUT -> A0
LDR    ένα άκρο -> 5V   άλλο -> A1 και 10kΩ -> GND
LEDs   κόκκινο -> pin 4   κίτρινο -> pin 5   πράσινο -> pin 6   (το καθένα μέσω 220Ω -> GND)
Buzzer + -> pin 8
Κουμπί 1 (cycle LCD) -> pin 2 -> GND       (INPUT_PULLUP)
Κουμπί 2 (reset min/max) -> pin 3 -> GND   (INPUT_PULLUP)
```

> Το RX του HC-05 είναι λογική 3.3V — κράτα τον διαιρέτη 1kΩ/2kΩ ώστε τα 5V του Arduino να μην το καταστρέψουν.

> **Σημείωση κατασκευής (αυτοσχεδιασμός):** Δεν είχα άλλη αντίσταση για το **πράσινο LED**, οπότε αυτοσχεδίασα με ένα **ποτενσιόμετρο 10kΩ** συνδεδεμένο σε σειρά ως μεταβλητός περιοριστής ρεύματος — το ανέβασα όσο χρειαζόταν για να προστατεύεται το LED αλλά να παραμένει ορατό. Το σωστό εξάρτημα είναι μια σταθερή 220Ω· το ποτενσιόμετρο απλά έκανε την κατασκευή να δουλέψει με ό,τι είχα διαθέσιμο.

### Setup της Android app

1. **Android Studio → New Project → Empty Views Activity**, γλώσσα **Kotlin**, package `com.weather.dashboard`, min SDK **21**.
2. Ενεργοποίησε ViewBinding στο `app/build.gradle`:
   ```gradle
   android { buildFeatures { viewBinding true } }
   dependencies { implementation 'androidx.cardview:cardview:1.0.0' }
   ```
3. Πρόσθεσε permissions Bluetooth στο `AndroidManifest.xml`:
   ```xml
   <uses-permission android:name="android.permission.BLUETOOTH" android:maxSdkVersion="30" />
   <uses-permission android:name="android.permission.BLUETOOTH_ADMIN" android:maxSdkVersion="30" />
   <uses-permission android:name="android.permission.BLUETOOTH_CONNECT" />
   ```
4. Αντικατέστησε τα `MainActivity.kt` και `res/layout/activity_main.xml` με τα αρχεία στο [`android/`](android/).
5. **Pair** το HC-05 από Android Settings (PIN `1234` ή `0000`), τρέξε την app, πάτα **Connect**, διάλεξε το device.

### Οδηγίες χρήσης

1. **Τροφοδότησε το Arduino.** Η LCD δείχνει `IoT Weather / Starting...` και μετά ζωντανές μετρήσεις. Το module Bluetooth αναβοσβήνει γρήγορα (αζευγάρωτο ακόμα).
2. **Pair μία φορά** (μόνο την πρώτη): κινητό **Settings → Bluetooth → HC-05 → Pair**, PIN `1234`/`0000`.
3. **Άνοιξε την εφαρμογή**, δώσε τα permissions Bluetooth/Location, πάτα **Connect** και διάλεξε το ζευγαρωμένο module. Το status γίνεται **Connected** και ο πίνακας γεμίζει σε ~2 δευτ. (τα δεδομένα έρχονται κάθε 2 δευτ.).
4. **Διάβασε τον πίνακα:** θερμοκρασία °C/°F, φως %, κατάσταση (NIGHT → SUNNY), max/min, uptime. Η **κάρτα alert** εμφανίζεται αυτόματα — κόκκινη όταν `≥ 35 °C`, μπλε όταν `≤ 5 °C`.
5. **Κουμπιά πάνω στο κύκλωμα** (στο hardware, όχι στην app):
   - **SW1 (pin 2):** αλλάζει οθόνη στην LCD — Temp → Light → Max/Min → Uptime.
   - **SW2 (pin 3):** reset των max/min.
6. **Γρήγορο live test:** άγγιξε το TMP36 με το δάχτυλο → η θερμοκρασία ανεβαίνει στο κινητό. Κάλυψε το LDR → το φως % πέφτει και η κατάσταση γίνεται DARK/NIGHT.
7. **Αποσύνδεση:** ξαναπάτα το κουμπί (γίνεται **Disconnect**) ή κλείσε την app — το socket κλείνει μόνο του (`onDestroy`).

### Troubleshooting

| Πρόβλημα | Αιτία | Λύση |
|---|---|---|
| "Connection failed" | baud mismatch | HC-05 default 38400 → βάλε `btSerial.begin(38400)` |
| Δεν εμφανίζεται | δεν έγινε pair | Settings → Bluetooth → Pair |
| Δεν έρχονται data | parsing | έλεγξε το format στο Serial Monitor |
| Crash στο launch | λείπει permission | επιβεβαίωσε `BLUETOOTH_CONNECT` στο manifest |

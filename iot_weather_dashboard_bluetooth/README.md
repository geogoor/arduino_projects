# IoT Weather Dashboard (Bluetooth + Android App)

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![IoT Weather Dashboard](iot_weather_dashboard_bluetooth.jpg)

> _Add your build photo here as `iot_weather_dashboard_bluetooth.jpg` — Πρόσθεσε τη φωτογραφία της κατασκευής σου ως `iot_weather_dashboard_bluetooth.jpg`._

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

### Troubleshooting

| Πρόβλημα | Αιτία | Λύση |
|---|---|---|
| "Connection failed" | baud mismatch | HC-05 default 38400 → βάλε `btSerial.begin(38400)` |
| Δεν εμφανίζεται | δεν έγινε pair | Settings → Bluetooth → Pair |
| Δεν έρχονται data | parsing | έλεγξε το format στο Serial Monitor |
| Crash στο launch | λείπει permission | επιβεβαίωσε `BLUETOOTH_CONNECT` στο manifest |

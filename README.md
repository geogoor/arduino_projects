# 🛠️ Arduino Projects

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

Showcase Arduino Uno projects — home automation, sensors, IoT and security.
Each project lives in its own folder with a bilingual `README.md`, an `.ino` sketch and a build photo.

---

## English

### Projects

| Project | Description | Hardware | Status |
|---|---|---|---|
| [Smart Home Hub](smart_home_hub/) | Auto lights, climate alert, PIN alarm, door control | PIR, TMP36, LDR, Keypad 4×4, Servo, LCD | ✅ Built |
| [Parking Sensor](parking_sensor/) | Parking assistant with adjustable threshold (pot), IR confirm, bar graph & buzzer | HC-SR04, IR, pot, 2× LED, buzzer, LCD | ✅ Built |
| [Weather Dashboard](weather_dashboard/) | Weather station — min/max, trend, LED bar graph, CSV log | TMP36, LDR, 3× LED, 2× button, LCD | ✅ Built |
| [Motion Security System](motion_security_system/) | Alarm with sensor fusion (PIR+HC-SR04), PIN arm/disarm, BT alerts | PIR, HC-SR04, Keypad 4×4, HC-05, LCD | ✅ Built |
| [Sentry Turret](sentry_turret/) | Autonomous turret that scans & locks on a target — radar + auto-targeting | Servo, HC-SR04, PIR, LCD | ✅ Built |
| [IoT Weather Dashboard](iot_weather_dashboard_bluetooth/) | Full IoT stack: sensors → Arduino → HC-05 → native Android app (Kotlin) | HC-05, TMP36, LDR, LCD + Android | ✅ Built |
| [Vault Keypad v1](vault_keypad_v1/) | Insecure predecessor of v2: plaintext PIN, no lockout, press `C` to brute-force it | Keypad 4×4, Servo, LCD | ✅ Built |
| [Vault Keypad v2](vault_keypad_v2/) | Secure lock: hashed PIN, brute-force lockout, duress code, EEPROM | Keypad 4×4, Servo, LCD | ✅ Built |
| [Vault Keypad v3](vault_keypad_v3_bluetooth/) | Secure v2 + Bluetooth & Android app: challenge-response (PIN never sent), lockout applies to BT | Keypad 4×4, HC-05, Servo, LCD + Android | ✅ Built |
| [Flipper Zero DIY](flipper_zero_diy/) | Multi-tool: IR capture/replay + keypad 4×4 navigation | IR RX/TX, Keypad 4×4, LCD | 📐 Design only |

### Proof of Concept

Each built project has real build photos in its own `photos/` folder, shown at the top of the project's README. The circuits were wired on a breadboard and tested on real hardware.

> ⚠️ The security-related projects are **for educational use and authorized security testing only**.

---

## Ελληνικά

### Projects

| Project | Περιγραφή | Hardware | Status |
|---|---|---|---|
| [Smart Home Hub](smart_home_hub/) | Αυτόματα φώτα, climate alert, συναγερμός PIN, έλεγχος πόρτας | PIR, TMP36, LDR, Keypad 4×4, Servo, LCD | ✅ Built |
| [Parking Sensor](parking_sensor/) | Βοηθός παρκαρίσματος με ρυθμιζόμενο όριο (pot), IR επιβεβαίωση, bar graph & buzzer | HC-SR04, IR, pot, 2× LED, buzzer, LCD | ✅ Built |
| [Weather Dashboard](weather_dashboard/) | Σταθμός καιρού — min/max, trend, LED bar graph, CSV log | TMP36, LDR, 3× LED, 2× button, LCD | ✅ Built |
| [Motion Security System](motion_security_system/) | Συναγερμός με sensor fusion (PIR+HC-SR04), PIN arm/disarm, BT alerts | PIR, HC-SR04, Keypad 4×4, HC-05, LCD | ✅ Built |
| [Sentry Turret](sentry_turret/) | Αυτόνομος πύργος που σαρώνει & κλειδώνει σε στόχο — radar + auto-targeting | Servo, HC-SR04, PIR, LCD | ✅ Built |
| [IoT Weather Dashboard](iot_weather_dashboard_bluetooth/) | Πλήρες IoT stack: αισθητήρες → Arduino → HC-05 → native Android app (Kotlin) | HC-05, TMP36, LDR, LCD + Android | ✅ Built |
| [Vault Keypad v1](vault_keypad_v1/) | Ανασφαλής πρόγονος του v2: plaintext PIN, χωρίς lockout, πάτα `C` για brute-force | Keypad 4×4, Servo, LCD | ✅ Built |
| [Vault Keypad v2](vault_keypad_v2/) | Ασφαλής κλειδαριά: hashed PIN, brute-force lockout, duress code, EEPROM | Keypad 4×4, Servo, LCD | ✅ Built |
| [Vault Keypad v3](vault_keypad_v3_bluetooth/) | Ασφαλές v2 + Bluetooth & Android app: challenge-response (το PIN δεν φεύγει ποτέ), lockout και στο BT | Keypad 4×4, HC-05, Servo, LCD + Android | ✅ Built |
| [Flipper Zero DIY](flipper_zero_diy/) | Multi-tool: IR capture/replay + keypad 4×4 navigation | IR RX/TX, Keypad 4×4, LCD | 📐 Design only |

### Proof of Concept

Κάθε ολοκληρωμένο project έχει πραγματικές φωτογραφίες κατασκευής στον δικό του φάκελο `photos/`, που εμφανίζονται στην κορυφή του README του. Τα κυκλώματα στήθηκαν σε breadboard και δοκιμάστηκαν σε πραγματικό hardware.

> ⚠️ Τα security-related projects είναι **αποκλειστικά για εκπαιδευτικούς σκοπούς και authorized security testing**.

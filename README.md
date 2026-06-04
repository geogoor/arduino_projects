# 🛠️ Arduino Projects

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

Showcase Arduino Uno projects — home automation, sensors, IoT and security.
Each project lives in its own folder with a bilingual `README.md`, an `.ino` sketch and a build photo.

---

## English

### Projects

| Project | Description | Hardware |
|---|---|---|
| [Smart Home Hub](smart_home_hub/) | Auto lights, climate alert, PIN alarm, door control | PIR, TMP36, LDR, Keypad 4×4, Servo, LCD |
| [Parking Sensor](parking_sensor/) | Parking assistant with adjustable threshold (pot), IR confirm, bar graph & buzzer | HC-SR04, IR, pot, 2× LED, buzzer, LCD |
| [Weather Dashboard](weather_dashboard/) | Weather station — min/max, trend, LED bar graph, CSV log | TMP36, LDR, 3× LED, 2× button, LCD |
| [Motion Security System](motion_security_system/) | Alarm with sensor fusion (PIR+HC-SR04), PIN arm/disarm, BT alerts | PIR, HC-SR04, Keypad 4×4, HC-05, LCD |
| [Sentry Turret](sentry_turret/) | Autonomous turret that scans & locks on a target — radar + auto-targeting | Servo, HC-SR04, PIR, LCD |
| [IoT Weather Dashboard](iot_weather_dashboard_bluetooth/) | Full IoT stack: sensors → Arduino → HC-05 → native Android app (Kotlin) | HC-05, TMP36, LDR, LCD + Android |
| [Vault Keypad v2](vault_keypad_v2/) | Secure lock: hashed PIN, brute-force lockout, duress code, EEPROM | Keypad 4×4, Servo, LCD |
| [Flipper Zero DIY](flipper_zero_diy/) | Multi-tool: IR capture/replay + keypad 4×4 navigation | IR RX/TX, Keypad 4×4, LCD |

### Proof of Concept

Every project folder has a photo placeholder. To add yours:
1. Build the circuit and take a clear photo.
2. Save it inside the project folder as `<project_name>.jpg` (e.g. `parking_sensor/parking_sensor.jpg`).
3. It shows up automatically in that project's README.

> ⚠️ The security-related projects are **for educational use and authorized security testing only**.

---

## Ελληνικά

### Projects

| Project | Περιγραφή | Hardware |
|---|---|---|
| [Smart Home Hub](smart_home_hub/) | Αυτόματα φώτα, climate alert, συναγερμός PIN, έλεγχος πόρτας | PIR, TMP36, LDR, Keypad 4×4, Servo, LCD |
| [Parking Sensor](parking_sensor/) | Βοηθός παρκαρίσματος με ρυθμιζόμενο όριο (pot), IR επιβεβαίωση, bar graph & buzzer | HC-SR04, IR, pot, 2× LED, buzzer, LCD |
| [Weather Dashboard](weather_dashboard/) | Σταθμός καιρού — min/max, trend, LED bar graph, CSV log | TMP36, LDR, 3× LED, 2× button, LCD |
| [Motion Security System](motion_security_system/) | Συναγερμός με sensor fusion (PIR+HC-SR04), PIN arm/disarm, BT alerts | PIR, HC-SR04, Keypad 4×4, HC-05, LCD |
| [Sentry Turret](sentry_turret/) | Αυτόνομος πύργος που σαρώνει & κλειδώνει σε στόχο — radar + auto-targeting | Servo, HC-SR04, PIR, LCD |
| [IoT Weather Dashboard](iot_weather_dashboard_bluetooth/) | Πλήρες IoT stack: αισθητήρες → Arduino → HC-05 → native Android app (Kotlin) | HC-05, TMP36, LDR, LCD + Android |
| [Vault Keypad v2](vault_keypad_v2/) | Ασφαλής κλειδαριά: hashed PIN, brute-force lockout, duress code, EEPROM | Keypad 4×4, Servo, LCD |
| [Flipper Zero DIY](flipper_zero_diy/) | Multi-tool: IR capture/replay + keypad 4×4 navigation | IR RX/TX, Keypad 4×4, LCD |

### Proof of Concept

Κάθε φάκελος project έχει placeholder για φωτο. Για να βάλεις τη δική σου:
1. Στήσε το κύκλωμα και βγάλε καθαρή φωτογραφία.
2. Αποθήκευσέ τη μέσα στον φάκελο του project ως `<όνομα_project>.jpg` (π.χ. `parking_sensor/parking_sensor.jpg`).
3. Εμφανίζεται αυτόματα στο README του project.

> ⚠️ Τα security-related projects είναι **αποκλειστικά για εκπαιδευτικούς σκοπούς και authorized security testing**.

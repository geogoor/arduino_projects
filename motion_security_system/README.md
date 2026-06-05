# Motion Security System

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![Motion Security System](motion_security_system.jpg)

> 📸 **Proof of Concept:** the photos show the full alarm sequence on real hardware **plus the dedicated phone app** reacting live. As the "intruder" I used a **roll of masking tape** moved into the sensors' field — you can see it in the shots triggering the ENTRY/INTRUSION states while the app card turns orange then red.
>
> _Οι φωτογραφίες δείχνουν ολόκληρη την ακολουθία του συναγερμού σε πραγματικό hardware **μαζί με την αποκλειστική εφαρμογή** να αντιδρά ζωντανά. Ως «εισβολέα» χρησιμοποίησα έναν **ρόλο χαρτοταινίας** που τον κινούσα μέσα στο πεδίο των αισθητήρων — φαίνεται στις λήψεις να ενεργοποιεί τα ENTRY/INTRUSION ενώ η κάρτα της app γίνεται πορτοκαλί και μετά κόκκινη._

📄 **Code / Κώδικας:** [`motion_security_system.ino`](motion_security_system.ino)
📱 **Android app:** [`android/MainActivity.kt`](android/MainActivity.kt) · [`android/activity_main.xml`](android/activity_main.xml)

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

### How it works — step by step

The whole system is one **non-blocking state machine** (`switch(state)` in `loop()`), so the keypad, sensors, buzzer and Bluetooth all stay responsive at once. There are five states:

1. **DISARMED** — idle. Green LED on, no alarm. The LCD shows `DISARMED` and waits for `PIN + #`. Motion is ignored on purpose (you're home).
2. **EXITING** (exit delay, 10 s) — you typed `1234#`. The green LED **blinks** as a countdown and the app shows **ARMING…** (amber). This grace period lets you walk out without instantly tripping the alarm. At the end it auto-arms and sends `$STATUS,ARMED#`.
3. **ARMED** — watching. This is where the **sensor fusion** lives:
   - the PIR sets a timestamp `pirSeen` every time it sees heat/motion;
   - on each loop the HC-SR04 measures distance; if something is closer than `DETECT_CM` (100 cm) **and** the PIR fired within the last 2 s (`FUSION_WIN`), *both* sensors agree → go to ENTRY.
   - One sensor alone is **not** enough — that's what kills false alarms from a hot draft (PIR only) or a curtain swaying in range (HC-SR04 only).
4. **ENTRY** (entry delay, 8 s) — something got in. The buzzer gives **warning beeps** and the app shows **ENTRY** (orange). If you disarm with `1234#` in time → back to DISARMED, no alarm. If not → ALARM.
5. **ALARM** — intrusion. The buzzer **pulses** and the red LED **flashes** (non-blocking, every 300 ms), and `$ALERT,INTRUSION#` is pushed to the phone → the app card goes **red** with a 🚨 banner. Only the correct PIN clears it.

Every transition also pushes a Bluetooth frame, so the phone always mirrors the real state.

### Bluetooth protocol (to the phone)

The Arduino sends a framed message (`$...#`) on every **state change** — it is event-driven, not periodic:

| Frame | Meaning |
|---|---|
| `$STATUS,ARMING#` | exit delay started (you have 10 s to leave) |
| `$STATUS,ARMED#` | system armed |
| `$STATUS,ENTRY#` | something entered — entry delay running (8 s to disarm) |
| `$STATUS,DISARMED#` | disarmed with the PIN |
| `$ALERT,INTRUSION#` | **alarm fired** — no disarm in time |

### Companion Android app

A dedicated Kotlin app ([`android/`](android/), package `com.motion.security`) that receives the alerts over Bluetooth and shows them as a live security panel:

- a big **state card** that changes colour: DISARMED (grey) → ARMING (amber) → ARMED (green) → ENTRY (orange) → **INTRUSION (red)**
- a red **🚨 INTRUSION** banner when the alarm fires
- a timestamped **event log** of every frame received

Because the protocol is event-driven, the app shows the live state from the moment the next event arrives after you connect.

**Build & install** (Android Studio, or CLI): clone/open the project, then
```
./gradlew installDebug      # phone connected via USB, debugging on
```
Pair the HC-05 first from **Settings → Bluetooth** (PIN `1234`/`0000`), open the app, tap **Connect**, pick the module.

> No phone / no app handy? Any generic **"Serial Bluetooth Terminal"** app also shows the raw `$...#` frames.

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

### Πώς δουλεύει — βήμα προς βήμα

Όλο το σύστημα είναι ένα **non-blocking state machine** (`switch(state)` μέσα στο `loop()`), οπότε keypad, αισθητήρες, buzzer και Bluetooth παραμένουν όλα responsive ταυτόχρονα. Υπάρχουν πέντε καταστάσεις:

1. **DISARMED** — αδρανές. Πράσινο LED αναμμένο, χωρίς συναγερμό. Η LCD δείχνει `DISARMED` και περιμένει `PIN + #`. Η κίνηση **αγνοείται** σκόπιμα (είσαι σπίτι).
2. **EXITING** (exit delay, 10 δευτ.) — πάτησες `1234#`. Το πράσινο LED **αναβοσβήνει** ως αντίστροφη μέτρηση και η app δείχνει **ARMING…** (κίτρινο). Αυτό το grace period σε αφήνει να βγεις χωρίς να χτυπήσει αμέσως ο συναγερμός. Στο τέλος οπλίζει μόνο του και στέλνει `$STATUS,ARMED#`.
3. **ARMED** — επιτήρηση. Εδώ ζει το **sensor fusion**:
   - ο PIR βάζει timestamp `pirSeen` κάθε φορά που πιάνει θερμότητα/κίνηση·
   - σε κάθε loop το HC-SR04 μετράει απόσταση· αν κάτι είναι πιο κοντά από `DETECT_CM` (100 cm) **και** ο PIR ενεργοποιήθηκε μέσα στα τελευταία 2 δευτ. (`FUSION_WIN`), *και οι δύο* αισθητήρες συμφωνούν → πάει σε ENTRY.
   - Ένας αισθητήρας μόνος του **δεν** φτάνει — αυτό σκοτώνει τα false alarms από ζεστό ρεύμα αέρα (μόνο PIR) ή μια κουρτίνα που κουνιέται στην εμβέλεια (μόνο HC-SR04).
4. **ENTRY** (entry delay, 8 δευτ.) — κάτι μπήκε. Το buzzer κάνει **προειδοποιητικά beep** και η app δείχνει **ENTRY** (πορτοκαλί). Αν κάνεις disarm με `1234#` εγκαίρως → επιστροφή σε DISARMED, χωρίς συναγερμό. Αλλιώς → ALARM.
5. **ALARM** — εισβολή. Το buzzer **παλμώνει** και το κόκκινο LED **αναβοσβήνει** (non-blocking, κάθε 300 ms), και στέλνεται `$ALERT,INTRUSION#` στο κινητό → η κάρτα της app γίνεται **κόκκινη** με banner 🚨. Μόνο το σωστό PIN το καθαρίζει.

Κάθε μετάβαση στέλνει και ένα Bluetooth frame, οπότε το κινητό αντικατοπτρίζει πάντα την πραγματική κατάσταση.

### Bluetooth protocol (προς το κινητό)

Το Arduino στέλνει ένα framed μήνυμα (`$...#`) σε **κάθε αλλαγή κατάστασης** — event-driven, όχι περιοδικά:

| Frame | Σημασία |
|---|---|
| `$STATUS,ARMING#` | ξεκίνησε το exit delay (10 δευτ. για να φύγεις) |
| `$STATUS,ARMED#` | το σύστημα οπλίστηκε |
| `$STATUS,ENTRY#` | κάτι μπήκε — τρέχει entry delay (8 δευτ. για disarm) |
| `$STATUS,DISARMED#` | έγινε disarm με το PIN |
| `$ALERT,INTRUSION#` | **χτύπησε ο συναγερμός** — δεν έγινε disarm εγκαίρως |

### Συνοδευτική Android app

Αποκλειστική app σε Kotlin ([`android/`](android/), package `com.motion.security`) που λαμβάνει τα alerts μέσω Bluetooth και τα δείχνει ως ζωντανό πίνακα ασφαλείας:

- μεγάλη **κάρτα κατάστασης** που αλλάζει χρώμα: DISARMED (γκρι) → ARMING (κίτρινο) → ARMED (πράσινο) → ENTRY (πορτοκαλί) → **INTRUSION (κόκκινο)**
- κόκκινο banner **🚨 ΕΙΣΒΟΛΗ** όταν χτυπά ο συναγερμός
- **event log** με timestamp για κάθε frame που λαμβάνεται

Επειδή το protocol είναι event-driven, η app δείχνει τη ζωντανή κατάσταση από τη στιγμή που έρθει το επόμενο event μετά τη σύνδεση.

**Build & install** (Android Studio ή CLI): άνοιξε το project και
```
./gradlew installDebug      # κινητό συνδεμένο με USB, debugging ON
```
Πρώτα κάνε pair το HC-05 από **Settings → Bluetooth** (PIN `1234`/`0000`), άνοιξε την app, πάτα **Connect**, διάλεξε το module.

> Δεν έχεις κινητό/app πρόχειρα; Οποιαδήποτε generic **"Serial Bluetooth Terminal"** app δείχνει επίσης τα raw `$...#` frames.

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

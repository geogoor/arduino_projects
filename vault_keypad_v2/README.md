# Vault Keypad v2

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![Vault Keypad v2](vault_keypad_v2.jpg)

> _Add your build photo here as `vault_keypad_v2.jpg` — Πρόσθεσε τη φωτογραφία της κατασκευής σου ως `vault_keypad_v2.jpg`._

📄 **Code / Κώδικας:** [`vault_keypad_v2.ino`](vault_keypad_v2.ino)

---

## English

A real 4-digit PIN lock on a 4×4 keypad with "pro" security features: **hashed PIN** (no plaintext in memory), masked input, **brute-force lockout** after 3 wrong tries, **duress code** (a hidden code that unlocks normally but raises a silent alert), and **EEPROM** storage so the PIN survives a reset.

> Upgrade of an older combination lock that used 3 separate buttons.

### Why hash?

We never compare plaintext. We compute a small 16-bit FNV-style hash of the entered PIN and compare it to the stored hash, so the correct code never exists as plaintext — an educational take on "store the hash, not the password".

### Keys

| Key(s) | Action |
|---|---|
| `1234` + `#` | Enter PIN (default) |
| `A` | Lock |
| `D` | Change PIN (old → new) |
| `*` | Clear |
| duress `9119` | Unlocks but fires a silent alert |

3 wrong tries → 30 s lockout.

### Materials

| Part | Qty |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| Keypad 4×4 | 1 |
| Servo SG90 | 1 |
| Green / Red LED | 1 each |
| Passive buzzer | 1 |
| Resistor 220Ω | 2 |
| Breadboard + jumpers | ~26 |

**Libraries:** `LiquidCrystal_I2C`, `Keypad`, `Servo`, `EEPROM` (built-in)

### Wiring

```
Keypad 4×4   ROW1-4 -> 2,3,4,5    COL1-4 -> 6,7,8,9
Servo SG90   SIGNAL -> pin 11
Buzzer       + -> pin 12
Green LED    anode -> A2 -> 220Ω -> GND
Red LED      anode -> A3 -> 220Ω -> GND
LCD I2C      SDA -> A4   SCL -> A5
```

### Future ideas

- HC-05 alert: send `$ALERT,DURESS#` to the phone from `silentAlert()`.
- Add a per-device salt to the hash (shows why salts beat rainbow tables).
- Timing-safe compare (ties into the side-channel idea).

---

## Ελληνικά

Πραγματική κλειδαριά 4ψήφιου PIN σε keypad 4×4 με χαρακτηριστικά «επαγγελματικής» ασφάλειας: **hashed PIN** (όχι plaintext στη μνήμη), masked input, **brute-force lockout** μετά από 3 λάθη, **duress code** (κρυφός κωδικός που ξεκλειδώνει κανονικά αλλά σηκώνει silent alert), και αποθήκευση στην **EEPROM** ώστε ο κωδικός να επιβιώνει σε reset.

> Αναβάθμιση παλιότερης κλειδαριάς που είχε 3 ξεχωριστά κουμπιά.

### Γιατί hash;

Δεν συγκρίνουμε ποτέ plaintext. Υπολογίζουμε ένα μικρό 16-bit FNV hash του εισαγόμενου PIN και το συγκρίνουμε με το αποθηκευμένο — έτσι ο σωστός κωδικός δεν υπάρχει ποτέ ως καθαρό κείμενο. Εκπαιδευτική επίδειξη του «store the hash, not the password».

### Πλήκτρα

| Πλήκτρο(α) | Ενέργεια |
|---|---|
| `1234` + `#` | Εισαγωγή PIN (προεπιλογή) |
| `A` | Κλείδωμα |
| `D` | Αλλαγή PIN (παλιός → νέος) |
| `*` | Καθαρισμός |
| duress `9119` | Ξεκλειδώνει αλλά στέλνει silent alert |

3 λάθη → lockout 30 δευτ.

αναλυτικα :
Οδηγίες Χρήσης – Vault Keypad v2

Το Vault Keypad v2 είναι ένα ηλεκτρονικό σύστημα κλειδώματος με Arduino Uno, το οποίο λειτουργεί με πληκτρολόγιο 4×4 και PIN 4 ψηφίων. Για να ξεκλειδώσει το σύστημα, ο χρήστης εισάγει τον προσωπικό κωδικό στο keypad και επιβεβαιώνει με το πλήκτρο “#”. Αν ο κωδικός είναι σωστός, το σύστημα ενεργοποιεί το servo motor και ξεκλειδώνει τη μηχανική κλειδαριά, ενώ εμφανίζεται μήνυμα επιτυχίας στην οθόνη LCD και ενεργοποιείται η πράσινη ένδειξη.

Σε περίπτωση λάθους κωδικού, το σύστημα εμφανίζει μήνυμα αποτυχίας και αυξάνει τον μετρητή λαθών. Μετά από τρεις συνεχόμενες λανθασμένες προσπάθειες, ενεργοποιείται λειτουργία lockout διάρκειας 30 δευτερολέπτων, κατά την οποία το σύστημα δεν δέχεται νέες εισόδους.

Ο χρήστης μπορεί να καθαρίσει την εισαγωγή ανά πάσα στιγμή με το πλήκτρο “*”, ενώ το πλήκτρο “A” χρησιμοποιείται για χειροκίνητο κλείδωμα του συστήματος. Η αλλαγή PIN γίνεται μέσω του πλήκτρου “D”, όπου απαιτείται πρώτα η εισαγωγή του παλιού κωδικού και στη συνέχεια του νέου, ο οποίος αποθηκεύεται μόνιμα στην EEPROM ώστε να διατηρείται ακόμη και μετά από επανεκκίνηση.

Το σύστημα περιλαμβάνει επίσης λειτουργία duress code, όπου συγκεκριμένος κωδικός ξεκλειδώνει το σύστημα κανονικά αλλά ενεργοποιεί αθόρυβη ειδοποίηση για λόγους ασφαλείας. Συνολικά, το Vault Keypad v2 αποτελεί ένα ολοκληρωμένο σύστημα ελέγχου πρόσβασης με βασικές αρχές ασφάλειας, αποθήκευσης δεδομένων και ηλεκτρονικού ελέγχου μηχανισμών.

### Υλικά

| Υλικό | Ποσότητα |
|---|---|
| Arduino Uno | 1 |
| LCD 16x2 I2C | 1 |
| Keypad 4×4 | 1 |
| Servo SG90 | 1 |
| LED πράσινο / κόκκινο | 1 το καθένα |
| Passive buzzer | 1 |
| Αντίσταση 220Ω | 2 |
| Breadboard + jumpers | ~26 |

**Libraries:** `LiquidCrystal_I2C`, `Keypad`, `Servo`, `EEPROM` (built-in)

### Συνδεσμολογία

```
Keypad 4×4   ROW1-4 -> 2,3,4,5    COL1-4 -> 6,7,8,9
Servo SG90   SIGNAL -> pin 11
Buzzer       + -> pin 12
LED πράσινο  anode -> A2 -> 220Ω -> GND
LED κόκκινο  anode -> A3 -> 220Ω -> GND
LCD I2C      SDA -> A4   SCL -> A5
```

### Ιδέες για το μέλλον

- HC-05 alert: στείλε `$ALERT,DURESS#` στο κινητό από το `silentAlert()`.
- Per-device salt στο hash (δείχνει γιατί τα salts νικούν τα rainbow tables).
- Timing-safe compare (συνδέεται με την ιδέα του side-channel).

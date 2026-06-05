# Vault Keypad v1

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![Vault Keypad v1](vault_keypad_v1.jpg)

> _Add your build photo here as `vault_keypad_v1.jpg` — Πρόσθεσε τη φωτογραφία της κατασκευής σου ως `vault_keypad_v1.jpg`._

📄 **Code / Κώδικας:** [`vault_keypad_v1.ino`](vault_keypad_v1.ino)

---

## English

The **insecure** first version of the keypad lock — the "before" that [Vault Keypad v2](../vault_keypad_v2/) later fixes. A 4-digit PIN on a 4×4 keypad, set by the user at startup. It works… but it's full of holes, and that's the point: build it, break it, then learn why v2 does things differently.

> 🎓 Educational demo only. The brute-force here runs against your **own** lock to show the weakness.

### The vulnerabilities (by design)

| Weakness | What it means |
|---|---|
| **Plaintext PIN** | The code is kept in RAM as a plain `String` — no hashing. Anyone reading memory (or the code) sees it. |
| **PIN leaked over serial** | At startup the PIN is printed to the Serial Monitor (`[v1] PIN set to: ...`). |
| **No lockout / no delay** | Wrong PIN gives a cosmetic 300 ms flash, then lets you retry instantly. Unlimited attempts. |
| **Brute-forceable** | Only 10,000 combos (`0000`–`9999`). Press **`C`** and the Arduino cracks its own PIN in milliseconds and opens the lock. |
| **No persistence** | PIN lives only in RAM; a reset wipes it (here a "feature", but shows there's no real storage model). |

### v1 → v2: what we fixed

| | v1 (this) | [v2](../vault_keypad_v2/) |
|---|---|---|
| PIN storage | plaintext in RAM | **hashed** (FNV-style) in **EEPROM** |
| Wrong tries | unlimited, no delay | **3 tries → 30 s lockout** |
| Serial | prints the PIN | never prints the PIN |
| Extra | — | **duress code** → silent alert |
| Attack | `C` cracks it instantly | brute-force throttled by lockout |

### Keys

| Key(s) | Action |
|---|---|
| set at startup | Type 4 digits + `#` to define the PIN |
| `0`–`9` then `#` | Enter PIN to unlock (plaintext compare) |
| `A` | Lock |
| `C` | **Run the brute-force attack** (cracks & opens) |
| `*` | Clear |

### Materials

Same wiring and parts as v2 — only the firmware differs.

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

**Libraries:** `LiquidCrystal_I2C`, `Keypad`, `Servo`

### Wiring

```
Keypad 4×4   ROW1-4 -> 2,3,4,5    COL1-4 -> 6,7,8,9
Servo SG90   SIGNAL -> pin 11
Buzzer       + -> pin 12
Green LED    anode -> A2 -> 220Ω -> GND
Red LED      anode -> A3 -> 220Ω -> GND
LCD I2C      SDA -> A4   SCL -> A5
```

### Try it yourself

1. Upload, set a PIN (4 digits + `#`).
2. Open the Serial Monitor → notice the PIN is printed in the clear.
3. Lock with `A`, then press `C` and watch the LCD counter race through `0000…9999` until it **CRACKED** the code and the servo opens.
4. Now compare with [v2](../vault_keypad_v2/) and see why hashing + lockout kills this attack.

---

## Ελληνικά

Η **ανασφαλής** πρώτη έκδοση της κλειδαριάς — το «πριν» που αργότερα διορθώνει το [Vault Keypad v2](../vault_keypad_v2/). PIN 4 ψηφίων σε keypad 4×4, που το ορίζει ο χρήστης στην αρχή. Δουλεύει… αλλά είναι γεμάτο τρύπες, και αυτό ακριβώς είναι το νόημα: το φτιάχνεις, το σπας, και μετά καταλαβαίνεις γιατί το v2 τα κάνει αλλιώς.

> 🎓 Μόνο για εκπαιδευτικούς σκοπούς. Το brute-force τρέχει πάνω στη **δική σου** κλειδαριά για να δείξει την ευπάθεια.

### Οι ευπάθειες (επίτηδες)

| Ευπάθεια | Τι σημαίνει |
|---|---|
| **Plaintext PIN** | Ο κωδικός μένει στη RAM ως απλό `String` — χωρίς hashing. Όποιος δει τη μνήμη (ή τον κώδικα) τον βλέπει. |
| **Διαρροή στο serial** | Στην εκκίνηση το PIN τυπώνεται στο Serial Monitor (`[v1] PIN set to: ...`). |
| **Χωρίς lockout / delay** | Λάθος PIN δίνει ένα διακοσμητικό flash 300 ms και ξαναδοκιμάζεις αμέσως. Απεριόριστες προσπάθειες. |
| **Brute-forceable** | Μόνο 10.000 συνδυασμοί (`0000`–`9999`). Πάτα **`C`** και το Arduino σπάει το ίδιο του το PIN σε χιλιοστά και ανοίγει. |
| **Χωρίς αποθήκευση** | Το PIN ζει μόνο στη RAM· ένα reset το σβήνει (δείχνει ότι δεν υπάρχει πραγματικό μοντέλο αποθήκευσης). |

### v1 → v2: τι διορθώσαμε

| | v1 (εδώ) | [v2](../vault_keypad_v2/) |
|---|---|---|
| Αποθήκευση PIN | plaintext στη RAM | **hashed** (FNV) στην **EEPROM** |
| Λάθος προσπάθειες | απεριόριστες, χωρίς delay | **3 λάθη → lockout 30 δευτ.** |
| Serial | τυπώνει το PIN | δεν τυπώνει ποτέ το PIN |
| Επιπλέον | — | **duress code** → silent alert |
| Επίθεση | το `C` το σπάει ακαριαία | brute-force φρενάρει από το lockout |

### Πλήκτρα

| Πλήκτρο(α) | Ενέργεια |
|---|---|
| στην αρχή | Πληκτρολόγησε 4 ψηφία + `#` για να ορίσεις το PIN |
| `0`–`9` και `#` | Εισαγωγή PIN για ξεκλείδωμα (plaintext σύγκριση) |
| `A` | Κλείδωμα |
| `C` | **Εκτέλεση brute-force** (σπάει & ανοίγει) |
| `*` | Καθαρισμός |

### Υλικά

Ίδια συνδεσμολογία και υλικά με το v2 — αλλάζει μόνο το firmware.

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

**Libraries:** `LiquidCrystal_I2C`, `Keypad`, `Servo`

### Συνδεσμολογία

```
Keypad 4×4   ROW1-4 -> 2,3,4,5    COL1-4 -> 6,7,8,9
Servo SG90   SIGNAL -> pin 11
Buzzer       + -> pin 12
LED πράσινο  anode -> A2 -> 220Ω -> GND
LED κόκκινο  anode -> A3 -> 220Ω -> GND
LCD I2C      SDA -> A4   SCL -> A5
```

### Δοκίμασέ το

1. Ανέβασε τον κώδικα, όρισε PIN (4 ψηφία + `#`).
2. Άνοιξε το Serial Monitor → δες ότι το PIN τυπώνεται καθαρό.
3. Κλείδωσε με `A`, μετά πάτα `C` και δες τον μετρητή στην LCD να τρέχει `0000…9999` μέχρι να βρει τον κωδικό (**CRACKED**) και να ανοίξει το servo.
4. Τώρα σύγκρινε με το [v2](../vault_keypad_v2/) και δες γιατί hashing + lockout σκοτώνουν αυτή την επίθεση.

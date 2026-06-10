# Sentry Turret

**🇬🇧 [English](#english) · 🇬🇷 [Ελληνικά](#ελληνικά)**

![Sentry Turret](photos/1000027435.jpg)


> 📸 **PoC demo notes (visible in the photos):**
> - The "intruder" target is a **roll of masking tape** — you can see the turret having locked onto it at different angles across the shots.
> - DIY rig: the **servo is held in place with tape**, and the **PIR sits on a quick makeshift base** aimed at the area (away from the moving arm). Both are visible in the photos — function over finish.
>
> 📸 **Σημειώσεις PoC (φαίνονται στις φωτο):**
> - Ο «στόχος» είναι ένας **ρόλος χαρτοταινίας** — φαίνεται ο πύργος να έχει κλειδώσει πάνω του σε διαφορετικές θέσεις σε κάθε λήψη.
> - Πρόχειρες πατέντες: το **servo στερεώθηκε με ταινία**, και το **PIR κάθεται σε αυτοσχέδια βάση** στραμμένο στον χώρο (μακριά από τον κινούμενο βραχίονα). Και τα δύο φαίνονται στις φωτο — λειτουργία πάνω από αισθητική.

📄 **Code / Κώδικας:** [`sentry_turret.ino`](sentry_turret.ino)

---

## English

A servo-mounted HC-SR04 that **sweeps the area** in an arc. When the PIR catches motion, the turret **stops and locks onto the target**, lights a red LED + buzzer alarm, and prints the target's angle + distance over serial (target lock). Idle = green LED + slow sweep. The most photogenic project for a PoC video (it moves and beeps).

### How it works

```
BOOT: PIR warm-up (60s) → calibration sweep (records the empty scene) → SCANNING
IDLE (SCANNING): servo 0°→180°→0°, HC-SR04 measures each step (radar over serial), green LED
PIR motion → TARGET LOCK: stop, fast sweep, lock onto the closest NEW object,
             turn there, red LED + buzzer, hold 3s, then back to SCANNING
```

**Startup (important):** on boot the PIR needs ~60 s to settle, then the turret runs a **calibration sweep** that memorises the distance of the empty scene at every angle. **Keep the area clear during both steps** — anything standing there gets recorded as background and won't be detected later.

**Target acquisition:** when the PIR fires, the turret sweeps and locks onto the closest object that is **at least 25 cm closer than the calibrated background** (and within 3 m). This is what stops it from constantly locking onto fixed clutter like the LCD, the breadboard or a wall — it only reacts to something *new*. If nothing new appears it shows `No target` and resumes scanning.

**Tuning** (constants at the top of the sketch):

| Constant | Meaning | Default |
|---|---|---|
| `MARGIN` | how much closer than the background a target must be | 25 cm |
| `MAXR` | ignore anything farther than this | 300 cm |

Lower `MARGIN` (e.g. 15) to detect from farther; raise it if it false-locks.

> ⚠️ The HC-SR04 has a narrow ~15° beam and a human body is a poor ultrasonic reflector, so long-range detection is hit-or-miss. For a reliable demo, face it directly or hold a flat object (book/cardboard) — it bounces the sound back much better than a body does.

### Mounting

- **HC-SR04** on the servo arm ("eyes" forward) — the servo rotates it so it scans the arc.
- **PIR on a small fixed base**, aimed **outward at the area you're guarding** and **away from the moving servo arm and the cables**. If the PIR can see the sweeping arm or dangling wires, it false-triggers non-stop and locks onto random spots. A simple DIY stand that points the PIR at open space (and keeps the moving parts out of its view) fixes this completely.

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

### Build notes: problems we hit (and how we fixed them, one by one)

Getting this reliable took a few iterations — documenting them so future-me doesn't repeat the hunt:

1. **Random alarms, ignored real motion.** Out of the box it fired at nothing and didn't react when I waved my hand. Two causes: (a) the HC-SR501 needs ~30–60 s to settle after power-up, and (b) the PIR could "see" the sweeping servo arm and the dangling cables, so it triggered non-stop. → Added a **60 s warm-up** and a **~300 ms debounce** in code, and isolated the PIR with a tiny test sketch to confirm it actually worked.

2. **PIR placement (the real fix for the false triggers).** I moved the PIR onto a **small DIY base, aimed outward at the room and away from the moving arm and the wires**. Once the moving parts were out of its field of view, the random triggering stopped completely. Lesson: with a PIR, *where it looks* matters as much as the code.

3. **Locked onto the wrong thing.** Now motion was detected, but the turret only "found" me within ~20 cm; otherwise it snapped to the far right and locked onto the LCD/breadboard sitting there. Cause: it was locking onto the **nearest object in the whole sweep**, and my own build was the nearest thing. → Added a **calibration sweep at startup** that memorises the empty scene, so the turret now only locks onto objects **meaningfully closer than that background** (new arrivals). The fixed clutter is ignored.

**Result:** warm-up + debounce + good PIR placement killed the false alarms, and the calibration baseline made the targeting lock onto *me* instead of the furniture.

### Future ideas

- A Processing/Python radar that reads the `scan ...` serial and draws the sweep.
- A second servo (tilt) for a 2-axis turret.
- Continuous tracking (PID on the angle) instead of a 3 s lock.

---

## Ελληνικά

Servo-mounted HC-SR04 που **σαρώνει** τον χώρο σε τόξο. Όταν ο PIR πιάσει κίνηση, ο πύργος **σταματά και «κλειδώνει» στον στόχο**, ανάβει κόκκινο LED + buzzer alarm και τυπώνει στο serial τη γωνία + απόσταση του στόχου (target lock). Σε ηρεμία: πράσινο LED + αργό sweep. Το πιο θεαματικό project για PoC video (κινείται + ηχεί).

### Πώς δουλεύει

```
BOOT: PIR warm-up (60s) → calibration sweep (καταγράφει το άδειο σκηνικό) → SCANNING
IDLE (SCANNING): servo 0°→180°→0°, HC-SR04 μετράει σε κάθε βήμα (radar στο serial), πράσινο LED
PIR κίνηση → TARGET LOCK: stop, γρήγορο sweep, κλείδωμα στο πιο κοντινό ΝΕΟ αντικείμενο,
             στρίψε εκεί, κόκκινο LED + buzzer, κράτα 3s, μετά πίσω σε SCANNING
```

**Εκκίνηση (σημαντικό):** στο boot ο PIR θέλει ~60s να σταθεροποιηθεί, και μετά ο πύργος κάνει ένα **calibration sweep** που απομνημονεύει την απόσταση του άδειου σκηνικού σε κάθε γωνία. **Κράτα την περιοχή άδεια και στα δύο βήματα** — ό,τι στέκεται εκεί καταγράφεται ως φόντο και μετά δεν θα ανιχνεύεται.

**Target acquisition:** όταν χτυπήσει ο PIR, ο πύργος σαρώνει και κλειδώνει στο πιο κοντινό αντικείμενο που είναι **τουλάχιστον 25cm πιο κοντά από το φόντο που βαθμονόμησε** (και εντός 3μ). Αυτό είναι που τον σταματάει από το να κλειδώνει συνέχεια σε μόνιμα «σκουπίδια» όπως η LCD, το breadboard ή ένας τοίχος — αντιδρά μόνο σε κάτι *νέο*. Αν δεν εμφανιστεί κάτι νέο, δείχνει `No target` και συνεχίζει τη σάρωση.

**Ρύθμιση** (σταθερές στην αρχή του κώδικα):

| Σταθερά | Σημασία | Default |
|---|---|---|
| `MARGIN` | πόσο πιο κοντά από το φόντο πρέπει να είναι ο στόχος | 25 cm |
| `MAXR` | αγνόησε ό,τι είναι πιο μακριά από αυτό | 300 cm |

Μείωσε το `MARGIN` (π.χ. 15) για ανίχνευση από πιο μακριά· αύξησέ το αν κλειδώνει σε ψεύτικους στόχους.

> ⚠️ Το HC-SR04 έχει στενή δέσμη ~15° και ο άνθρωπος είναι κακός ανακλαστήρας υπερήχων, οπότε σε μεγάλη απόσταση η ανίχνευση είναι ασταθής. Για αξιόπιστο demo, στάσου μετωπικά ή κράτα ένα επίπεδο αντικείμενο (βιβλίο/χαρτόνι) — ανακλά τον ήχο πολύ καλύτερα από το σώμα.

### Mounting

- **HC-SR04** πάνω στον βραχίονα του servo («μάτια» μπροστά) — ο servo το περιστρέφει ώστε να σαρώνει το τόξο.
- **PIR σε μικρή σταθερή βάση**, στραμμένος **προς τα έξω, στην περιοχή που φυλάς**, και **μακριά από τον κινούμενο βραχίονα και τα καλώδια**. Αν ο PIR βλέπει τον σαρωτικό βραχίονα ή κρεμασμένα καλώδια, χτυπάει ασταμάτητα και κλειδώνει σε τυχαία σημεία. Μια απλή πρόχειρη βάση που στρέφει τον PIR σε ανοιχτό χώρο (κρατώντας τα κινούμενα μέρη εκτός του οπτικού του πεδίου) το λύνει εντελώς.

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

### Σημειώσεις κατασκευής: δυσκολίες που βρήκαμε (και πώς τις λύσαμε, μία-μία)

Για να γίνει αξιόπιστο χρειάστηκαν μερικές επαναλήψεις — τα γράφω για να μην ψάχνω ξανά τα ίδια:

1. **Τυχαίοι συναγερμοί, αγνοούσε την πραγματική κίνηση.** Στην αρχή χτυπούσε χωρίς λόγο και δεν αντιδρούσε όταν κουνούσα το χέρι. Δύο αιτίες: (α) το HC-SR501 θέλει ~30–60s να σταθεροποιηθεί μετά την τροφοδοσία, και (β) ο PIR «έβλεπε» τον σαρωτικό βραχίονα και τα κρεμασμένα καλώδια, οπότε χτυπούσε ασταμάτητα. → Πρόσθεσα **60s warm-up** και **~300ms debounce** στον κώδικα, και απομόνωσα το PIR με ένα μικρό test sketch για να επιβεβαιώσω ότι όντως δουλεύει.

2. **Θέση του PIR (η πραγματική λύση για τα false triggers).** Μετακίνησα τον PIR σε μια **μικρή πρόχειρη βάση, στραμμένο προς τον χώρο και μακριά από τον κινούμενο βραχίονα και τα καλώδια**. Μόλις τα κινούμενα μέρη βγήκαν από το οπτικό του πεδίο, το τυχαίο χτύπημα σταμάτησε εντελώς. Μάθημα: στον PIR, το *πού κοιτάει* μετράει όσο και ο κώδικας.

3. **Κλείδωνε σε λάθος στόχο.** Τώρα η κίνηση ανιχνευόταν, αλλά ο πύργος με «έβρισκε» μόνο μέχρι ~20cm· αλλιώς πήγαινε τέρμα δεξιά και κλείδωνε στην LCD/breadboard που ήταν εκεί. Αιτία: κλείδωνε στο **πιο κοντινό αντικείμενο όλης της σάρωσης**, και η ίδια μου η κατασκευή ήταν το πιο κοντινό. → Πρόσθεσα **calibration sweep στην εκκίνηση** που απομνημονεύει το άδειο σκηνικό, ώστε ο πύργος να κλειδώνει μόνο σε αντικείμενα **σαφώς πιο κοντά από αυτό το φόντο** (νέες παρουσίες). Τα μόνιμα «σκουπίδια» αγνοούνται.

**Αποτέλεσμα:** warm-up + debounce + σωστή θέση PIR έκοψαν τους ψεύτικους συναγερμούς, και η baseline βαθμονόμηση έκανε το targeting να κλειδώνει σε *μένα* αντί για τα έπιπλα.

### Ιδέες για το μέλλον

- Processing/Python radar που διαβάζει το serial `scan ...` και ζωγραφίζει το sweep.
- Δεύτερος servo (tilt) για 2-axis turret.
- Continuous tracking (PID στη γωνία) αντί για 3s lock.

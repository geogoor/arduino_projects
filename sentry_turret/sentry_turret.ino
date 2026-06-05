// ============================================
//  Sentry Turret  (with PIR warm-up + debounce)
//  A servo-mounted HC-SR04 sweeps the area like radar.
//  When the PIR sees motion, the turret stops, finds the
//  closest object (the target), locks onto it, and alarms.
//
//  IMPORTANT mounting note:
//   - HC-SR04 goes on the servo arm.
//   - PIR stays on the base, AIMED AWAY from the moving arm.
//     If the PIR can "see" the sweeping arm/wires it will
//     false-trigger non-stop and lock onto random spots.
// ============================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo pan;

const int PIR=2, TRIG=5, ECHO=6, LED_OK=7, LED_ALARM=8, BUZZER=4, SERVO=9;

int angle = 0, dir = 2;             // sweep position & step
unsigned long lastStep = 0;
unsigned long lockUntil = 0;        // millis() deadline for holding the lock
int lastPir = -1;                   // for printing PIR changes only

const int STEP  = 10;               // targeting sweep resolution (degrees)
const int NSCAN = 19;               // angles 0,10,...,180  => 19 values
long baseline[NSCAN];               // empty-scene distance per angle
const long MARGIN = 25;             // a target must be >=25cm closer than baseline
const long MAXR   = 300;            // ignore anything farther than 300cm

long distAt(int a) {                // turn to angle a, then measure
  pan.write(a); delay(120);         // give the servo time to get there
  digitalWrite(TRIG,LOW); delayMicroseconds(2);
  digitalWrite(TRIG,HIGH); delayMicroseconds(10); digitalWrite(TRIG,LOW);
  long d = pulseIn(ECHO,HIGH,30000);
  return d==0 ? 999 : d*0.0343/2;
}

// HC-SR501 needs ~30-60 s to settle after power-up; ignore it meanwhile.
void warmUp() {
  for (int t = 60; t > 0; t--) {
    lcd.clear(); lcd.print("PIR warm-up");
    lcd.setCursor(0,1); lcd.print(t); lcd.print("s keep clear ");
    Serial.print("warmup "); Serial.print(t); Serial.println("s");
    delay(1000);
  }
}

// Require the PIR to stay HIGH for ~300 ms -> kills random noise spikes.
bool motionConfirmed() {
  if (digitalRead(PIR) != HIGH) return false;
  for (int i = 0; i < 6; i++) {     // 6 x 50ms = 300ms of steady HIGH
    if (digitalRead(PIR) != HIGH) return false;
    delay(50);
  }
  return true;
}

// Record the empty scene so we can later ignore fixed clutter (LCD, walls, base).
// Keep the area clear while this runs.
void calibrate() {
  lcd.clear(); lcd.print("Calibrating...");
  lcd.setCursor(0,1); lcd.print("keep area clear");
  for (int i = 0; i < NSCAN; i++) {
    int a = i * STEP;
    long d1 = distAt(a), d2 = distAt(a);
    baseline[i] = min(d1, d2);        // nearest stable reading at this angle
    Serial.print("base "); Serial.print(a); Serial.print("deg "); Serial.println(baseline[i]);
  }
  pan.write(0);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIR,INPUT); pinMode(TRIG,OUTPUT); pinMode(ECHO,INPUT);
  pinMode(LED_OK,OUTPUT); pinMode(LED_ALARM,OUTPUT); pinMode(BUZZER,OUTPUT);
  pan.attach(SERVO); pan.write(0);
  lcd.init(); lcd.backlight();
  lcd.print("Sentry Turret");
  lcd.setCursor(0,1); lcd.print("Booting...");
  delay(1000);
  warmUp();                         // <-- wait for the PIR to stabilise
  calibrate();                      // <-- memorise the empty scene
  lcd.clear(); lcd.print("Sentry Turret");
  lcd.setCursor(0,1); lcd.print("Scanning...");
}

void acquireTarget() {
  // sweep, keep the closest NEW object: closer than the empty-scene baseline,
  // by at least MARGIN, and within MAXR. This ignores the fixed LCD/base/walls.
  int bestA = -1; long bestD = MAXR;
  for (int i = 0; i < NSCAN; i++) {
    int a = i * STEP;
    long d = distAt(a);
    Serial.print("scan "); Serial.print(a); Serial.print("deg "); Serial.print(d);
    Serial.print("cm (base "); Serial.print(baseline[i]); Serial.println(")");
    if (d < MAXR && d < baseline[i] - MARGIN && d < bestD) { bestD = d; bestA = a; }
  }

  if (bestA < 0) {                  // nothing new appeared -> no real target
    pan.write(90);
    lcd.clear(); lcd.print("No target");
    lcd.setCursor(0,1); lcd.print("(background only)");
    Serial.println(">>> no new target");
    delay(800);
    lcd.clear(); lcd.print("Sentry Turret");
    lcd.setCursor(0,1); lcd.print("Scanning...");
    return;
  }

  // lock onto the target
  pan.write(bestA);
  lockUntil = millis() + 3000;
  digitalWrite(LED_OK,LOW); digitalWrite(LED_ALARM,HIGH);
  lcd.clear(); lcd.print("TARGET LOCKED");
  lcd.setCursor(0,1);
  lcd.print(bestA); lcd.print("deg  "); lcd.print(bestD); lcd.print("cm");
  Serial.print(">>> TARGET @ "); Serial.print(bestA);
  Serial.print("deg, "); Serial.print(bestD); Serial.println("cm");
  // alarm beeps while locked
  while (millis() < lockUntil) {
    tone(BUZZER,2500,120); digitalWrite(LED_ALARM,(millis()/150)%2); delay(150);
  }
  noTone(BUZZER); digitalWrite(LED_ALARM,LOW);
  lcd.clear(); lcd.print("Sentry Turret");
  lcd.setCursor(0,1); lcd.print("Scanning...");
}

void loop() {
  // debug: print only when the PIR state changes, so it doesn't flood
  int p = digitalRead(PIR);
  if (p != lastPir) { Serial.println(p ? "PIR: HIGH" : "PIR: low"); lastPir = p; }

  // --- TARGET LOCK trigger (debounced) ---
  if (motionConfirmed()) { acquireTarget(); return; }

  // --- IDLE SCANNING (non-blocking sweep) ---
  digitalWrite(LED_OK,HIGH);
  if (millis()-lastStep >= 30) {
    lastStep = millis();
    angle += dir;
    if (angle>=180 || angle<=0) dir = -dir;   // bounce at the ends
    pan.write(angle);
  }
}

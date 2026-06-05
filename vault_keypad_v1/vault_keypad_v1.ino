// ============================================
//  Vault Keypad v1  (INSECURE - on purpose)
//  The "before" version that v2 later fixes.
//
//  Weaknesses, by design:
//    - PIN stored in PLAINTEXT in RAM (no hashing)
//    - no lockout and no delay after wrong tries
//    - press 'C' to run a live brute-force that cracks it
//
//  Same wiring as v2 - only the code changed.
// ============================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo lockServo;

const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int SERVO = 11, BUZZER = 12, LED_OK = A2, LED_BAD = A3;

String pin = "";        // the secret PIN - stored in PLAINTEXT (vulnerability!)
String input = "";
bool locked = true;

void setLock(bool state) {
  locked = state;
  lockServo.write(state ? 0 : 90);
  digitalWrite(LED_OK, state ? LOW : HIGH);
}

void msg(const char* l1, const char* l2) {
  lcd.clear(); lcd.setCursor(0,0); lcd.print(l1);
  lcd.setCursor(0,1); lcd.print(l2);
}

// Blocking read of exactly 4 digits, confirmed with '#'.
String readPin() {
  String s = "";
  while (true) {
    char k = keypad.getKey();
    if (!k) continue;
    if (k == '#') { if (s.length() == 4) return s; }
    else if (k == '*') { s = ""; lcd.setCursor(0,1); lcd.print("                "); }
    else if (k >= '0' && k <= '9' && s.length() < 4) {
      s += k; tone(BUZZER, 1800, 40);
      lcd.setCursor(0,1);
      for (uint8_t i = 0; i < s.length(); i++) lcd.print('*');
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_BAD, OUTPUT);
  lockServo.attach(SERVO);
  lcd.init(); lcd.backlight();

  // The user sets the PIN at startup; it is kept in plaintext.
  msg("Set 4-digit PIN", "");
  pin = readPin();
  setLock(true);
  msg("PIN set. Locked", "Enter PIN + #");
  Serial.print("[v1] PIN set to: "); Serial.println(pin);   // also leaked over serial!
}

void unlock(const char* l1) {
  setLock(false);
  tone(BUZZER, 2000, 200);
  msg(l1, "UNLOCKED");
}

// VULNERABILITY DEMO: no lockout + plaintext compare -> trivially crackable.
// Tries 0000..9999 until it matches, then opens the lock.
void bruteForce() {
  msg("BRUTE FORCE...", "");
  unsigned long start = millis();
  char cand[5];
  for (long i = 0; i <= 9999; i++) {
    sprintf(cand, "%04ld", i);
    if (i % 100 == 0) { lcd.setCursor(0,1); lcd.print(cand); lcd.print("      "); }  // live counter
    if (pin.equals(cand)) {
      unsigned long ms = millis() - start;
      Serial.print("[v1] CRACKED "); Serial.print(cand);
      Serial.print(" in "); Serial.print(i + 1); Serial.print(" tries, ");
      Serial.print(ms); Serial.println(" ms");
      lcd.clear();
      lcd.setCursor(0,0); lcd.print("CRACKED: "); lcd.print(cand);
      lcd.setCursor(0,1); lcd.print(i + 1); lcd.print(" try "); lcd.print(ms); lcd.print("ms");
      setLock(false);                         // attacker opens the lock
      digitalWrite(LED_BAD, HIGH);
      tone(BUZZER, 2500, 400);
      return;
    }
  }
}

void loop() {
  char k = keypad.getKey();
  if (!k) return;

  if (k == 'A') { setLock(true); tone(BUZZER, 1000, 150); msg("LOCKED", "Enter PIN + #"); input = ""; return; }
  if (k == 'C') { bruteForce(); input = ""; return; }        // press C = attack
  if (k == '*') { input = ""; lcd.setCursor(0,1); lcd.print("                "); return; }

  if (k >= '0' && k <= '9') {
    input += k; tone(BUZZER, 1800, 40);
    lcd.setCursor(0,1);
    for (uint8_t i = 0; i < input.length(); i++) lcd.print('*');
    return;
  }

  if (k == '#') {
    if (input.equals(pin)) {                  // plaintext compare, no delay, no lockout
      unlock("ACCESS GRANTED");
    } else {
      digitalWrite(LED_BAD, HIGH); tone(BUZZER, 400, 400);
      delay(300); digitalWrite(LED_BAD, LOW); // cosmetic flash only - NO security delay
      msg("ACCESS DENIED", "Try again");       // immediate retry, no penalty
    }
    input = "";
  }
}

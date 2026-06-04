// ============================================
//  Vault Keypad v2 - secure lock
//  4-digit PIN on a 4x4 keypad with "pro" security touches:
//   - hashed PIN (no plaintext in memory)
//   - brute-force lockout after 3 wrong tries
//   - duress code (unlocks normally but raises a silent alert)
//   - PIN stored in EEPROM (survives reset)
// ============================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>

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

// duress code: fixed here (in a real system this would be hashed/in EEPROM too)
const char* DURESS = "9119";

const int EE_FLAG = 0;     // byte: 0x42 = "a stored hash exists"
const int EE_HASH = 1;     // 2 bytes: the PIN hash
const char* DEFAULT_PIN = "1234";

String input = "";
int fails = 0;
bool locked = true;

// --- Simple 16-bit FNV-1a hash ---
uint16_t pinHash(const String& s) {
  uint16_t h = 0x811C;          // offset basis (16-bit variant)
  for (uint8_t i = 0; i < s.length(); i++) {
    h ^= (uint8_t)s[i];
    h *= 0x0101;                 // simple multiplier
  }
  return h;
}

void saveHash(uint16_t h) {
  EEPROM.update(EE_FLAG, 0x42);
  EEPROM.update(EE_HASH, h & 0xFF);
  EEPROM.update(EE_HASH + 1, (h >> 8) & 0xFF);
}

uint16_t loadHash() {
  if (EEPROM.read(EE_FLAG) != 0x42) {       // first boot -> set default
    saveHash(pinHash(DEFAULT_PIN));
  }
  return EEPROM.read(EE_HASH) | (EEPROM.read(EE_HASH + 1) << 8);
}

void setLock(bool state) {
  locked = state;
  lockServo.write(state ? 0 : 90);
  digitalWrite(LED_OK,  state ? LOW : HIGH);
}

void msg(const char* l1, const char* l2) {
  lcd.clear(); lcd.setCursor(0,0); lcd.print(l1);
  lcd.setCursor(0,1); lcd.print(l2);
}

void silentAlert() {
  // discreet: NO sound. Short red flashes at intervals.
  // (extension: send "$ALERT,DURESS#" to the phone over HC-05)
  for (int i = 0; i < 3; i++) { digitalWrite(LED_BAD, HIGH); delay(40); digitalWrite(LED_BAD, LOW); delay(400); }
}

void lockout() {
  msg("!! LOCKOUT !!", "Wait 30s...");
  for (int t = 30; t > 0; t--) {
    tone(BUZZER, 300, 60);
    lcd.setCursor(11, 1); lcd.print(t); lcd.print("s  ");
    delay(1000);
  }
  fails = 0;
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_BAD, OUTPUT);
  lockServo.attach(SERVO);
  lcd.init(); lcd.backlight();
  loadHash();                 // init EEPROM if needed
  setLock(true);
  msg("Vault Keypad v2", "Enter PIN + #");
}

void changePin() {
  msg("Old PIN + #", "");
  String oldp = "";
  while (true) { char k = keypad.getKey(); if (!k) continue;
    if (k=='#') break; if (k=='*'){oldp="";continue;} if (k>='0'&&k<='9') oldp+=k; }
  if (pinHash(oldp) != loadHash()) { tone(BUZZER,400,400); msg("Wrong old PIN","Cancelled"); delay(1500); return; }
  msg("New PIN + #", "");
  String newp = "";
  while (true) { char k = keypad.getKey(); if (!k) continue;
    if (k=='#') break; if (k=='*'){newp="";continue;} if (k>='0'&&k<='9') newp+=k; }
  if (newp.length() < 4) { msg("Too short","Min 4 digits"); delay(1500); return; }
  saveHash(pinHash(newp));
  tone(BUZZER,2000,200); msg("PIN changed","Saved to EEPROM"); delay(1500);
}

void loop() {
  char k = keypad.getKey();
  if (!k) return;

  if (k == 'A') { setLock(true); tone(BUZZER,1000,150); msg("LOCKED","Enter PIN + #"); return; }
  if (k == 'D') { changePin(); setLock(locked); msg(locked?"LOCKED":"UNLOCKED","Enter PIN + #"); return; }
  if (k == '*') { input = ""; lcd.setCursor(0,1); lcd.print("                "); return; }

  if (k >= '0' && k <= '9') {
    input += k; tone(BUZZER,1800,40);
    lcd.setCursor(0,1);
    for (uint8_t i=0;i<input.length();i++) lcd.print('*');
    return;
  }

  if (k == '#') {
    uint16_t h = pinHash(input);
    if (input == DURESS) {                 // duress: looks normal
      setLock(false); silentAlert();
      Serial.println("[ALERT] DURESS CODE USED");
      msg("UNLOCKED", "Welcome");          // NO alarm hint on the screen
      fails = 0;
    }
    else if (h == loadHash()) {            // correct PIN
      setLock(false); tone(BUZZER,2000,200);
      msg("ACCESS GRANTED", "UNLOCKED"); fails = 0;
    }
    else {                                  // wrong
      fails++; digitalWrite(LED_BAD,HIGH); tone(BUZZER,400,400);
      delay(400); digitalWrite(LED_BAD,LOW);
      if (fails >= 3) lockout();
      char buf[16]; sprintf(buf,"Wrong! (%d/3)",fails);
      msg("ACCESS DENIED", buf);
    }
    input = "";
  }
}

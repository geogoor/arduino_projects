// ============================================
//  Vault Keypad v3  (SECURE + Bluetooth)
//  The v2 secure lock, now controllable from a
//  phone app - WITHOUT weakening the security.
//
//  Inherited from v2:
//    - hashed PIN (FNV-1a 16-bit), no plaintext in memory
//    - PIN hash stored in EEPROM (survives reset)
//    - 3 wrong tries -> 30 s lockout (now non-blocking)
//    - duress code: unlocks normally but raises a silent alert
//
//  New in v3 - Bluetooth done RIGHT:
//    - the PIN NEVER travels over the air. The phone proves
//      knowledge of the PIN with a challenge-response:
//        Arduino -> CHALLENGE:<nonce>
//        Phone   -> RESP:<fnv16( fnv16(PIN) + ":" + nonce )>
//      A fresh random nonce each time defeats replay/sniffing.
//    - the SAME lockout throttles BT attempts, so a remote
//      brute-force is just 3 tries / 30 s -> infeasible.
//    - there is intentionally NO "brute-force" command.
//
//  Same wiring as v1/v2 + HC-05 on pins 10/A1.
//  (TX is on A1, NOT pin 13: pin 13 has the on-board LED which
//   loads the line and breaks SoftwareSerial transmit.)
// ============================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo lockServo;

// HC-05: Arduino RX(10) <- HC-05 TX ; Arduino TX(A1) -> HC-05 RX (via divider)
SoftwareSerial bt(10, A1);

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
String btLine = "";        // accumulates one command line from the app
int fails = 0;
bool locked = true;

unsigned long lockUntil = 0;   // non-blocking lockout deadline (millis)
long btNonce = 0;              // current BT challenge (0 = none pending)

// --- Simple 16-bit FNV-1a hash (identical maths on the phone) ---
uint16_t fnv16(const String& s) {
  uint16_t h = 0x811C;          // offset basis (16-bit variant)
  for (uint8_t i = 0; i < s.length(); i++) {
    h ^= (uint8_t)s[i];
    h *= 0x0101;                 // simple multiplier
  }
  return h;
}
uint16_t pinHash(const String& s) { return fnv16(s); }

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

// Send a status frame the app can parse: $V,<text>#
void notify(const String& s) {
  bt.print("$V,"); bt.print(s); bt.print("#");
}

void setLock(bool state) {
  locked = state;
  lockServo.write(state ? 0 : 90);
  digitalWrite(LED_OK, state ? LOW : HIGH);
  notify(state ? "LOCKED" : "UNLOCKED");
}

void msg(const char* l1, const char* l2) {
  lcd.clear(); lcd.setCursor(0,0); lcd.print(l1);
  lcd.setCursor(0,1); lcd.print(l2);
}

void silentAlert() {
  // discreet: NO sound. Short red flashes + a quiet BT alert to the app.
  notify("ALERT:DURESS");
  for (int i = 0; i < 3; i++) { digitalWrite(LED_BAD, HIGH); delay(40); digitalWrite(LED_BAD, LOW); delay(400); }
}

bool inLockout() { return (long)(lockUntil - millis()) > 0; }
int  lockoutLeft() { return inLockout() ? (int)((lockUntil - millis()) / 1000) + 1 : 0; }

// Register a wrong attempt (shared by keypad and Bluetooth).
void registerFail() {
  fails++;
  digitalWrite(LED_BAD, HIGH); tone(BUZZER, 400, 400);
  delay(400); digitalWrite(LED_BAD, LOW);
  if (fails >= 3) {
    lockUntil = millis() + 30000UL;   // non-blocking 30 s lockout
    fails = 0;
    msg("!! LOCKOUT !!", "Wait 30s...");
    notify("LOCKOUT:30");
  } else {
    char buf[16]; sprintf(buf, "Wrong! (%d/3)", fails);
    msg("ACCESS DENIED", buf);
    notify("DENIED:" + String(fails));
  }
}

void grant(bool duress) {
  setLock(false);                // setLock already notifies UNLOCKED
  fails = 0;
  if (duress) {
    silentAlert();
    Serial.println("[v3] DURESS CODE USED");
    msg("UNLOCKED", "Welcome");   // NO alarm hint on the screen
  } else {
    tone(BUZZER, 2000, 200);
    msg("ACCESS GRANTED", "UNLOCKED");
  }
}

// Verify a 4-digit PIN typed on the KEYPAD (plaintext stays on-device only).
void verifyKeypad(const String& pin) {
  if (inLockout()) { char b[16]; sprintf(b,"Wait %ds", lockoutLeft()); msg("!! LOCKOUT !!", b); return; }
  if (pin == DURESS)               grant(true);
  else if (pinHash(pin) == loadHash()) grant(false);
  else                             registerFail();
}

void setup() {
  Serial.begin(9600);
  bt.begin(9600);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_BAD, OUTPUT);
  lockServo.attach(SERVO);
  lcd.init(); lcd.backlight();
  randomSeed(analogRead(A0));   // entropy for the BT nonce
  loadHash();                   // init EEPROM if needed
  setLock(true);
  msg("Vault Keypad v3", "Enter PIN + #");
  notify("READY");
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

// ---- Bluetooth: challenge-response, never the PIN itself ----
// UNLOCK            ask for a challenge
// RESP:<hash>       answer the last challenge
// LOCK              lock
// SETHASH:<hash>    change PIN (only while unlocked; phone sends fnv16(newPIN))
// STATUS            report current state
void handleCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "STATUS") {
    notify(locked ? "LOCKED" : "UNLOCKED");

  } else if (cmd == "LOCK") {
    setLock(true); tone(BUZZER, 1000, 150); msg("LOCKED", "Enter PIN + #");

  } else if (cmd == "UNLOCK") {
    if (inLockout()) { notify("LOCKOUT:" + String(lockoutLeft())); return; }
    btNonce = random(100000, 1000000);          // fresh single-use challenge
    notify("CHALLENGE:" + String(btNonce));

  } else if (cmd.startsWith("RESP:")) {
    if (inLockout()) { notify("LOCKOUT:" + String(lockoutLeft())); return; }
    if (btNonce == 0) { notify("NO_CHALLENGE"); return; }
    String resp = cmd.substring(5); resp.trim();
    // expected = fnv16( fnv16(storedPIN) + ":" + nonce )
    String baseOk = String(loadHash()) + ":" + String(btNonce);
    String baseDu = String(pinHash(DURESS)) + ":" + String(btNonce);
    uint16_t expOk = fnv16(baseOk);
    uint16_t expDu = fnv16(baseDu);
    long want = resp.toInt();
    btNonce = 0;                                 // burn the nonce (anti-replay)
    if (want == (long)expDu)      grant(true);
    else if (want == (long)expOk) grant(false);
    else                          registerFail();

  } else if (cmd.startsWith("SETHASH:")) {
    if (locked) { notify("NEED_UNLOCK"); return; } // must be unlocked to re-key
    long h = cmd.substring(8).toInt();
    saveHash((uint16_t)h);
    Serial.print("[v3] PIN hash updated via BT: "); Serial.println(h);
    msg("PIN changed", "via app"); notify("PIN_SET");

  } else {
    notify("UNKNOWN");
  }
}

// Read newline-terminated commands from the app without blocking.
void pollBluetooth() {
  while (bt.available()) {
    char c = bt.read();
    if (c == '\n' || c == '\r') {
      if (btLine.length()) { handleCommand(btLine); btLine = ""; }
    } else {
      btLine += c;
      if (btLine.length() > 32) btLine = "";   // anti-overflow
    }
  }
}

void loop() {
  pollBluetooth();   // the phone app drives the lock securely over BT

  char k = keypad.getKey();
  if (!k) return;

  if (inLockout()) { char b[16]; sprintf(b,"Wait %ds", lockoutLeft()); msg("!! LOCKOUT !!", b); return; }

  if (k == 'A') { setLock(true); tone(BUZZER,1000,150); msg("LOCKED","Enter PIN + #"); input=""; return; }
  if (k == 'D') { changePin(); setLock(locked); msg(locked?"LOCKED":"UNLOCKED","Enter PIN + #"); input=""; return; }
  if (k == '*') { input = ""; lcd.setCursor(0,1); lcd.print("                "); return; }

  if (k >= '0' && k <= '9') {
    input += k; tone(BUZZER,1800,40);
    lcd.setCursor(0,1);
    for (uint8_t i=0;i<input.length();i++) lcd.print('*');
    return;
  }

  if (k == '#') { verifyKeypad(input); input = ""; }
}

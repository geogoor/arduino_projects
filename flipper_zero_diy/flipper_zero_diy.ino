// ============================================================
//  Flipper Zero DIY - Arduino Edition
//  IR capture / IR replay / IR library + info screens
//  Navigation with a 4x4 keypad (A/B/C and 1-6 shortcuts)
// ============================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int IR_RECV   = 11;
const int IR_SEND   = 3;
const int LED_GREEN = 8;
const int LED_RED   = 9;
const int BUZZER    = 6;

// ---- Keypad 4x4 ----
const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 4, 5, 7};
byte colPins[COLS] = {10, 12, 13, A0};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---- Modes ----
enum FlipperMode {
  MODE_IR_CAPTURE,
  MODE_IR_REPLAY,
  MODE_IR_LIBRARY,   // stored TV/AC power codes
  MODE_BADUSB_INFO,
  MODE_SUBGHZ_INFO,
  MODE_RFID_INFO,
  MODE_COUNT
};
FlipperMode mode = MODE_IR_CAPTURE;

const char* modeNames[] = {
  "IR CAPTURE",
  "IR REPLAY",
  "IR LIBRARY",
  "BAD USB",
  "SUB-GHz",
  "RFID"
};

// ---- IR storage ----
uint32_t capturedCode  = 0;
uint8_t  capturedProto = 0;
bool     hasCaptured   = false;

// ---- IR library (common TV/AC codes) ----
struct IREntry {
  const char* name;
  uint32_t    code;
  uint8_t     protocol;  // 1 = NEC
  uint8_t     address;
};

const IREntry IR_LIBRARY[] = {
  {"TV POWER (Samsung)",  0xE0E040BF, 1, 0x07},
  {"TV POWER (LG)",       0x20DF10EF, 1, 0x04},
  {"TV POWER (Sony)",     0xA8BCA, 1, 0x01},
  {"TV VOL+  (Samsung)",  0xE0E0E01F, 1, 0x07},
  {"TV VOL-  (Samsung)",  0xE0E0D02F, 1, 0x07},
  {"AC OFF   (generic)",  0xFF18E7, 1, 0x00},
};
const int IR_LIB_COUNT = 6;
int libIndex = 0;

void bip(int f, int ms) { tone(BUZZER, f, ms); delay(ms + 20); }

void showMode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("> "); lcd.print(modeNames[mode]);
  lcd.setCursor(0, 1);
  switch (mode) {
    case MODE_IR_CAPTURE:
      lcd.print(hasCaptured ? "Saved! B=replay" : "B=start capture");
      break;
    case MODE_IR_REPLAY:
      lcd.print(hasCaptured ? "B=send" : "Nothing captured");
      break;
    case MODE_IR_LIBRARY:
      lcd.print(IR_LIBRARY[libIndex].name);
      break;
    case MODE_BADUSB_INFO:
      lcd.print("Use Leonardo!");
      break;
    case MODE_SUBGHZ_INFO:
      lcd.print("Need HC-12/CC1101");
      break;
    case MODE_RFID_INFO:
      lcd.print("Need RC522 module");
      break;
  }
}

// ---- IR CAPTURE ----
void doCapture() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("IR CAPTURE");
  lcd.setCursor(0, 1); lcd.print("Point remote...");
  digitalWrite(LED_RED, HIGH);
  Serial.println("[FLIP] Waiting for IR signal (10s timeout)...");

  unsigned long start = millis();
  while (millis() - start < 10000) {
    if (IrReceiver.decode()) {
      capturedCode  = IrReceiver.decodedIRData.decodedRawData;
      capturedProto = (uint8_t)IrReceiver.decodedIRData.protocol;
      hasCaptured   = true;

      Serial.print("[FLIP] Captured! Code=0x"); Serial.print(capturedCode, HEX);
      Serial.print(" Proto="); Serial.println(capturedProto);

      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("CAPTURED!");
      lcd.setCursor(0, 1); lcd.print("0x"); lcd.print(capturedCode, HEX);
      bip(1200, 100);

      IrReceiver.resume();
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH); delay(200); digitalWrite(LED_GREEN, LOW);
      delay(2000);
      showMode();
      return;
    }
  }

  digitalWrite(LED_RED, LOW);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("No signal");
  lcd.setCursor(0, 1); lcd.print("Try again");
  bip(300, 300);
  delay(2000);
  showMode();
}

// ---- IR REPLAY ----
void doReplay() {
  if (!hasCaptured) { showMode(); return; }

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("SENDING...");
  lcd.setCursor(0, 1); lcd.print("0x"); lcd.print(capturedCode, HEX);
  digitalWrite(LED_RED, HIGH);

  IrSender.sendNEC(0xFF, (uint8_t)(capturedCode & 0xFF), 3);
  Serial.print("[FLIP] Sent 0x"); Serial.println(capturedCode, HEX);

  delay(300);
  digitalWrite(LED_RED, LOW);
  bip(900, 80);
  delay(1500);
  showMode();
}

// ---- IR LIBRARY ----
void doLibrarySend() {
  const IREntry& e = IR_LIBRARY[libIndex];
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("SENDING:");
  lcd.setCursor(0, 1); lcd.print(e.name);
  digitalWrite(LED_RED, HIGH);

  IrSender.sendNEC(e.address, (uint8_t)(e.code & 0xFF), 3);
  Serial.print("[FLIP] Library send: "); Serial.print(e.name);
  Serial.print(" code=0x"); Serial.println(e.code, HEX);

  delay(300);
  digitalWrite(LED_RED, LOW);
  bip(900, 80);
  delay(1500);
  showMode();
}

// ---- INFO SCREENS ----
void showBadUSBInfo() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("BadUSB = HID");
  lcd.setCursor(0, 1); lcd.print("Need Leonardo!");
  Serial.println("[FLIP] BadUSB: Arduino Leonardo/Pro Micro emulates a keyboard");
  Serial.println("[FLIP] Upload HID sketch -> plug in -> types commands automatically");
  Serial.println("[FLIP] Example: open Run, type powershell, download+execute payload");
  delay(3000);
  showMode();
}

void showSubGHzInfo() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Sub-GHz 433MHz");
  lcd.setCursor(0, 1); lcd.print("Need CC1101/HC-12");
  Serial.println("[FLIP] Sub-GHz: garage doors, car fobs, weather stations, doorbells");
  Serial.println("[FLIP] Modules: CC1101 (full), HC-12 (basic), RTL-SDR (receive only)");
  Serial.println("[FLIP] Frequencies: 315MHz (US), 433MHz (EU), 868MHz (EU IoT)");
  delay(3000);
  showMode();
}

void showRFIDInfo() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("RFID 125kHz");
  lcd.setCursor(0, 1); lcd.print("Need RC522 SPI");
  Serial.println("[FLIP] RFID: 125kHz (EM4100, HID Prox) - old access cards");
  Serial.println("[FLIP] NFC: 13.56MHz (Mifare Classic, DESFire) - modern cards");
  Serial.println("[FLIP] RC522 module + MFRC522 library reads/writes Mifare");
  delay(3000);
  showMode();
}

void setup() {
  Serial.begin(9600);
  lcd.init(); lcd.backlight();

  IrReceiver.begin(IR_RECV, ENABLE_LED_FEEDBACK);
  IrSender.begin(IR_SEND);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED,   OUTPUT);
  pinMode(BUZZER,    OUTPUT);

  lcd.setCursor(0, 0); lcd.print("FLIPPER ZERO DIY");
  lcd.setCursor(0, 1); lcd.print("Arduino Edition");
  delay(2000);

  showMode();
  Serial.println("[FLIP] Flipper Zero DIY ready");
  Serial.println("[FLIP] A=mode B=action C=next-in-list  1-6=jump to mode");
}

void loop() {
  char k = keypad.getKey();
  if (!k) return;

  // A: next mode
  if (k == 'A') {
    mode = (FlipperMode)(((int)mode + 1) % MODE_COUNT);
    libIndex = 0;
    bip(500, 30);
    showMode();
  }
  // B: action depending on the current mode
  else if (k == 'B') {
    switch (mode) {
      case MODE_IR_CAPTURE:  doCapture();      break;
      case MODE_IR_REPLAY:   doReplay();       break;
      case MODE_IR_LIBRARY:  doLibrarySend();  break;
      case MODE_BADUSB_INFO: showBadUSBInfo(); break;
      case MODE_SUBGHZ_INFO: showSubGHzInfo(); break;
      case MODE_RFID_INFO:   showRFIDInfo();   break;
    }
  }
  // C: next item in the IR library
  else if (k == 'C') {
    if (mode == MODE_IR_LIBRARY) {
      libIndex = (libIndex + 1) % IR_LIB_COUNT;
      bip(600, 30);
      showMode();
    }
  }
  // 1-6: jump directly to a mode
  else if (k >= '1' && k <= '6') {
    mode = (FlipperMode)(k - '1');
    libIndex = 0;
    bip(700, 30);
    showMode();
  }
}

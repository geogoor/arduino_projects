// ============================================
//  Smart Home Hub
//  Four subsystems on one board:
//   - auto lights (PIR + LDR)
//   - climate alert (TMP36)
//   - security alarm (keypad PIN arm/disarm)
//   - door control (servo)
//  Note: keypad uses 8 pins, so the LEDs live on A2/A3 (as digital).
//  Servo uses Timer1, tone() uses Timer2 - no conflict.
// ============================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo doorServo;

// ---------- Keypad 4x4 ----------
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

// ---------- Pins ----------
const int PIR    = 10;
const int SERVO  = 11;
const int BUZZER = 12;
const int LED_CLIMATE = 13;   // yellow
const int TMP36  = A0;
const int LDR    = A1;
const int LED_LIGHT = A2;     // green (used as digital)
const int LED_ALARM = A3;     // red (used as digital)

// ---------- Settings ----------
String PIN = "1234";                       // default code
const float TEMP_LIMIT = 28.0;             // °C climate threshold
const int   DARK_LEVEL = 350;              // LDR analogRead below this = dark
const unsigned long LIGHT_TIMEOUT = 8000;  // how long the light stays on (ms)

// ---------- State ----------
String input = "";
bool armed = false;
bool alarmActive = false;
bool manualLight = false;
bool doorOpen = false;

unsigned long lightUntil = 0;      // millis() deadline for the auto-light
unsigned long lastLCD = 0;
unsigned long lastBeep = 0;
unsigned long armGraceUntil = 0;   // exit delay: ignore motion right after arming
bool beepState = false;

const unsigned long ARM_GRACE = 8000;  // 8 s exit delay

void shortBeep() { tone(BUZZER, 1800, 80); }

void updateDoor(bool open) {
  doorOpen = open;
  doorServo.write(open ? 90 : 0);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_CLIMATE, OUTPUT);
  pinMode(LED_LIGHT, OUTPUT);
  pinMode(LED_ALARM, OUTPUT);

  doorServo.attach(SERVO);
  updateDoor(false);            // door closed at startup

  lcd.init();
  lcd.backlight();
  lcd.print("Smart Home Hub");
  lcd.setCursor(0, 1);
  lcd.print("Booting...");
  delay(1500);                  // let the PIR settle
  lcd.clear();
}

void handleKey(char k) {
  if (k >= '0' && k <= '9') {
    input += k;
    shortBeep();
    if (input.length() > 8) input = "";   // anti-overflow
  }
  else if (k == '*') {                     // clear
    input = "";
    shortBeep();
  }
  else if (k == '#') {                      // submit PIN -> arm/disarm
    if (input == PIN) {
      armed = !armed;
      alarmActive = false;                  // disarming also clears the alarm
      noTone(BUZZER);
      digitalWrite(LED_ALARM, LOW);
      if (armed) armGraceUntil = millis() + ARM_GRACE;  // ignore lingering motion
      tone(BUZZER, armed ? 2000 : 1000, 200);
    } else {
      tone(BUZZER, 400, 400);               // wrong PIN
    }
    input = "";
  }
  else if (k == 'A') { updateDoor(true);  shortBeep(); }   // open
  else if (k == 'B') { updateDoor(false); shortBeep(); }   // close
  else if (k == 'C') { manualLight = !manualLight; shortBeep(); }
}

void loop() {
  // ----- 1. Keypad -----
  char k = keypad.getKey();
  if (k) handleKey(k);

  // ----- 2. Sensors -----
  bool motion = digitalRead(PIR) == HIGH;
  int lightRaw = analogRead(LDR);
  float volts = analogRead(TMP36) * (5.0 / 1023.0);
  float tempC = (volts - 0.5) * 100.0;

  // ----- 3. SECURITY -----
  if (armed && motion && millis() >= armGraceUntil) alarmActive = true;

  if (alarmActive) {
    // alternating buzzer + flashing red (non-blocking)
    if (millis() - lastBeep >= 300) {
      lastBeep = millis();
      beepState = !beepState;
      digitalWrite(LED_ALARM, beepState);
      if (beepState) tone(BUZZER, 2500); else noTone(BUZZER);
    }
  } else {
    digitalWrite(LED_ALARM, LOW);
  }

  // ----- 4. AUTO LIGHTS (only when NOT armed) -----
  if (!armed && motion && lightRaw < DARK_LEVEL) {
    lightUntil = millis() + LIGHT_TIMEOUT;   // (re)trigger
  }
  bool autoLight = millis() < lightUntil;
  digitalWrite(LED_LIGHT, (autoLight || manualLight) ? HIGH : LOW);

  // ----- 5. CLIMATE -----
  digitalWrite(LED_CLIMATE, tempC > TEMP_LIMIT ? HIGH : LOW);

  // ----- 6. LCD (refresh every 250ms) -----
  if (millis() - lastLCD >= 250) {
    lastLCD = millis();
    int lightPct = map(lightRaw, 0, 1023, 0, 100);

    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print((int)tempC);
    lcd.print("C L:");
    lcd.print(lightPct);
    lcd.print("%  ");
    lcd.setCursor(15, 0);
    lcd.print(armed ? "A" : "D");          // Armed / Disarmed

    lcd.setCursor(0, 1);
    if (alarmActive)        lcd.print("!! ALARM !!     ");
    else if (input.length()) {
      lcd.print("PIN:");
      for (uint8_t i = 0; i < input.length(); i++) lcd.print('*');
      lcd.print("        ");
    }
    else if (tempC > TEMP_LIMIT) lcd.print("Climate: COOL!  ");
    else if (doorOpen)      lcd.print("Door: OPEN      ");
    else if (armed && millis() < armGraceUntil) {        // exit-delay countdown
      int s = (armGraceUntil - millis()) / 1000 + 1;
      lcd.print("ARMING "); lcd.print(s); lcd.print("s        ");
    }
    else if (armed)         lcd.print("ARMED - secure  ");
    else                    lcd.print("System Ready    ");
  }
}

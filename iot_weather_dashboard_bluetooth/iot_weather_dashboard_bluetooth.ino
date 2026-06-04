// ============================================================
//  IoT Weather Dashboard - Arduino side
//  TMP36 + LDR -> Arduino -> HC-05 Bluetooth -> Android app
//  Sends a framed CSV packet every 2 seconds:
//    $W,tempC,tempF,light%,COND,max,min,uptime,alert#
// ============================================================

#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial btSerial(10, 11);   // RX, TX
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int PIN_TMP   = A0;
const int PIN_LDR   = A1;
const int PIN_BUZZ  = 8;
const int PIN_LED_R = 4;
const int PIN_LED_Y = 5;
const int PIN_LED_G = 6;
const int PIN_SW1   = 2;
const int PIN_SW2   = 3;

const float TEMP_HIGH = 35.0;
const float TEMP_LOW  = 5.0;

float tempC, tempF;
int   lightPct;
float tempMax, tempMin;
int   lcdScreen = 0;

unsigned long lastSend = 0;
unsigned long lastLCD  = 0;
bool sw1Prev = HIGH, sw2Prev = HIGH;

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600); // HC-06 default = 9600, HC-05 default = 38400

  lcd.init();
  lcd.backlight();

  pinMode(PIN_BUZZ,  OUTPUT);
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_Y, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_SW1,   INPUT_PULLUP);
  pinMode(PIN_SW2,   INPUT_PULLUP);

  tempC = readTemp();
  tempMax = tempC;
  tempMin = tempC;

  lcd.setCursor(0, 0); lcd.print("IoT Weather");
  lcd.setCursor(0, 1); lcd.print("Starting...");
  delay(1500);
  lcd.clear();
}

void loop() {
  handleButtons();
  unsigned long now = millis();

  if (now - lastSend >= 2000) {
    lastSend = now;
    readSensors();
    sendBluetooth();
    updateLEDs();
  }

  if (now - lastLCD >= 500) {
    lastLCD = now;
    updateLCD();
  }
}

void readSensors() {
  tempC    = readTemp();
  tempF    = tempC * 9.0 / 5.0 + 32.0;
  lightPct = constrain(map(analogRead(PIN_LDR), 0, 1023, 0, 100), 0, 100);
  if (tempC > tempMax) tempMax = tempC;
  if (tempC < tempMin) tempMin = tempC;
}

void sendBluetooth() {
  int alert = 0;
  if (tempC >= TEMP_HIGH) alert = 1;
  else if (tempC <= TEMP_LOW) alert = 2;

  btSerial.print("$W,");
  btSerial.print(tempC,  1); btSerial.print(",");
  btSerial.print(tempF,  1); btSerial.print(",");
  btSerial.print(lightPct);  btSerial.print(",");
  btSerial.print(condition()); btSerial.print(",");
  btSerial.print(tempMax, 1); btSerial.print(",");
  btSerial.print(tempMin, 1); btSerial.print(",");
  btSerial.print(millis() / 1000); btSerial.print(",");
  btSerial.print(alert);
  btSerial.println("#");
}

void updateLEDs() {
  if (tempC >= TEMP_HIGH) {
    digitalWrite(PIN_LED_R, HIGH);
    digitalWrite(PIN_LED_Y, LOW);
    digitalWrite(PIN_LED_G, LOW);
    tone(PIN_BUZZ, 1000, 200);
  } else if (tempC <= TEMP_LOW) {
    digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_Y, HIGH);
    digitalWrite(PIN_LED_G, LOW);
    tone(PIN_BUZZ, 500, 200);
  } else {
    digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_Y, LOW);
    digitalWrite(PIN_LED_G, HIGH);
  }
}

void updateLCD() {
  lcd.clear();
  switch (lcdScreen) {
    case 0:
      lcd.setCursor(0,0); lcd.print("Temp: "); lcd.print(tempC,1); lcd.print("C");
      lcd.setCursor(0,1); lcd.print("     "); lcd.print(tempF,1); lcd.print("F");
      break;
    case 1:
      lcd.setCursor(0,0); lcd.print("Light: "); lcd.print(lightPct); lcd.print("%");
      lcd.setCursor(0,1); lcd.print(condition());
      break;
    case 2:
      lcd.setCursor(0,0); lcd.print("Max: "); lcd.print(tempMax,1); lcd.print("C");
      lcd.setCursor(0,1); lcd.print("Min: "); lcd.print(tempMin,1); lcd.print("C");
      break;
    case 3:
      lcd.setCursor(0,0); lcd.print("Up: "); lcd.print(millis()/1000); lcd.print("s");
      lcd.setCursor(0,1); lcd.print("BT: Active");
      break;
  }
}

void handleButtons() {
  bool sw1 = digitalRead(PIN_SW1);
  bool sw2 = digitalRead(PIN_SW2);

  if (sw1 == LOW && sw1Prev == HIGH) {     // button 1: cycle LCD screen
    lcdScreen = (lcdScreen + 1) % 4;
    delay(50);
  }
  if (sw2 == LOW && sw2Prev == HIGH) {     // button 2: reset min/max
    tempMax = tempC;
    tempMin = tempC;
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("Min/Max Reset");
    delay(800);
  }

  sw1Prev = sw1;
  sw2Prev = sw2;
}

float readTemp() {
  float v = analogRead(PIN_TMP) * (5.0 / 1023.0);
  return (v - 0.5) * 100.0;
}

String condition() {
  if (lightPct < 10) return "NIGHT";
  if (lightPct < 30) return "DARK";
  if (lightPct < 55) return "CLOUDY";
  if (lightPct < 80) return "BRIGHT";
  return "SUNNY";
}

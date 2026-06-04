// ============================================================
//  Weather Station Dashboard
//  TMP36 + LDR + LCD + LED bar + serial CSV log
//  Two buttons: toggle °C/°F and reset min/max
// ============================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int TMP_PIN   = A0;
const int LDR_PIN   = A1;
const int LED_GREEN = 5;
const int LED_YEL   = 6;
const int LED_RED   = 7;
const int SW_UNIT   = 2;
const int SW_RESET  = 3;

bool  showFahrenheit = false;
float tempMin =  999.0;
float tempMax = -999.0;
float lastTemp = 0;

bool sw1Was = false;
bool sw2Was = false;

unsigned long lastLog     = 0;
unsigned long lastDisplay = 0;

// Custom chars for the LCD
byte arrowUp[8]   = {0b00100,0b01110,0b11111,0b00100,0b00100,0b00100,0b00100,0b00000};
byte arrowDown[8] = {0b00100,0b00100,0b00100,0b00100,0b11111,0b01110,0b00100,0b00000};
byte arrowRight[8]= {0b00000,0b00100,0b00010,0b11111,0b00010,0b00100,0b00000,0b00000};
byte degreeC[8]   = {0b01100,0b10010,0b10010,0b01100,0b00000,0b00000,0b00000,0b00000};

float readTempC() {
  int raw = analogRead(TMP_PIN);
  float v = raw * (5.0 / 1023.0);
  return (v - 0.5) * 100.0;
}

int readLight() {
  return map(analogRead(LDR_PIN), 0, 1023, 0, 100);
}

void updateLEDs(int light) {
  digitalWrite(LED_GREEN, light > 10);
  digitalWrite(LED_YEL,   light > 40);
  digitalWrite(LED_RED,   light > 70);
}

// Trend: +1 = rising, -1 = falling, 0 = stable
int getTrend(float current, float prev) {
  if (current - prev >  0.3) return  1;
  if (current - prev < -0.3) return -1;
  return 0;
}

void updateDisplay(float tempC, int light) {
  float displayTemp = showFahrenheit ? (tempC * 9.0 / 5.0 + 32.0) : tempC;
  int   trend       = getTrend(tempC, lastTemp);

  lcd.clear();

  // Line 1: temperature + trend + unit
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(displayTemp, 1);
  lcd.print(showFahrenheit ? "F" : "C");
  lcd.print(" ");

  // Trend arrow
  if (trend > 0)      lcd.write(0);  // up
  else if (trend < 0) lcd.write(1);  // down
  else                lcd.write(2);  // right

  // Min/Max
  lcd.print(" ");
  float dispMax = showFahrenheit ? (tempMax * 9.0/5.0 + 32.0) : tempMax;
  lcd.print((int)dispMax);

  // Line 2: light level
  lcd.setCursor(0, 1);
  lcd.print("L:");
  lcd.print(light); lcd.print("% ");

  // Bar graph (8 chars)
  int bars = map(light, 0, 100, 0, 8);
  for (int i = 0; i < bars; i++)       lcd.print((char)255);
  for (int i = bars; i < 8; i++)       lcd.print('.');
}

void setup() {
  Serial.begin(9600);
  lcd.init(); lcd.backlight();

  // Custom chars
  lcd.createChar(0, arrowUp);
  lcd.createChar(1, arrowDown);
  lcd.createChar(2, arrowRight);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YEL,   OUTPUT);
  pinMode(LED_RED,   OUTPUT);
  pinMode(SW_UNIT,   INPUT_PULLUP);
  pinMode(SW_RESET,  INPUT_PULLUP);

  lcd.setCursor(0, 0); lcd.print("Weather Station");
  lcd.setCursor(0, 1); lcd.print("Dashboard v2.0");
  delay(2000);

  lastTemp = readTempC();

  // CSV header
  Serial.println("timestamp_ms,temp_c,temp_f,light_pct,temp_min,temp_max");
}

void loop() {
  float tempC = readTempC();
  int   light = readLight();

  // Min/Max tracking
  if (tempC < tempMin) tempMin = tempC;
  if (tempC > tempMax) tempMax = tempC;

  // Buttons
  bool sw1 = (digitalRead(SW_UNIT)  == LOW);
  bool sw2 = (digitalRead(SW_RESET) == LOW);

  if (sw1 && !sw1Was) {
    showFahrenheit = !showFahrenheit;
    Serial.println(showFahrenheit ? "[WS] Switched to F" : "[WS] Switched to C");
  }
  sw1Was = sw1;

  if (sw2 && !sw2Was) {
    tempMin = tempC;
    tempMax = tempC;
    Serial.println("[WS] Min/Max reset");
  }
  sw2Was = sw2;

  // LEDs
  updateLEDs(light);

  // Display update (every 500ms)
  if (millis() - lastDisplay > 500) {
    lastDisplay = millis();
    updateDisplay(tempC, light);
    lastTemp = tempC;
  }

  // CSV serial log (every 5s)
  if (millis() - lastLog > 5000) {
    lastLog = millis();
    float tempF = tempC * 9.0 / 5.0 + 32.0;
    Serial.print(millis()); Serial.print(",");
    Serial.print(tempC, 2); Serial.print(",");
    Serial.print(tempF, 2); Serial.print(",");
    Serial.print(light); Serial.print(",");
    Serial.print(tempMin, 2); Serial.print(",");
    Serial.println(tempMax, 2);
  }

  delay(100);
}

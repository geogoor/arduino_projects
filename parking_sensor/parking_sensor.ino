// ============================================
//  Parking Sensor - parking assistant
//  HC-SR04 + IR sensor + potentiometer + LCD + buzzer
//  The potentiometer sets the STOP threshold live (5-30 cm)
// ============================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int TRIG    = 7;
const int ECHO    = 6;
const int IR_PIN  = 2;
const int POT     = A0;
const int BUZZ    = 8;
const int LED_RED = 12;
const int LED_GRN = 13;

// full block character used for the bar graph
byte block[8] = {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};

long getDistance() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, 30000) * 0.034 / 2;
}

void setup() {
  pinMode(TRIG,    OUTPUT);
  pinMode(ECHO,    INPUT);
  pinMode(IR_PIN,  INPUT);
  pinMode(BUZZ,    OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GRN, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, block);
  lcd.setCursor(1, 0); lcd.print("Parking System");
  delay(1200);
  lcd.clear();
}

void loop() {
  long dist     = getDistance();
  bool irOK     = (digitalRead(IR_PIN) == LOW);   // LOW = object detected in front
  int  potVal   = analogRead(POT);
  int  stopDist = map(potVal, 0, 1023, 5, 30);    // STOP threshold, 5-30 cm

  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GRN, LOW);
  noTone(BUZZ);

  if (dist == 0 || dist > 150) {
    // free / nothing ahead
    digitalWrite(LED_GRN, HIGH);
    lcd.setCursor(0, 0); lcd.print("SPACE AVAILABLE ");
    lcd.setCursor(0, 1); lcd.print("Stop@:"); lcd.print(stopDist); lcd.print("cm IR:");
    lcd.print(irOK ? "Y" : "N"); lcd.print("  ");

  } else if (dist <= stopDist) {
    // STOP zone
    digitalWrite(LED_RED, HIGH);
    tone(BUZZ, 1500);
    lcd.setCursor(0, 0); lcd.print("!!   STOP   !!  ");
    lcd.setCursor(0, 1); lcd.print("Dist:"); lcd.print(dist); lcd.print("cm IR:");
    lcd.print(irOK ? "Y" : "N"); lcd.print("  ");

  } else {
    // approaching - bar graph + beep that speeds up as you get closer
    bool close = (dist < 50);
    close ? digitalWrite(LED_RED, HIGH) : digitalWrite(LED_GRN, HIGH);

    int beepDelay = map(dist, stopDist, 150, 80, 500);
    tone(BUZZ, 900, beepDelay / 2);

    int bars = map(dist, stopDist, 150, 0, 10);   // 10 blocks = far, 0 = close
    lcd.setCursor(0, 0);
    lcd.print("[");
    for (int i = 0; i < 10; i++) lcd.write(i < bars ? 0 : ' ');
    lcd.print("]");
    lcd.print(dist < 100 ? " " : ""); lcd.print(dist); lcd.print("cm");

    lcd.setCursor(0, 1);
    lcd.print("Stop@:"); lcd.print(stopDist); lcd.print("cm IR:");
    lcd.print(irOK ? "Y" : "N"); lcd.print("  ");
  }

  delay(180);
}

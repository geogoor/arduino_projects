// ============================================
//  Sentry Turret
//  A servo-mounted HC-SR04 sweeps the area like radar.
//  When the PIR sees motion, the turret stops, finds the
//  closest object (the target), locks onto it, and alarms.
//  Mount the HC-SR04 on the servo arm; keep the PIR on the base.
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

long distAt(int a) {                // turn to angle a, then measure
  pan.write(a); delay(120);         // give the servo time to get there
  digitalWrite(TRIG,LOW); delayMicroseconds(2);
  digitalWrite(TRIG,HIGH); delayMicroseconds(10); digitalWrite(TRIG,LOW);
  long d = pulseIn(ECHO,HIGH,30000);
  return d==0 ? 999 : d*0.0343/2;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIR,INPUT); pinMode(TRIG,OUTPUT); pinMode(ECHO,INPUT);
  pinMode(LED_OK,OUTPUT); pinMode(LED_ALARM,OUTPUT); pinMode(BUZZER,OUTPUT);
  pan.attach(SERVO); pan.write(0);
  lcd.init(); lcd.backlight();
  lcd.print("Sentry Turret");
  lcd.setCursor(0,1); lcd.print("Scanning...");
  delay(1500);
}

void acquireTarget() {
  // quick sweep, keep the angle with the smallest distance
  int bestA = 90; long bestD = 999;
  for (int a = 0; a <= 180; a += 10) {
    long d = distAt(a);
    Serial.print("scan "); Serial.print(a); Serial.print("deg "); Serial.print(d); Serial.println("cm");
    if (d < bestD) { bestD = d; bestA = a; }
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
  // --- TARGET LOCK trigger ---
  if (digitalRead(PIR)==HIGH) { acquireTarget(); return; }

  // --- IDLE SCANNING (non-blocking sweep) ---
  digitalWrite(LED_OK,HIGH);
  if (millis()-lastStep >= 30) {
    lastStep = millis();
    angle += dir;
    if (angle>=180 || angle<=0) dir = -dir;   // bounce at the ends
    pan.write(angle);
  }
}

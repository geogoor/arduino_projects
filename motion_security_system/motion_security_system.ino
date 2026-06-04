// ============================================
//  Motion Security System
//  Sensor fusion (PIR + HC-SR04) to cut false positives,
//  arm/disarm with a 4x4 keypad PIN, Bluetooth alerts (HC-05).
//  Pins: keypad 2-9, PIR 10, HC-SR04 11/12, buzzer 13,
//        LEDs A2/A3, HC-05 on A0/A1 (analog pins as digital).
// ============================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial bt(A0, A1);     // RX, TX

const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}
};
byte rowPins[ROWS] = {2,3,4,5};
byte colPins[COLS] = {6,7,8,9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int PIR=10, TRIG=11, ECHO=12, BUZZER=13, LED_OK=A2, LED_ALARM=A3;
const String PIN = "1234";
const int DETECT_CM = 100;                // HC-SR04 trigger threshold
const unsigned long EXIT_DELAY  = 10000;
const unsigned long ENTRY_DELAY = 8000;
const unsigned long FUSION_WIN  = 2000;   // PIR + SR04 must agree within 2s

enum State { DISARMED, EXITING, ARMED, ENTRY, ALARM };
State state = DISARMED;

String input = "";
unsigned long stateSince = 0;
unsigned long pirSeen = 0;
unsigned long lastBeep = 0;
bool beepOn = false;

long distCM() {
  digitalWrite(TRIG,LOW); delayMicroseconds(2);
  digitalWrite(TRIG,HIGH); delayMicroseconds(10); digitalWrite(TRIG,LOW);
  long d = pulseIn(ECHO,HIGH,30000);
  return d==0 ? 9999 : d*0.0343/2;
}

void go(State s) { state=s; stateSince=millis(); }

void btSend(const char* m) { bt.print(m); Serial.println(m); }

void showState() {
  lcd.setCursor(0,0);
  switch(state){
    case DISARMED: lcd.print("DISARMED        "); break;
    case EXITING:  lcd.print("Arming...       "); break;
    case ARMED:    lcd.print("ARMED - secure  "); break;
    case ENTRY:    lcd.print("ENTRY! PIN+#    "); break;
    case ALARM:    lcd.print("!! INTRUSION !! "); break;
  }
}

void setup() {
  Serial.begin(9600); bt.begin(9600);
  pinMode(PIR,INPUT); pinMode(TRIG,OUTPUT); pinMode(ECHO,INPUT);
  pinMode(BUZZER,OUTPUT); pinMode(LED_OK,OUTPUT); pinMode(LED_ALARM,OUTPUT);
  lcd.init(); lcd.backlight();
  go(DISARMED);
  lcd.setCursor(0,1); lcd.print("Enter PIN + #");
}

void handlePIN(char k) {
  if (k>='0'&&k<='9'){ input+=k; tone(BUZZER,1800,40); }
  else if (k=='*'){ input=""; }
  else if (k=='#'){
    if (input==PIN){
      tone(BUZZER, state==DISARMED?2000:1000, 200);
      if (state==DISARMED){ go(EXITING); btSend("$STATUS,ARMING#"); }
      else { go(DISARMED); digitalWrite(LED_ALARM,LOW); noTone(BUZZER); btSend("$STATUS,DISARMED#"); }
    } else tone(BUZZER,400,400);
    input="";
    lcd.setCursor(0,1); lcd.print("                ");
  }
}

void loop() {
  char k = keypad.getKey();
  if (k) handlePIN(k);

  bool motion = digitalRead(PIR)==HIGH;
  unsigned long now = millis();

  switch(state){
    case DISARMED:
      digitalWrite(LED_OK,HIGH); digitalWrite(LED_ALARM,LOW); noTone(BUZZER);
      break;

    case EXITING:
      digitalWrite(LED_OK, (now/300)%2);     // blink = countdown
      if (now-stateSince >= EXIT_DELAY){ go(ARMED); btSend("$STATUS,ARMED#"); }
      break;

    case ARMED: {
      digitalWrite(LED_OK,HIGH);
      if (motion) pirSeen = now;
      bool ranged = distCM() < DETECT_CM;
      // sensor fusion: both must fire within the window
      if (ranged && (now - pirSeen) < FUSION_WIN && pirSeen != 0){
        go(ENTRY); btSend("$STATUS,ENTRY#");
      }
      break;
    }

    case ENTRY:
      if (now-stateSince >= ENTRY_DELAY){ go(ALARM); btSend("$ALERT,INTRUSION#"); }
      else if ((now-lastBeep)>500){ lastBeep=now; tone(BUZZER,1200,80); }  // warning beeps
      break;

    case ALARM:
      if (now-lastBeep >= 300){
        lastBeep=now; beepOn=!beepOn;
        digitalWrite(LED_ALARM,beepOn);
        if (beepOn) tone(BUZZER,2500); else noTone(BUZZER);
      }
      break;
  }

  static unsigned long lastUI=0;
  if (now-lastUI > 300){ lastUI=now; showState(); }
}

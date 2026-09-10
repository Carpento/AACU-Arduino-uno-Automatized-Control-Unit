// C++ coode
// Macros
#define TRANSISTOR1_PIN 8
#define TRANSISTOR2_PIN 2
#define DOOR_SERVO1_PIN 9
#define DOOR_SERVO2_PIN 10

#define LOCK_SERVO1_PIN 11

#define TEMPERATURE_SENSOR1_PIN A0
#define TEMPERATURE_SENSOR2_PIN A1

#define PHOTOTRANSISTOR1_PIN A2
#define PHOTOTRANSISTOR2_PIN A3

#define BUZZER1_PIN 7

#define BUTTON1_PIN 4

// Costants
const int totalTime = 20000; 
const int maxWidth = 2412.6;
const int minWidth = 420; 
const double timePerAngle = 11.07;

// Variables
bool SERVO_STATE = 0;
bool SERVO_POWER_STATE = 0;
bool LOCK_SERVO_STATE = 0;

bool BUZZER_STATE = 0;
bool BUTTON_STATE = 0;

bool FAN_STATE = 0;

// FUNCTIONS FOR SERVO MOTORS
long calculateTime(int angle) {
  return (timePerAngle*angle)+minWidth;
}

bool open() {
  openLock();
  if (SERVO_POWER_STATE) {
   for (int i =0;i<=91;i++) {
     digitalWrite(DOOR_SERVO1_PIN, HIGH);
     digitalWrite(DOOR_SERVO2_PIN, HIGH);
     delayMicroseconds(calculateTime(i));
     digitalWrite(DOOR_SERVO1_PIN, LOW);
     digitalWrite(DOOR_SERVO2_PIN, LOW);
     delayMicroseconds(totalTime-calculateTime(i));
     delay(30);
   }
   SERVO_STATE = 1;
  }
  return SERVO_STATE;
}

bool close() {
  if (SERVO_POWER_STATE) {
   for (int i = 90;i>=0;i--) {
     digitalWrite(DOOR_SERVO1_PIN, HIGH);
     digitalWrite(DOOR_SERVO2_PIN, HIGH);
     delayMicroseconds(calculateTime(0));
     digitalWrite(DOOR_SERVO1_PIN, LOW);
     digitalWrite(DOOR_SERVO2_PIN, LOW);
     delayMicroseconds(totalTime-calculateTime(0));
     delay(30);
   }
   closeLock();
   SERVO_STATE = 0;
  }
  return SERVO_STATE;
}

void recalibreate() {
  activate();
  openLock();
  closeLock();
  openLock();
  open();
  close();
  open();
  close();
  closeLock();
  deactivate();
}

bool openLock() {
if (SERVO_POWER_STATE) {
   for (int i =0;i<=91;i++) {
     digitalWrite(LOCK_SERVO1_PIN, HIGH);
     delayMicroseconds(calculateTime(i));
     digitalWrite(LOCK_SERVO1_PIN, LOW);
     delayMicroseconds(totalTime-calculateTime(i));
     delay(30);
   }
   LOCK_SERVO_STATE = 1;
  }
  return LOCK_SERVO_STATE;
}

bool closeLock() {
  if (SERVO_POWER_STATE) {
   for (int i = 90;i>=0;i--) {
     digitalWrite(LOCK_SERVO1_PIN, HIGH);
     delayMicroseconds(calculateTime(0));
     digitalWrite(LOCK_SERVO1_PIN, LOW);
     delayMicroseconds(totalTime-calculateTime(0));
     delay(30);
   }
   LOCK_SERVO_STATE = 0;
  }
  return LOCK_SERVO_STATE;
}

// FUNCTIONS FOR TRANSISTORS
bool activate() {
  digitalWrite(TRANSISTOR1_PIN,HIGH);
  SERVO_POWER_STATE = 1;
  return SERVO_POWER_STATE;
}

bool deactivate() {
  digitalWrite(TRANSISTOR1_PIN,LOW);
  SERVO_POWER_STATE = 0;
  return SERVO_POWER_STATE;
}

// FUNCTIONS FOR TEMPERATURE SENSOR
double readTemperatureExternal() {
 double voltage = analogRead(TEMPERATURE_SENSOR2_PIN) * (5.0 / 1024.0);
 return (voltage - 0.5) * 100;
}

double readTemperatureInternal() {
 double voltage = analogRead(TEMPERATURE_SENSOR1_PIN) * (5.0 / 1024.0);
 return (voltage - 0.5) * 100;
}

// FUNCTION FOR ALARM
bool activateBuzzer() {
	digitalWrite(BUZZER1_PIN, HIGH);
    BUZZER_STATE=1;
}

bool deactivateBuzzer() {
	digitalWrite(BUZZER1_PIN, LOW);
    BUZZER_STATE=0;
}

// FUNCTION FOR CONTROL UNIT
bool checkMargin(int value1, int value2, double percentage) {
  int margin = value1*percentage;
  if (value2>=(value1-margin)&&value2<=(value1+margin)) {
    return 1;
  } else {
    return 0;
  }
}

int automatizeWithSolarCell() {
  double tempIn = readTemperatureInternal();
  while (tempIn<-5.0) {
    if (tempIn<-5.0) {
      tempIn = readTemperatureInternal();
    }
  }
  
  if (tempIn>35.0) {
  	digitalWrite(TRANSISTOR2_PIN,HIGH);
    FAN_STATE = 1;
  } else {
    if (FAN_STATE) {
      digitalWrite(TRANSISTOR2_PIN,LOW);
      FAN_STATE = 0;
    }
  }
  
  if (BUZZER_STATE&&digitalRead(BUTTON1_PIN)==1) {
  	deactivateBuzzer();
  }
  int solarCell1Value = analogRead(PHOTOTRANSISTOR1_PIN);
  int solarCell2Value = analogRead(PHOTOTRANSISTOR2_PIN);
  int average = (solarCell1Value+solarCell2Value)/2;
  if (average>=75) {
    if (!(checkMargin(solarCell1Value,solarCell2Value,0.35))) {
      activateBuzzer();
      Serial.println(average);
      return 1;
    }
  }
  
  if (!SERVO_POWER_STATE) {
   	activate();
    }
    if (average>=125) {
      if (!SERVO_STATE) {
        open();
      }
    } else {
      if (SERVO_STATE) {
        close();
      }
    }
    Serial.println(average);
  
}

// Setup
void setup() {
  Serial.begin(9600);
  Serial.flush();
  pinMode(DOOR_SERVO1_PIN, OUTPUT);
  pinMode(DOOR_SERVO2_PIN, OUTPUT);
  pinMode(LOCK_SERVO1_PIN, OUTPUT);
  pinMode(TRANSISTOR1_PIN, OUTPUT); 
  pinMode(TRANSISTOR2_PIN, OUTPUT);
  pinMode(PHOTOTRANSISTOR1_PIN, INPUT);
  pinMode(PHOTOTRANSISTOR2_PIN, INPUT);
  pinMode(BUTTON1_PIN, INPUT);
  delay(200);
}

// Loop
void loop() {
  automatizeWithSolarCell();
}

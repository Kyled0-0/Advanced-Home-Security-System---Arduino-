#include <Keypad.h>
#include <Servo.h>

//pin setup for every components
const byte ROWS = 4; 
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {7, 6, 5, 4};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int buttonPin = 13; // doorbell
const int pirPin = 12; // motion detector
const int ledGreenPin = 3; // door unlock
const int ledRedPin = 2; // alarm
const int servoPin = A5; // camera

const char correctPassword[] = "0307";

Servo myServo;

//states
volatile bool doorbellFlag = false;
volatile bool motionFlag = false;
volatile uint32_t timerCounter = 0; 
volatile bool servoSweepFlag = false;

bool lastDoorbellState = HIGH;
bool lastMotionState = LOW;
bool doorUnlocked = false;
int attemptCount = 0;
bool alarmTriggered = false;

//pci for doorbell and pir detector
ISR(PCINT2_vect) {
  doorbellFlag = true;
  motionFlag = true;
}

//timer for the servomotor
ISR(TIMER2_COMPA_vect) 
{
  timerCounter++;
  if (timerCounter >= 305)//~5 seconds
  {
    timerCounter = 0;
    servoSweepFlag = true;
  }
}


void setupTimer2()
{
  cli();
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;

  OCR2A = 255; 
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); 
  TIMSK2 |= (1 << OCIE2A);       
  sei();
}



void setup() {
  Serial.begin(9600);
  
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pirPin, INPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledRedPin, OUTPUT);
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledRedPin, LOW);
  myServo.attach(servoPin);
  myServo.write(90);
  
  PCICR |= (1 << PCIE2);                 
  PCMSK2 |= (1 << PCINT4) | (1 << PCINT5);
  
  setupTimer2();
}



void loop() {
  unsigned long currentMillis = millis();

  //motion detector and output
  if (motionFlag) {
    motionFlag = false;
    bool currentMotion = digitalRead(pirPin);
    if (currentMotion != lastMotionState) {
      lastMotionState = currentMotion;
      if (currentMotion == HIGH) {
        Serial.println("System: Motion detected!");
      }
    }
  }

  //doorbell and output
  if (doorbellFlag) {
    doorbellFlag = false;
    bool currentDoorbell = digitalRead(buttonPin);
    if (currentDoorbell != lastDoorbellState) {
      lastDoorbellState = currentDoorbell;
      if (currentDoorbell == LOW) {
        Serial.println("System: Doorbell rang!");
      }
    }
  }

  //keypad
  char key = keypad.getKey();
  if (key) 
  {
    static char enteredPassword[5];
    static int index = 0;

    if (key == 'C')
      index = 0;
    else
    {
      if (index < 4) enteredPassword[index++] = key;

      if (index == 4)
      {
        enteredPassword[index] = '\0';
        if (strcmp(enteredPassword, correctPassword) == 0)
        {
          Serial.println("System: Password correct. Door unlocked!");
          doorUnlocked = true;
          digitalWrite(ledGreenPin, HIGH);
          digitalWrite(ledRedPin, LOW);
          attemptCount = 0;
          alarmTriggered = false;
        } 
        else 
        {
          attemptCount++;
          Serial.print("System: Password incorrect. Attempt ");
          Serial.println(attemptCount);
          doorUnlocked = false;
          digitalWrite(ledGreenPin, LOW);

          if (attemptCount >= 3)
          {
            Serial.println("System: Passcode failed too many times! Alarm triggered!");
            alarmTriggered = true;
          }
        }

        index = 0;

      }
    }
  }

  //alarm armed
  static unsigned long previousLEDTime = 0;
  const unsigned long ledInterval = 150;
  if (alarmTriggered) 
  {
    if (currentMillis - previousLEDTime >= ledInterval) {
      previousLEDTime = currentMillis;
      digitalWrite(ledRedPin, !digitalRead(ledRedPin));
    }
  } 
  else 
  {
    digitalWrite(ledRedPin, LOW);
  }

  //servo auto sweeps
  if (servoSweepFlag)
  {
    servoSweepFlag = false;
    Serial.println("System: Timer triggered single camera sweep");

    for (int pos = 30; pos <= 120; pos++)
    {
      myServo.write(pos);
      delay(10);
    }
    for (int pos = 150; pos >= 90; pos--)
    {
      myServo.write(pos);
      delay(10);
    }
    myServo.write(90);
  }
}

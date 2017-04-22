/*
* Name:    Control.ino
* Created:  1/11/2017 12:15:59 PM
* Author: Connor Minielly & Jon Marcello
*
* Jon's Code preceded by //#J => ...
*
* Connor's code preceded by //#C => ...
*/

//#C => For I2C
#include <Wire.h>

//#C => I2C address of DAC when all pins of DAC go to ground.
const int DAC_address = 0x2C;


//#C => boolean value to represent a coasting state, another to tell the loop when to write values to the DAC.
bool coasting = false;
bool write = false;

int redLED = 5;
int dataLED = 6;

int demoModeSwitch = 7;
int raceModeSwitch = 8;
int brakeSwitch = 4;

int currentPin = A0;
int voltagePin = A1;
int speedPin = 3;
int currentPower = 0;
int previousPower = 0;

float currentCalibration = 0;

float wheelRadius = 0.254; // meters
const float pi = 3.14159265359;
double speed = 0.0;

bool brakeActive = false;

bool speedPinStep = false;
unsigned long previousMillis = 0;

long unsigned int AMP_TENTH_SECONDS = 0.0;
float AMP_HOURS = 0.0;
float current = 0.0;

int x = -1;

// Log Variables
double maxSpeed = 0.0;
float maxCurrent = 0.0;



void writeToDAC(int data) {
  if(data < 0 || data > 255 || data == currentPower) return;
  Wire.beginTransmission(DAC_address);
  Wire.write(0x00);
  Wire.write(data);
  Wire.endTransmission();
}

//#C => triggers when the arduino recives I2C communications from master.

//#C => basic set up of I2C communication line, runs first in practise.
void setup()
{

  pinMode(dataLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  pinMode(demoModeSwitch, INPUT);
  pinMode(raceModeSwitch, INPUT);
  pinMode(brakeSwitch, INPUT);

  pinMode(currentPin, INPUT);
  pinMode(speedPin, INPUT);

  Wire.begin(8);
  Wire.onReceive(speedChange);
  Wire.onRequest(sendStatus);

  Serial.begin(9600);

  writeToDAC(0);

  attachInterrupt(1, calculateSpeed, FALLING);

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 1561;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei();

  currentCalibration = analogRead(currentPin);
  //currentCalibration = 121.0;
}

void sendStatus() {
  String encodedMessage = String(getCarMode()) + " " + String(AMP_HOURS) + " " + String(current) + " " + String((int)round(speed)) + " " + String(currentPower) + " 0";
  Serial.println(encodedMessage);
  Wire.write(encodedMessage.c_str());
}

void speedChange(int bytes)
{
  x = Wire.read();
  Serial.println(x);
  currentPower = x;
}

int getCarMode() {
  //******
  // 0 = normal
  // 1 = race
  // 2 = demo
  //******
  if(digitalRead(demoModeSwitch)) {
    if(!brakeActive) {
      digitalWrite(redLED, HIGH);
    }
    return 2;
  } else if(!brakeActive) {
    digitalWrite(redLED, LOW);
  }

  if(digitalRead(raceModeSwitch)) {
    return 1;
  }

  return 0;

}

ISR(TIMER1_COMPA_vect) {
  current = calculateAmps();
  if(current < 0.0) current = 0.0;
  if(current > maxCurrent) maxCurrent = current;

  AMP_TENTH_SECONDS += current;
  AMP_HOURS = AMP_TENTH_SECONDS / 36000.0;
}

float calculateAmps() {
  return mapfloat(analogRead(currentPin), currentCalibration, 1024, 0, 200);
}

float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
 return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

float calculateVoltage() {
  return (analogRead(voltagePin) * 30.0) / 1024.0;
}

void calculateSpeed() {
    unsigned long currentMillis = millis();

    float rotationDistance = (2.0 * pi * wheelRadius) / 4.0;

    float velocity = ((rotationDistance / ((currentMillis - previousMillis) / 1000.0)) * 18.0) / 5.0;

    previousMillis = currentMillis;

  if(currentMillis - previousMillis > 500) {
    velocity = 0.0;
    previousMillis = currentMillis;
  }

  speed = velocity;

  if(speed > maxSpeed) maxSpeed = speed;
}

void loop()
{
  brakeActive = !digitalRead(brakeSwitch);

  if(brakeActive) {
    //previousPower = currentPower;
    writeToDAC(0);

    digitalWrite(redLED, HIGH);
    Serial.println("BRK");
    currentPower = 0;
  } else {
    //currentPower = previousPower;
    digitalWrite(redLED, LOW);
  }

  getCarMode();

  if(millis() - previousMillis > 750) {
    speed = 0;
  }

  if (x != -1) {
    x = map(x,0,100,0,255);

    if(!brakeActive) {
      writeToDAC(x);      
    }

    Serial.println(x);

    x = -1;
  }
}
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

//#C => volatile ints to represnt speedvalue and temp storage for speed in coast mode.
volatile int speedVal = 0;
volatile int speedMod = 0;
volatile int speedTmp = 0;

//#C => boolean value to represent a coasting state, another to tell the loop when to write values to the DAC.
bool coasting = false;
bool write = false;

//#C => triggers when the arduino recives I2C communications from master.
void speedChange(int bytes)
{
  digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(5, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
}

//#C => basic set up of I2C communication line, runs first in practise.
void setup()
{
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  Wire.begin(8);
  Wire.onReceive(speedChange);
}

void loop()
{
  digitalWrite(6, HIGH);   // turn the LED on (HIGH is the voltage level)
  Wire.beginTransmission(DAC_address);
  Wire.write(0x00);
  Wire.write(128);
  Wire.endTransmission();
  
  delay(1000);                       // wait for a second
  
  digitalWrite(6, LOW);    // turn the LED off by making the voltage LOW
  Wire.beginTransmission(DAC_address);
  Wire.write(0x00);
  Wire.write(0);
  Wire.endTransmission();
  
  delay(1000);
}

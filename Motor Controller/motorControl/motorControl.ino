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

int x = -1;
//#C => triggers when the arduino recives I2C communications from master.

//#C => basic set up of I2C communication line, runs first in practise.
void setup()
{
  pinMode(5, OUTPUT);

  Wire.begin(8);
  Wire.onReceive(speedChange);

  Serial.begin(9600);
}

void speedChange(int bytes)
{
  x = Wire.read();
}


void loop()
{
  
  if (x != -1) {
    x = map(x,0,100,0,255);
    Wire.beginTransmission(DAC_address);
    Wire.write(0x00);
    Wire.write(x);
    Wire.endTransmission();

    Serial.println(x);

//    digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
//    delay(50);                       // wait for a second
//    digitalWrite(5, LOW);    // turn the LED off by making the voltage LOW
//    delay(50);
    x = -1;
  }
  
}

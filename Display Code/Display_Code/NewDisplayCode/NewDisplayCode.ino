#include <Wire.h>
// LCD
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // initialize LCD pins lcd(rs, en, d4, d5, d6, d7)

byte fullRow = 0b11111;

byte x10[8] = {0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x07};   
byte x11[8] = {0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x1C};
byte x12[8] = {0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F};
byte x13[8] = {0x07,0x07,0x07,0x07,0x07,0x1F,0x1F,0x1F};
byte x14[8] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1F,0x1F,0x1F};
byte x15[8] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C};
byte x16[8] = {0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07};

byte topBar[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte row = 0, col = 0;

int pushButton1 = 6;  // speed up
int pushButton2 = 5;  // speed down
int pushButton3 = 13; // boost
int pushButton4 = 4;  // coast
int pushButton5 = 3;
const int intteruptPin = 2;
const int control_address = 8;

int currentPower = 0;
int previousPower = 0;
int powerInterval = 15;
int powerFineInterval = 5;
int powerFineStart = 45;
int powerRow = 0;
int powerCol = 15;
int displayPower = 0;

bool blink = true;

bool up = true;
int downBtn = 0;

int splashDelay = 500;
int splashNumDelay = 200;

//*****
// 0 = normal
// 1 = boost
// 2 = coast
//*****
int mode = 0;
int prev_mode = 0;
int boostSafteySpeed = 70;

//******
// 0 = normal
// 1 = race
// 2 = demo
//******
int carMode = 0;
int testModeMaxPower = 15;

bool resumePower = false;
int powerResumeFineInterval = 5;

int speed = 0;

unsigned long previousMillis = 0;
unsigned long previousMillisFlash = 0;
unsigned long previousMillisCount = 0;
unsigned long previousMillisResume = 0;


// delay for changing of display number, or else it is not possible to tell what speed you are going
const long interval = 300;
const long flashInterval = 300;
const long countInterval = 350;
const long resumeInterval = 100;

//*****************************************************************************************//
//                                      Initial Setup
//*****************************************************************************************//
void setup() {
  
  Serial.begin(9600);
  lcd.begin(20, 4);          // setup LCD  for 20 columns and 4 rows
  lcd.createChar(0, x10);     // digit piece
  lcd.createChar(1, x11);     // digit piece
  lcd.createChar(2, x12);     // digit piece
  lcd.createChar(3, x13);     // digit piece
  lcd.createChar(4, x14);     // digit piece
  lcd.createChar(5, x15);     // digit piece
  lcd.createChar(6, x16);     // digit piece

  lcd.createChar(7, topBar);    // bar

  pinMode(pushButton1, INPUT);
  pinMode(pushButton2, INPUT);
  pinMode(pushButton3, INPUT);
  pinMode(pushButton4, INPUT);
  pinMode(pushButton5, INPUT);
  pinMode(intteruptPin, INPUT_PULLUP);

  lcd.setCursor(0,0);

  splashScreen();

  drawNumber(currentPower, powerRow, powerCol);



  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  
  Wire.begin();
  for (byte i = 1; i < 120; i++)
  {
    Serial.println(i);
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
  
  //Wire.begin();
  //Wire.onReceive(wireRead);
  //attachInterrupt(digitalPinToInterrupt(intteruptPin), interruptFunc, RISING);

}

void interruptFunc() {
  Serial.println("int.");
}

void wireRead() {
  Wire.requestFrom(control_address, 5);

  while (Wire.available()) {          // slave may send less than requested
    int decodeMessage = Wire.read();  // receive a byte as character
  }

  mode = decodeMessage / 10000;
  batteryPercent = (decodeMessage - (mode * 10000)) / 100;
  speed = decodeMessage - (mode * 10000) - (batteryPercent * 100);

  Serial.println(decodeMessage);
}

byte wireWrite(int data, bool changePower, bool changeMode) {
  int dataEncode = (changePower * 1000) + (changeMode * 2000) + data;
  Serial.println(dataEncode);
  Wire.beginTransmission(control_address);
  Wire.write(data);
  Wire.endTransmission();
}

// Animate everything on the screen (useful to see if anything isn't working)
void splashScreen() {
  /**
  // Function that gets called during startup
  // used to make sure the display and characters all work properly
  //
  // This function has a set delay of splashNumDelay which can be changed
  // to fit the needs of the user, startup times will be longer with
  // a longer delay.  A second delay of splashDelay is called to give a
  // blank screen between the end of this function and the start of the
  // main loop
  //
  // The calculation for length of this function is (10 * splashNumDelay) + splashDelay.
  */
  int test_speed = 0;
  while(test_speed < 100) {    

    drawNumber(test_speed, 0, 0);

    lcd.setCursor(5, 2);
    lcd.print("km/h");

    drawBar(test_speed, 3, 10, 4, 3);

    drawNumber(test_speed, powerRow, powerCol);

    drawBattery(test_speed);

    if(test_speed < 55) {
      printMode(1, true);
    } else {
      printMode(2, true);
    }

    test_speed += 11;
    delay(splashNumDelay);
  }

  lcd.clear();
  delay(splashDelay);
}

void drawBar(int num, int r, int c, int height, int width) {
  /*
  // This function takes a percent and makes a nice bar out of it
  //
  // Args:
  //  num (int):    the percent between 0 and 100
  //  r (int):      the row (bottom left)
  //  c (int):      the column (left most)
  //  height (int): the height of the bar you would like to draw
  //  width (int):  the width of the bar
  //
  */
  byte topBar[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
  };

  int maxNum = height * 8;
  int lastPixel = map(num, 0, 100, 0, maxNum);
  int numOfBlocks = (int)(lastPixel / 8);
  int heightOfPixelBlock = lastPixel - (8 * numOfBlocks);

  int heightOfPixelBlockFlipped = map(heightOfPixelBlock, 0, 7, 7, 0);

  for(int i = 7; i > heightOfPixelBlockFlipped; i--) {
    topBar[i] = fullRow;
  }

  lcd.createChar(7, topBar);

  for(int i = 0; i < width; i++) {
    for(int j = 0; j < height; j++) {
      lcd.setCursor(c + i, r - j);
      if(j < numOfBlocks) {
      lcd.write(255);
      } else if(j > numOfBlocks) {
      lcd.print(" ");
      } else if(j == numOfBlocks){
      lcd.write(char(7));
      }
    }    
  }
}

int changePower(bool increase = true) {
  /*
  // Changes the power by a defined increment
  //
  // This function uses 4 variables, currentPower, powerFineInterval
  // powerInterval and powerFineStart.  When called it will determine
  // wether to increase the function by powerInterval (coarse adjustment)
  // or powerFineInterval (fine adjustment) based on the current power
  // level.
  //
  // Args:
  //  increase (bool): set direction to change power, increase or decrease
  //
  // Returns:
  //  int: The current power at which we are driving the motor
  */
  if(mode != 0) return currentPower;
  
  if(increase) {
    if(currentPower <= 100) {
      if(currentPower >= powerFineStart) {
        setPower(currentPower + powerFineInterval);
      } else {
        setPower(currentPower + powerInterval);
      }      
    }
  } else {
    if(currentPower - powerInterval >= 0) {
      if(currentPower > powerFineStart) {
        setPower(currentPower - powerFineInterval);
      } else if(currentPower == powerFineStart) {
        setPower(currentPower - powerInterval);
      } else if(currentPower - powerFineInterval <= powerFineStart && currentPower > powerFineStart - powerFineInterval) {
        setPower(currentPower - powerFineInterval);
      }  else {
        setPower(currentPower - powerInterval);
      }
    } else {
      currentPower = 0;
    }
  }
  
  return currentPower;
}

void setPower(int power) {
  /*
  // Sets the current power if all criteria are met
  //
  // Args:
  //  power (int): the power you want to set
  //
  */
  if(power < 0 || power > 100) return;

  if(carMode == 2 && power < testModeMaxPower) {
    currentPower = power;
  } else if(carMode == 2 && power >= testModeMaxPower) {
    currentPower = testModeMaxPower;
  } else {
    currentPower = power;
  }

  if(currentPower > 100) {
    Wire.write(wireWrite(100, true, false));
  } else {
    Wire.write(wireWrite(currentPower, true, false));
  }
  
}

//*****************************************************************************************//
//                                      MAIN LOOP
//*****************************************************************************************//
void loop() {
  unsigned long currentMillis = millis();

  wireRead();

  drawBar(speed, 3, 10, 4, 3);

  // draw current power
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    drawNumber(speed, 0, 0);
    if(speed < 100) {
      speed ++;
    } else {
      speed = 0;
    }
  }

  lcd.setCursor(5, 2);
  lcd.print("km/h");

  //************
  // BUTTONS
  // 1 = Speed Up
  // 2 = Speed Down
  // 3 = Coast
  // 4 = Boost
  // -3, -4 = Boost or Coast Up
  //***********
  int button = buttonPressed();

  // Serial.println(button);

  if(downBtn == 1) {
    if(currentMillis - previousMillisCount > countInterval) {
      previousMillisCount = currentMillis;
      displayPower = changePower();
    }
  } else if(downBtn == 2) {
    if(currentMillis - previousMillisCount > countInterval) {
      previousMillisCount = currentMillis;
      displayPower = changePower(false);
    }
  } else {
    previousMillisCount = currentMillis;
  }
  
  if(button == 1) {
    displayPower = changePower();
  } else if(button == 2) {
    displayPower = changePower(false);
  } else if(button == 3) {
    // Boost
    changeMode(1);
    displayPower = currentPower;
  } else if(button == 4) {
    changeMode(2);
    displayPower = currentPower;
  } else if(button == -3 || button == -4) {
    // Boast or Coast Up
    if(currentPower < boostSafteySpeed && button == -3) {
      changeMode(-1);
    } else {
      changeMode(0);
    }

    displayPower = currentPower;
  }

  if(currentMillis - previousMillisResume > resumeInterval && resumePower) {
      previousMillisResume = currentMillis;
      
      if(currentPower + powerResumeFineInterval < previousPower) {
        setPower(currentPower + powerResumeFineInterval);
      } else if(currentPower + powerResumeFineInterval >= previousPower) {
        setPower(previousPower);
        resumePower = false;
      }
      displayPower = currentPower;
    }

  //*****
  // MODES
  // 0 = normal
  // 1 = boost
  // 2 = coast
  //*****
  if (currentMillis - previousMillisFlash >= flashInterval && mode == 1) {
    previousMillisFlash = currentMillis;
    blink = !blink;

    if(!blink) {
      drawNumber(-1, powerRow, powerCol);
      printMode(0, true);
    } else {
      drawNumber(displayPower, powerRow, powerCol);
      printMode(mode, true);
    }
  }

  if(mode != 1) {
    drawNumber(displayPower, powerRow, powerCol);
  }

  drawBattery(speed);

  // Remove this line before production
  //delay(100);
}

//*****
// 0 = normal
// 1 = boost
// 2 = coast
//*****
void changeMode(int modeId) {
  /*
  // Changes the current mode of the car
  // 
  // Args:
  //  modeId (int): the id of the mode you would like the car to switch to
  //
  // 0 = normal
  // 1 = boost
  // 2 = coast
  */
  if(modeId < 0 || modeId > 2) return;
  prev_mode = mode;

  if(mode == modeId) {
    mode = 0;
  } else if(modeId == 1) {
    if(currentPower >= boostSafteySpeed) {
      mode = modeId;
    } else {
      printMode(-1, false);
      return;
    }
  } else {
    mode = modeId;
  }

  printMode(mode, false);
}

void printMode(int modeId, bool blink) {
  /*
  // If the car is in a special mode show it on the display
  //
  // Args:
  //  modeId (int): the mode you want to print
  //  blink (bool): if we are in a state of blink, don't show any numbers (used for BST)
  //
  */
  if(modeId < 0 || modeId > 2) return;
  lcd.setCursor(17, 5);
  
  switch(modeId) {
  case -1:
    break;
  case 0:
    if(!blink) {
      if(prev_mode == 2) {
        resumePower = true;
      } else {
        setPower(previousPower);
      }      
    }
    lcd.print("   ");
    break;
  case 1:
    if(!blink) {
      if(prev_mode != 2) {
        previousPower = currentPower;
      }
      setPower(100);
    }
    lcd.print("BST");
    break;
  case 2:
    if(!blink) {
    if(prev_mode != 1) {
      previousPower = currentPower;
    }
    setPower(0);
    }
    lcd.print("CST");
    break;
  }
}

void drawBattery(int percent) {
  /*
  // Draws the battery value to the display
  //
  // Args:
  //  percent (int): the current battery percent
  //
  // * NOTE * Function soon to be redundant
  */
  lcd.setCursor(0, 5);
  lcd.print("        ");
  lcd.setCursor(0, 5);
  lcd.print("Bat:" + String(percent) + "%");
}

void drawNumber(int num, byte r, byte c) {
  /*
  // Draws the large numbers
  //
  // Args:
  //  num (int): the number you want to draw between 0 and 99
  //  r (byte): the row
  //  c (byte): the column
  //
  */
  if(num >= 100) num = 99;
  String numStr = String(num);
  
  if(num == -1) {doNumber(20, r, c); doNumber(20, r, c + 3); return;}
  
  for(int i = 0; i < numStr.length(); i++) {
  if(numStr.length() == 1) {
    doNumber(0, r, c);
    c = c + 3;
  }
  doNumber(String(numStr.charAt(i)).toInt(), r, c + (i * 3));
  }
}

int buttonPressed() {
  /*
  // Returns what button is currently pressed or unpressed
  //
  // Returns:
  //  int:  The nunber of the pressed or relased button.
  //        (Only returns released for BST and CST buttons)
  */

  //Serial.println(up);
  
    if(!digitalRead(pushButton1) && downBtn == 1) {
    up = true;
    downBtn = 0;
    //return -1;
    } else if(!digitalRead(pushButton2) && downBtn == 2) {
    up = true;
    downBtn = 0;
    //return -2;
    } else if(!digitalRead(pushButton3) && downBtn == 3) {
    up = true;
    downBtn = 0;
    return -3;
    } else if(!digitalRead(pushButton4) && downBtn == 4) {
    up = true;
    downBtn = 0;
    return -4;
    } else if(!digitalRead(pushButton5) && downBtn == 5) {
    up = true;
    downBtn = 0;
    //return -5;
    }
  
  if(up) {
    if(digitalRead(pushButton1)) {
    up = false;
    downBtn = 1;
    return 1;
    } else if(digitalRead(pushButton2)) {
    up = false;
    downBtn = 2;
    return 2;
    } else if(digitalRead(pushButton3)) {
    up = false;
    downBtn = 3;
    return 3;
    } else if(digitalRead(pushButton4)) {
    up = false;
    downBtn = 4;
    return 4;
    } else if(digitalRead(pushButton5)) {
    up = false;
    downBtn = 5;
    return 5;
    }
  }
  return 0;
}





//*****************************************************************************************//
// doNumber: routine to position number 'num' starting top left at row 'r', column 'c'
void doNumber(byte num, byte r, byte c) {
  lcd.setCursor(c,r);
  switch(num) {
    case 0: lcd.write(byte(2)); lcd.write(byte(2)); 
        lcd.setCursor(c,r+1); lcd.write(byte(5)); lcd.write(byte(6)); 
        lcd.setCursor(c,r+2); lcd.write(byte(4)); lcd.write(byte(3)); break;
      
    case 1: lcd.write(byte(0)); lcd.write(byte(1)); 
        lcd.setCursor(c,r+1); lcd.print(" "); lcd.write(byte(5));
        lcd.setCursor(c,r+2); lcd.write(byte(0)); lcd.write(byte(4)); break;
      
    case 2: lcd.write(byte(2)); lcd.write(byte(2)); 
        lcd.setCursor(c,r+1); lcd.write(byte(2)); lcd.write(byte(3)); 
        lcd.setCursor(c,r+2); lcd.write(byte(4)); lcd.write(byte(2)); break;  
      
    case 3: lcd.write(byte(2)); lcd.write(byte(2)); 
        lcd.setCursor(c,r+1); lcd.write(byte(0)); lcd.write(byte(3)); 
        lcd.setCursor(c,r+2); lcd.write(byte(2)); lcd.write(byte(3)); break;  
      
    case 4: lcd.write(byte(1)); lcd.write(byte(0)); 
        lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(3)); 
        lcd.setCursor(c,r+2); lcd.print(" "); lcd.write(byte(6)); break;  
      
    case 5: lcd.write(byte(2)); lcd.write(byte(2)); 
        lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(2)); 
        lcd.setCursor(c,r+2); lcd.write(byte(2)); lcd.write(byte(3)); break;  
    case 6: lcd.write(byte(1)); lcd.print(" ");     
        lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(2)); 
        lcd.setCursor(c,r+2); lcd.write(byte(4)); lcd.write(byte(3)); break;  

    case 7: lcd.write(byte(2)); lcd.write(byte(2));
        lcd.setCursor(c,r+1); lcd.print(" "); lcd.write(byte(6)); 
        lcd.setCursor(c,r+2); lcd.print(" "); lcd.write(byte(6)); break;  

    case 8: lcd.write(byte(2)); lcd.write(byte(2)); 
        lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(3)); 
        lcd.setCursor(c,r+2); lcd.write(byte(4)); lcd.write(byte(3)); break;  
   
    case 9: lcd.write(byte(2)); lcd.write(byte(2)); 
        lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(3)); 
        lcd.setCursor(c,r+2); lcd.print(" "); lcd.write(byte(6)); break;  

    case 20: lcd.print(" "); lcd.print(" "); 
        lcd.setCursor(c,r+1); lcd.print(" "); lcd.print(" "); 
        lcd.setCursor(c,r+2); lcd.print(" "); lcd.print(" "); break;
  } 
}

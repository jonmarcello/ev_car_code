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
const int control_address = A4;

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

  Wire.begin();
  //Wire.onReceive(wireRead);
  //attachInterrupt(digitalPinToInterrupt(intteruptPin), interruptFunc, RISING);

}

void interruptFunc() {
  Serial.println("int.");
}

void wireRead(int bytes) {
  Serial.println(bytes);
}

byte wireWrite(int data, bool changePower, bool changeMode) {
  int dataEncode = (changePower * 1000) + (changeMode * 2000) + data;
  Serial.println(dataEncode);
  Wire.beginTransmission(control_address);
  Wire.write(dataEncode);
  Wire.endTransmission();
}

// Animate everything on the screen (useful to see if anything isn't working)
void splashScreen() {
  int test_speed = 0;
  while(test_speed + 11 < 100) {
    test_speed += 11;

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

    delay(200);
  }

  lcd.clear();
  delay(splashDelay);
}

void drawBar(int num, int r, int c, int height, int width) {
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
  
  if(currentPower > 100) {
    Wire.write(wireWrite(100, true, false));
  } else {
    Wire.write(wireWrite(currentPower, true, false));
  }
  
  return currentPower;
}

void setPower(int power) {
  if(power < 0 || power > 100) return;

  if(carMode == 2 && power < testModeMaxPower) {
    currentPower = power;
  } else if(carMode == 2 && power >= testModeMaxPower) {
    currentPower = testModeMaxPower;
  } else {
    currentPower = power;
  }
  
}

//*****************************************************************************************//
//                                      MAIN LOOP
//*****************************************************************************************//
void loop() {
  unsigned long currentMillis = millis();

  if(speed < 100) {
    speed ++;
  } else {
    speed = 0;
  }

  drawBar(speed, 3, 10, 4, 3);

  // draw current power
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    drawNumber(speed, 0, 0);
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
  delay(100);
}

//*****
// 0 = normal
// 1 = boost
// 2 = coast
//*****
void changeMode(int modeId) {
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
  lcd.setCursor(0, 5);
  lcd.print("        ");
  lcd.setCursor(0, 5);
  lcd.print("Bat:" + String(percent) + "%");
}

void drawNumber(int num, byte r, byte c) {
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

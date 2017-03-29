
// LCD
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // initialize LCD pins lcd(rs, en, d4, d5, d6, d7)

// OPTION 1 - for text on line 1
byte x10[8] = {0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x07};   
byte x11[8] = {0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x1C};
byte x12[8] = {0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F};
byte x13[8] = {0x07,0x07,0x07,0x07,0x07,0x1F,0x1F,0x1F};
byte x14[8] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1F,0x1F,0x1F};
byte x15[8] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C};
byte x16[8] = {0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07};
byte x17[8] = {0x00,0x00,0x0E,0x0E,0x0E,0x00,0x00,0x00};


byte row = 0,col = 0;

int pushButton6 = 6;
int pushButton5 = 5;
int pushButton4 = 4;
int pushButton13 = 13;
int pushButton3 = 3;


//*****************************************************************************************//
//                                      Initial Setup
//*****************************************************************************************//
void setup() {
  
    // Serial.begin(9600);
    lcd.begin(20, 4);                            // setup LCD  for 20 columns and 4 rows
    lcd.createChar(0, x10);                      // digit piece
    lcd.createChar(1, x11);                      // digit piece
    lcd.createChar(2, x12);                      // digit piece
    lcd.createChar(3, x13);                      // digit piece
    lcd.createChar(4, x14);                      // digit piece
    lcd.createChar(5, x15);                      // digit piece
    lcd.createChar(6, x16);                      // digit piece
    lcd.createChar(7, x17);                      // digit piece (colon)
    
    pinMode(pushButton6, INPUT);
    pinMode(pushButton5, INPUT);
    pinMode(pushButton4, INPUT);
    pinMode(pushButton13, INPUT);
    pinMode(pushButton3, INPUT);
    
    lcd.setCursor(0,0);
        
}

//*****************************************************************************************//
//                                      MAIN LOOP
//*****************************************************************************************//
int speed = 0;

unsigned long previousMillis = 0;

// delay for changing of display number, or else it is not possible to tell what speed you are going
const long interval = 300;

void loop() {                   // run over and over again
  //lcd.clear();
  unsigned long currentMillis = millis();
  
  if(speed < 99) {
    speed ++;
  } else {
    speed = 0;
  }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    drawSpeed(speed, 0, 0);
  }

  //lcd.setCursor((String(speed).length() * 2) + 1, 2);
  lcd.setCursor(5, 2);
  lcd.print("km/h");

  delay(100);
}

void drawSpeed(int num, byte r, byte c) {
  String numStr = String(num);
  for(int i = 0; i < numStr.length(); i++) {
    if(numStr.length() == 1) {
      // doNumber(0, r, c + (i * 3));
      c = c + 3;
    }
    doNumber(String(numStr.charAt(i)).toInt(), r, c + (i * 3));
  }
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

       case 11: lcd.setCursor(c,r+1); lcd.write(byte(7)); lcd.setCursor(c,r+2); lcd.write(byte(7)); break; 
    } 
}

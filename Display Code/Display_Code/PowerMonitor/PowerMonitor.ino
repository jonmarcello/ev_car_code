#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// define the good stuff
#define I2C_ADDR    0x3F
#define BACKLIGHT_PIN 3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

// define lcd
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

byte customChar[2][6] = {
	{
    0b00000,
    0b10000,
    0b11000,
    0b01100,
    0b11110,
    0b11111
  },{
		0b00000,
		0b10000,
    0b01000,		
		0b00100,
		0b00010,
		0b00001
	 }
};

int draw = 0;
int numberOfBars = 3;
int barWidth = 5;

int charPixelBuidler[8] = {
	0,0,0,0,0,0,0,0
};

int charBlockBuidler[8] = {
	0,0,0,0,0,0,0,0
};


void makeTriange(byte *data, int height) {
	if(height > 5 || height < 0) return;

	if(height == 0) {
		byte data[8] = {
			0b00000,
			0b00000,
			0b00000,
			0b00000,
			0b00000,
			0b00000,
			0b00000,
			0b10000
		};
	} else if(height == 1) {
		byte data[8] = {
			0b00000,
			0b00000,
			0b00000,
			0b00000,
			0b00000,
			0b00000,
			0b01000,
			0b11000
		};
	} else if(height == 2) {
		byte data[8] = {
			0b00000,
			0b00000,
			0b00000,
			0b00000,
			0b00000,
			0b00100,
			0b01100,
			0b11100
		};
	} else if(height == 3) {
		byte data[8] = {
			0b00000,
			0b00000,
			0b00000,
			0b00000,
			0b00010,
			0b00110,
			0b01110,
			0b11110
		};
	} else if(height == 4) {
		byte data[8] = {
			0b00000,
			0b00000,
			0b00000,
			0b00001,
			0b00011,
			0b00111,
			0b01111,
			0b11111
		};
	}
	
}


void displayCurrentPowerDraw(int powerDraw, int startX = 0, int startY = 0) {
	/*
	* Display the current power draw on a 5 bar scale, flash on max power draw
	* 
	* powerDraw is between 0 and 1023
	*/

	// return if powerDraw is not between given requirements
	if(powerDraw > 1023 || powerDraw < 0) return;

	int triangleWidth = barWidth * numberOfBars;
	int triangleHeight = 8;

	int powerMap = map(powerDraw, 0, 1023, 0, triangleWidth);

	int powerCharMap = map(powerDraw, 0, 1023, 0, 8);   

	int currentBlock = 1;

	int currentYPos = 0;
  
  bool defined = false;

  int topPixelLocation = 7;

	for(int i = 0; i <= powerMap; i++) {
		currentBlock = (int)(i / barWidth);
    defined = false;
    for(int j = 0; j < 8; j++) {
      charBlockBuidler[j] = 0;
    }
    
		for(int j = 7; j >= 0; j--) {

			charPixelBuidler[j] = map((map(powerMap, 0, barWidth * numberOfBars, 0, j)), 0, 7, 0, 5);

			currentYPos = (int) (charPixelBuidler[j] * triangleHeight) / triangleWidth;
		}

      for(int j = 0; j < 8; j++) {
        if(charPixelBuidler[j] != 0 && !defined) {
          charBlockBuidler[j] = 1;
          charPixelBuidler[j] = charPixelBuidler[7] + 1;
          topPixelLocation = j;
          defined = true;
        }
        
      }

      if(!defined) {
        charBlockBuidler[7] = 1;
        charPixelBuidler[7] = charPixelBuidler[7] + 1;
      }

      Serial.println("Before: ");
      for(int j = 0; j < 8; j++) {        
        Serial.print(charPixelBuidler[j]);
        Serial.print(", ");
      }
      Serial.println();
      Serial.println("After: ");


      int flipArray[8-(topPixelLocation + 1) - 1];

      for(int j = topPixelLocation + 1; j < 8; j++) {
          flipArray[j - (topPixelLocation + 1)] = charPixelBuidler[j];
      }

      int valueCounter = (topPixelLocation + 1);
      for(int j = 8-(topPixelLocation + 1) - 1; j >= 0; j--) {
        charPixelBuidler[valueCounter] = flipArray[j];
        valueCounter++;
      }

      
      for(int j = 0; j < 8; j++) {        
        Serial.print(charPixelBuidler[j]);
        Serial.print(", ");
      }
      Serial.println();
      for(int j = 0; j < 8; j++) {
        Serial.print(charBlockBuidler[j]);
        Serial.print(", ");
      }
      Serial.println();

		byte bar1[8] = {
		customChar[charBlockBuidler[0]][charPixelBuidler[0]],
		customChar[charBlockBuidler[1]][charPixelBuidler[1]],
		customChar[charBlockBuidler[2]][charPixelBuidler[2]],
		customChar[charBlockBuidler[3]][charPixelBuidler[3]],
		customChar[charBlockBuidler[4]][charPixelBuidler[4]],
		customChar[charBlockBuidler[5]][charPixelBuidler[5]],
		customChar[charBlockBuidler[6]][charPixelBuidler[6]],
		customChar[charBlockBuidler[7]][charPixelBuidler[7]]
		};

		//makeTriange(bar1, currentYPos);

		//delay(100);

		lcd.createChar (currentBlock, bar1 );

		lcd.setCursor(startX + currentBlock, startY);

		lcd.write(byte(currentBlock));

//      Serial.print(i);
//      Serial.print(" ");
		Serial.println(currentBlock);
		//delay(500);
	}
	lcd.print("    ");
}


void setup() {

	Serial.begin(9600);
	lcd.begin(16, 2);

	lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
	lcd.setBacklight(HIGH);  

	lcd.home();
	lcd.print("Speed: ");  
}

void loop() {
	lcd.home();
	lcd.setCursor(7,0);
	lcd.print(draw);
	lcd.print("    ");

	displayCurrentPowerDraw(draw, 11, 1);

	if(draw < 1023) {
	draw += 25;
	} else {
	draw = 0;
	}
}

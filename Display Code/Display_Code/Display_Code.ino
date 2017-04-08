#include "U8glib.h"

<<<<<<< Updated upstream
// Our display is model ST7920, it is using arduino pins 13, 11 and 10
=======
// Tells arduino what display we are using and what pins to use
>>>>>>> Stashed changes
U8GLIB_ST7920_128X64_1X u8g(13, 11, 10);

// Both the width and height are in pixles
int loadingBarWidth = 55;
int loadingBarHeight = 10;
int current_speed = 0;
int current_power = 0;

// This is for debugging that the display works, 
// it serves no purpose but to repeat the counting later on in the code
bool backwards = false;

<<<<<<< Updated upstream
int calcPercent(int current_value, int max_value = 100) {
  /*
   * Calculate the width of a percent bar using the max width of loadingBarWidth
   * and the given percent
   * 
   * calcPercent(int, int) -> int
   * 
   * current_value > 0
   * max_value > 0
   * max_value > current_value
   */
=======
// Used to calculate the percent for a bar of given width (loadingBarWidth)
int calcPercent(int max_percent, int current_value, int max_value) {
>>>>>>> Stashed changes
  double percent = ((double)current_value / (double)max_value) * 100;
  return map((int)percent, 0, 100, 0, loadingBarWidth);
}

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_fub42n);

<<<<<<< Updated upstream
  u8g.drawBox(69,6,calcPercent(current_speed),loadingBarHeight);   // xpos, ypos, width, height

  // Convert the ints to strings for drawing  
  String draw_speed = String(current_speed);
  String draw_power = String(current_power);

  // Draw the current speed  
  u8g.drawStr(0, 43, draw_speed.c_str());

  // Draw the current power level
  u8g.setFont(u8g_font_fub35n);
  u8g.drawStr(64, 55, draw_power.c_str());

  // Draw km/h
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr(30, 55, "km/h");

  // Debug
  // Serial.println(current_speed);

  // Draw the box at the end of the loading bar to make it look like a battery
=======
  // Draw the loading bar with the given value (test_speed), loadingBarWidth and Height stay constant
  u8g.drawBox(69,6,calcPercent(loadingBarWidth, test_speed, 100), loadingBarHeight);   // xpos, ypos, width, height

  // Draws the text of the speed
  String draw_this = String(test_speed);
  u8g.drawStr(0, 43, draw_this.c_str());

  // debug stuff
  Serial.println(draw_this);

  // Draw the tip of the battery at the middle right of the loading bar
>>>>>>> Stashed changes
  u8g.drawBox(124,8,1,5);
  u8g.drawFrame(68,5,loadingBarWidth + 1,loadingBarHeight + 1);
}

void setup(void) {
  // Setup debug and color index of 1 (no color)
  Serial.begin(9600);
  u8g.setColorIndex(1);
}

bool first = true;

void loop(void) {
  // picture loop
  
  u8g.firstPage();

  // This display has 8 segments, we must loop 8 times before 1 frame is drawn
  // we use the first variable to make sure that our variables don't change
  // during this looping process
  do {
      if(first) {
        first = false;

        // This is where you will set the variables
        
        // Start counting between 0 and 99 then back to 0 and repeating
        if(current_speed == 0) {
          backwards = false;
        }
        if(current_speed < 99 && !backwards) {
          current_speed++;
        } else {
          current_speed--;
          backwards = true;
        }
        // End Counting Stuff
        
      }
      draw();
  } while( u8g.nextPage() );

  // Set first to true and start the loop all over again
  first = true;
  
  // rebuild the picture after some delay
  delay(100);
}


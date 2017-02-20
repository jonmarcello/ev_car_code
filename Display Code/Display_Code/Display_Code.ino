#include "U8glib.h"

U8GLIB_ST7920_128X64_1X u8g(13, 11, 10);

int loadingBarWidth = 55;
int loadingBarHeight = 10;
int test_speed = 0;
bool backwards = false;

int calcPercent(int max_percent, int current_value, int max_value) {
  double percent = ((double)current_value / (double)max_value) * 100;
  return map((int)percent, 0, 100, 0, max_percent);
}

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_fub42n);
  //u8g.setFont(u8g_font_osb35);
  //u8g.setScale2x2();
  

  //u8g.undoScale();
  u8g.drawBox(69,6,calcPercent(loadingBarWidth, test_speed, 100),loadingBarHeight);   // xpos, ypos, width, height

  String draw_this = String(test_speed);
  u8g.drawStr(0, 43, draw_this.c_str());
  Serial.println(draw_this);
 
  u8g.drawBox(124,8,1,5);
  u8g.drawFrame(68,5,loadingBarWidth + 1,loadingBarHeight + 1);
}

void setup(void) {
  Serial.begin(9600);
  u8g.setColorIndex(1);
}

bool first = true;

void loop(void) {
  // picture loop
  
  u8g.firstPage();

  do {
      if(first) {
        first = false;
        if(test_speed == 0) {
          backwards = false;
        }
        if(test_speed < 99 && !backwards) {
          test_speed++;
        } else {
          test_speed--;
          backwards = true;
        }
      }
      draw();
  } while( u8g.nextPage() );
  first = true;
  // rebuild the picture after some delay
  delay(100);
}


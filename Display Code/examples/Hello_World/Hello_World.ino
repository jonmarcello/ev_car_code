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
  u8g.drawStr( 0, 43, String(test_speed));
  if(test_speed == 0) {
    backwards = false;
  }
  if(test_speed < 99 && !backwards) {
    test_speed++;
  } else {
    test_speed--;
    backwards = true;
  }
  
  //u8g.undoScale();
  u8g.drawBox(69,6,calcPercent(loadingBarWidth, 25, 50),loadingBarHeight);   // xpos, ypos, width, height
  u8g.drawBox(124,8,1,5);
  u8g.drawFrame(68,5,loadingBarWidth + 1,loadingBarHeight + 1);
}

void setup(void) {
  Serial.begin(9600);
  // flip screen, if required
  // u8g.setRot180();
  
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
}

void loop(void) {
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(500);
}


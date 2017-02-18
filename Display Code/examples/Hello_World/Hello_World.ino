
#include "U8glib.h"

U8GLIB_ST7920_128X64_1X u8g(10);
//U8GLIB_ST7920_128X64_4X u8g(13, 11, 10);
//U8GLIB_ST7920_128X64_4X u8g(13, 11, 10);

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here
  u8g.setFont(u8g_font_04b_03b);
  //u8g.setFont(u8g_font_osb21);
  u8g.drawStr( 0, 22, "Aqua Control V2");
}

void setup(void) {
// assign default color value
  Serial.begin(9600);  
  Serial.println("--- Start Serial Monitor SEND_RCVE ---");
  Serial.println(u8g.getMode());

  if ( u8g.getMode() == U8G_MODE_R3G3B2 )
    u8g.setColorIndex(255); // white
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT )
    u8g.setColorIndex(3); // max intensity
  else if ( u8g.getMode() == U8G_MODE_BW )
    u8g.setColorIndex(1); // pixel on
}

void loop(void) {
  // picture loop
  u8g.firstPage();
  do {
    draw();
  } while( u8g.nextPage() );

  // rebuild the picture after some delay
  delay(1000);
}

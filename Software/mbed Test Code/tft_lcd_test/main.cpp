#include "mbed.h"
#include "millis.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_MOSI    p5
#define TFT_MISO    p6
#define TFT_SCLK    p7
#define TFT_CS      p8
#define TFT_DC      p9
#define TFT_RST     p10

// For 1.44" and 1.8" TFT with ST7735 use
SPI spi(TFT_MOSI, TFT_MISO, TFT_SCLK);
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, TFT_CS, TFT_DC, TFT_RST);
PwmOut lite(p26);

float p = 3.1415926;

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
    wait_ms(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
    wait_ms(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
    wait_ms(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
    wait_ms(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
    wait_ms(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
    wait_ms(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
    wait_ms(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
    wait_ms(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.printf(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.printf("Hello World!\n");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.printf("Hello World!\n");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.printf("Hello World!\n");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.printf("%f", 1234.567);
  wait_ms(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.printf("Hello World!\n");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.printf("%0.6f", p);
  tft.printf(" Want pi?\n");
  tft.printf(" \n");
  tft.printf("0x%x", 8675309); // print 8,675,309 out in HEX!
  tft.printf(" Print HEX!\n");
  tft.printf(" \n");
  tft.setTextColor(ST77XX_WHITE);
  tft.printf("Sketch has been\n");
  tft.printf("running for: \n");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.printf("%lu", millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.printf(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
  wait_ms(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
  wait_ms(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
  wait_ms(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}

int main() {
  // Start millis() library
  millisStart();
  
  // Full brightness on backlight and 1 kHz frequency
  lite = 1.0;
  lite.period(1/1000.0);
  
  printf("Hello! ST77xx TFT Test");

  // Use this initializer if you're using a 1.8" TFT
  // tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  // Use this initializer (uncomment) if you're using a 1.44" TFT
  tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab

  // Use this initializer (uncomment) if you're using a 0.96" 180x60 TFT
  //tft.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display

  // Use this initializer (uncomment) if you're using a 1.54" 240x240 TFT
  //tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels

  printf("Initialized\n");

  unsigned long time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  printf("%lu", time);
  wait_ms(500);

  // large block of text
  tft.fillScreen(ST77XX_BLACK);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
  wait_ms(1000);

  // tft print function!
  tftPrintTest();
  wait_ms(4000);

  // a single pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, ST77XX_GREEN);
  wait_ms(500);

  // line draw test
  testlines(ST77XX_YELLOW);
  wait_ms(500);

  // optimized lines
  testfastlines(ST77XX_RED, ST77XX_BLUE);
  wait_ms(500);

  testdrawrects(ST77XX_GREEN);
  wait_ms(500);

  testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
  wait_ms(500);

  tft.fillScreen(ST77XX_BLACK);
  testfillcircles(10, ST77XX_BLUE);
  testdrawcircles(10, ST77XX_WHITE);
  wait_ms(500);

  testroundrects();
  wait_ms(500);

  testtriangles();
  wait_ms(500);

  mediabuttons();
  wait_ms(500);

  printf("done\n");
  wait_ms(1000);

  while (1) {
    bool flip = true;
      
    for (float i = 1.0; i > 0.1; i -= 0.1) {
        lite = i;
        tft.invertDisplay(flip);
        flip = !flip;
        wait_ms(500);
    }
    for (float i = 0.1; i < 1.0; i += 0.1) {
        lite = i;
        tft.invertDisplay(flip);
        flip = !flip;
        wait_ms(500);
    }
  }
}

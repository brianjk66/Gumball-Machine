#include "mbed.h"
#include "Arduino.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library for ILI9341


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_MOSI    p5
#define TFT_MISO    p6
#define TFT_SCLK    p7
#define TFT_CS      p8
#define TFT_DC      p9
#define TFT_RST     p10

#define min(a, b) ((a < b) ? a : b)
// For 2.8" TFT Touchscreen with ILI9341 use
SPI spi(TFT_MOSI, TFT_MISO, TFT_SCLK);
Adafruit_ILI9341 tft = Adafruit_ILI9341(&spi, TFT_CS, TFT_DC, TFT_RST);
PwmOut lite(p23);

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(ILI9341_BLACK);
  wait(1);
  tft.fillScreen(ILI9341_RED);
  wait(1);
  tft.fillScreen(ILI9341_GREEN);
  wait(1);
  tft.fillScreen(ILI9341_BLUE);
  wait(1);
  tft.fillScreen(ILI9341_BLACK);
  wait(1);
  return micros() - start;
}

unsigned long testText() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.printf("Hello World!\n");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.printf("%f\n", 1234.56);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.printf("%x\n\n", 0xDEADBEEF);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.printf("Groop\n");
  tft.setTextSize(2);
  tft.printf("I implore thee,\n");
  tft.setTextSize(1);
  tft.printf("my foonting turlingdromes.\n");
  tft.printf("And hooptiously drangle me\n");
  tft.printf("with crinkly bindlewurdles,\n");
  tft.printf("Or I will rend thee\n");
  tft.printf("in the gobberwarts\n");
  tft.printf("with my blurglecruncheon,\n");
  tft.printf("see if I don't!\n");
  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(ILI9341_BLACK);
  wait(1);
  
  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  wait(1);
  tft.fillScreen(ILI9341_BLACK);
  wait(1);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  wait(1);
  tft.fillScreen(ILI9341_BLACK);
  wait(1);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  wait(1);
  tft.fillScreen(ILI9341_BLACK);
  wait(1);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

  wait(1);
  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(ILI9341_BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  n = min(tft.width(), tft.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
    wait(1);
  }

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.color565(i, i, i));
  }

  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for(i=min(cx,cy); i>10; i-=5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(0, i*10, i*10));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(i*10, i*10, 0));
    wait(1);
  }

  return t;
}

unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  w     = min(tft.width(), tft.height());
  start = micros();
  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 0, 0));
  }

  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for(i=min(tft.width(), tft.height()); i>20; i-=6) {
    i2 = i / 2;
    tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(0, i, 0));
    wait(1);
  }

  return micros() - start;
}

int main() {
  // Start time for Arduino library
  StartArduinoTimer();
  
  // Full brightness on backlight and 1 kHz frequency
  lite = 1.0;
  lite.period(1/1000.0);
  
  // Begin Graphics Test
  printf("ILI9341 Test!\n"); 
 
  tft.begin();

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  printf("Display Power Mode: 0x%x\n", x);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  printf("MADCTL Mode: 0x%x\n", x);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  printf("Pixel Format: 0x%x\n", x);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  printf("Image Format: 0x%x\n", x);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  printf("Self Diagnostic: 0x%x\n", x); 
  
  printf("Benchmark                Time (microseconds)\n");
  wait_ms(10);
  printf("Screen fill              ");
  printf("%lu\n", testFillScreen());
  wait_ms(500);

  printf("Text                     ");
  printf("%lu\n", testText());
  wait_ms(3000);

  printf("Lines                    ");
  printf("%lu\n", testLines(ILI9341_CYAN));
  wait_ms(500);

  printf("Horiz/Vert Lines         ");
  printf("%lu\n", testFastLines(ILI9341_RED, ILI9341_BLUE));
  wait_ms(500);

  printf("Rectangles (outline)     ");
  printf("%lu\n", testRects(ILI9341_GREEN));
  wait_ms(500);

  printf("Rectangles (filled)      ");
  printf("%lu\n", testFilledRects(ILI9341_YELLOW, ILI9341_MAGENTA));
  wait_ms(500);

  printf("Circles (filled)         ");
  printf("%lu\n", testFilledCircles(10, ILI9341_MAGENTA));

  printf("Circles (outline)        ");
  printf("%lu\n", testCircles(10, ILI9341_WHITE));
  wait_ms(500);

  printf("Triangles (outline)      ");
  printf("%lu\n", testTriangles());
  wait_ms(500);

  printf("Triangles (filled)       ");
  printf("%lu\n", testFilledTriangles());
  wait_ms(500);

  printf("Rounded rects (outline)  ");
  printf("%lu\n", testRoundRects());
  wait_ms(500);

  printf("Rounded rects (filled)   ");
  printf("%lu\n", testFilledRoundRects());
  wait_ms(500);

  printf("Done!\n");

  while (1) {
    for(uint8_t rotation=0; rotation<4; rotation++) {
        tft.setRotation(rotation);
        testText();
        wait_ms(1000);
    }
//    bool flip = true;
//      
//    for (float i = 1.0; i > 0.1; i -= 0.1) {
//        lite = i;
//        tft.invertDisplay(flip);
//        flip = !flip;
//        wait_ms(500);
//    }
//    for (float i = 0.1; i < 1.0; i += 0.1) {
//        lite = i;
//        tft.invertDisplay(flip);
//        flip = !flip;
//        wait_ms(500);
//    }
  }
}

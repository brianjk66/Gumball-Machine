#include "mbed.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>

// The FT6206 uses hardware I2C (SCL/SDA)
I2C i2c(p28, p27);
InterruptIn interrupt(p11);
Adafruit_FT6206 ctp = Adafruit_FT6206(&i2c);

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

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

void touched() {
    // Retrieve a point  
    TS_Point p = ctp.getPoint();
    
    /*
    // Print out raw data from screen touch controller
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print(" -> ");
    */
    
    // flip it around to match the screen.
    p.x = 240 - p.x;
    p.y = 320 - p.y;
    
    // Print out the remapped (rotated) coordinates
//    printf("(%d, %d)\n\r", p.x, p.y);        
    
    // Select new color
    if (p.y < BOXSIZE) {
        oldcolor = currentcolor;
    
        if (p.x < BOXSIZE) { 
            currentcolor = ILI9341_RED; 
            tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
        } else if (p.x < BOXSIZE*2) {
            currentcolor = ILI9341_YELLOW;
            tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
        } else if (p.x < BOXSIZE*3) {
            currentcolor = ILI9341_GREEN;
            tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
        } else if (p.x < BOXSIZE*4) {
            currentcolor = ILI9341_CYAN;
            tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
        } else if (p.x < BOXSIZE*5) {
            currentcolor = ILI9341_BLUE;
            tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
        } else if (p.x <= BOXSIZE*6) {
            currentcolor = ILI9341_MAGENTA;
            tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
        }
    
        if (oldcolor != currentcolor) {
            if (oldcolor == ILI9341_RED) 
                tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
            if (oldcolor == ILI9341_YELLOW) 
                tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
            if (oldcolor == ILI9341_GREEN) 
                tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
            if (oldcolor == ILI9341_CYAN) 
                tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
            if (oldcolor == ILI9341_BLUE) 
                tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
            if (oldcolor == ILI9341_MAGENTA) 
                tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
        }
    }
    
    if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
        tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }    
}

int main() {
    printf("Cap Touch Paint!\n\r");
    
    tft.begin();
    
    if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
        printf("Couldn't start FT6206 touchscreen controller\n\r");
        while (1);
    }
    
    printf("Capacitive touchscreen started\n\r");
    
    tft.fillScreen(ILI9341_BLACK);
    
    // make the color selection boxes
    tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
    tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
    tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
    tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
    tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
    tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
    
    // select the current color 'red'
    tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    currentcolor = ILI9341_RED;
    
    // Attach ISR to interrupt pin
    interrupt.fall(&touched);
  
    while (1);
}

#include "mbed.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>

// The FT6206 uses hardware I2C (SCL/SDA)
I2C i2c(p28, p27);
InterruptIn interrupt(p11);
Adafruit_FT6206 ctp(&i2c);

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
Adafruit_ILI9341 tft(&spi, TFT_CS, TFT_DC, TFT_RST);

Adafruit_GFX_Button button;
Adafruit_GFX_Button button2;

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

bool inverted = false;
bool inverted2 = false;

void touched() {
    // Retrieve a point  
    TS_Point p = ctp.getPoint();
    
    // flip it around to match the screen.
    p.x = 240 - p.x;
    p.y = 320 - p.y;
    
    // Check if buttons were pressed
    button.press(button.contains(p.x, p.y));
    button2.press(button2.contains(p.x, p.y));
    
    if (button.justPressed()) {
        if (inverted) {
            // Write text in white
            tft.setCursor(0, 20);
            tft.setTextColor(ILI9341_WHITE);
            tft.setTextSize(2);
            tft.printf("Adam has a cute butt");
            
            // Invert button
            button.drawButton(inverted);
            inverted = !inverted;
        } else {
            // Erase text
            tft.setCursor(0, 20);
            tft.setTextColor(ILI9341_BLACK);
            tft.setTextSize(2);
            tft.printf("Adam has a cute butt");
            
            // Invert button
            button.drawButton(inverted);
            inverted = !inverted;
        }
    } else if (button2.justPressed()) {
        if (inverted2) {
            // Write text in white
            tft.setCursor(0, 280);
            tft.setTextColor(ILI9341_WHITE);
            tft.setTextSize(2);
            tft.printf(" Adam eats his poop ");
            
            // Invert button
            button2.drawButton(inverted2);
            inverted2 = !inverted2;
        } else {
            // Erase text
            tft.setCursor(0, 280);
            tft.setTextColor(ILI9341_BLACK);
            tft.setTextSize(2);
            tft.printf(" Adam eats his poop ");
            
            // Invert button
            button2.drawButton(inverted2);
            inverted2 = !inverted2;
        }
    }
}

int main() {
    printf("Cap Touch Button!\n\r");
    
    tft.begin();
    
    if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
        printf("Couldn't start FT6206 touchscreen controller\n\r");
        while (1);
    }
    
    printf("Capacitive touchscreen started\n\r");
    
    tft.fillScreen(ILI9341_BLACK);
        
    // Draw first button
    button.initButton(&tft, 120, 100, 120, 90, ILI9341_WHITE, ILI9341_CYAN, ILI9341_BLUE, "Press Me!", 2);
    button.drawButton(inverted2);
    inverted2 = !inverted2;
    
    // Draw second button
    button2.initButton(&tft, 120, 220, 120, 90, ILI9341_WHITE, ILI9341_MAGENTA, ILI9341_RED, "Press Me!", 2);
    button2.drawButton(inverted);
    inverted = !inverted;
    
    // Attach ISR to interrupt pin
    interrupt.fall(&touched);
  
    while (1);
}

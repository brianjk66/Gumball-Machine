#include "mbed.h"
#include "QEI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>

// Graphics Constants
#define NUM_BUTTONS       (5)
#define FIRST_BUTTON_Y    (60)
#define BUTTON_HEIGHT     (60)
#define BUTTON_WIDTH      (120)
#define HIGHLIGHT_HEIGHT  (BUTTON_HEIGHT + 4)
#define HIGHLIGHT_WIDTH   (BUTTON_WIDTH + 4)
#define BUTTON_SPACING    (100)
#define BUTTON_COLOR      (ILI9341_CYAN)
#define BUTTON_TEXT_COLOR (ILI9341_BLUE)
#define HIGHLIGHT_COLOR   (ILI9341_MAGENTA)

// Touchscreen Pins
#define TFT_MOSI    p5      // LCD Controller MOSI
#define TFT_MISO    p6      // LCD Controller MISO
#define TFT_SCLK    p7      // LCD Controller SCLK
#define TFT_CS      p8      // LCD Controller Chip Select
#define TFT_DC      p9      // LCD Controller Data/Command
#define TFT_RST     p10     // LCD Reset
#define TFT_SDA     p28     // Touch Controller SDA
#define TFT_SCL     p27     // Touch Controller SCL
#define TFT_INT     p11     // Touch Controller Interrupt Line

#define min(a, b) ((a < b) ? a : b)

void up();
void down();

// Quadrature Rotary Encoder
QEI encoder(p16, p17, NC, 24, QEI::X2_ENCODING, up, down);
InterruptIn button(p18);

// USB serial communication
Serial pc(USBTX, USBRX);

// 2.8" TFT Touchscreen with ILI9341 (SPI communication)
SPI spi(TFT_MOSI, TFT_MISO, TFT_SCLK);
Adafruit_ILI9341 tft(&spi, TFT_CS, TFT_DC, TFT_RST);

// FT6206 Touch Controller (I2C communication)
I2C i2c(TFT_SDA, TFT_SCL);
InterruptIn interrupt(TFT_INT);
Adafruit_FT6206 ctp(&i2c);

// Data structures for managing buttons
Adafruit_GFX_Button buttons[NUM_BUTTONS];
bool inverted[NUM_BUTTONS] = {false};

// Global Variables
int scrollOffset = 0;
int highlight = FIRST_BUTTON_Y;
int oldHighlight = highlight;
bool newOffset = false, newHighlight = false;
char buttonName[] = "Button X";
int index;

void moveHighlight(int newHighlight, bool redrawButton = true) {
    if (highlight == newHighlight) return;
    
    // Update highlight values
    oldHighlight = highlight;
    highlight = newHighlight;
    
    // Erase old highlight
    index = (oldHighlight - FIRST_BUTTON_Y - scrollOffset)/BUTTON_SPACING;
    tft.fillRoundRect(120-((HIGHLIGHT_WIDTH)/2), oldHighlight-(HIGHLIGHT_HEIGHT/2), HIGHLIGHT_WIDTH, HIGHLIGHT_HEIGHT, HIGHLIGHT_HEIGHT/4, ILI9341_BLACK);
    buttons[index].drawButton(inverted[index]);
    
    // Draw new highlight
    index = (highlight - FIRST_BUTTON_Y - scrollOffset)/BUTTON_SPACING;
    tft.fillRoundRect(120-((HIGHLIGHT_WIDTH)/2), highlight-(HIGHLIGHT_HEIGHT/2), HIGHLIGHT_WIDTH, HIGHLIGHT_HEIGHT, HIGHLIGHT_HEIGHT/4, HIGHLIGHT_COLOR);
    if (redrawButton) buttons[index].drawButton(inverted[index]);
}

// Action to take when button at specified index is pressed
void justPressed(int index) {
    if (!inverted[index]) {
        // Write text in white
        tft.setCursor(0, 100);
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(2);
        tft.printf("  Button %d pressed! ", index+1);
        
        // Invert button
        inverted[index] = !inverted[index];
        buttons[index].drawButton(inverted[index]);
    } else {
        // Erase text
        tft.setCursor(0, 100);
        tft.setTextColor(ILI9341_BLACK);
        tft.setTextSize(2);
        tft.printf("  Button %d pressed! ", index+1);
        
        // Invert button
        inverted[index] = !inverted[index];
        buttons[index].drawButton(inverted[index]);
    }    
}

void touched() {
    // Retrieve a point  
    TS_Point p = ctp.getPoint();
    
    // flip it around to match the screen.
    p.x = 240 - p.x;
    p.y = 320 - p.y;
    
    // Check if buttons were pressed
    for (int i = 0; i < NUM_BUTTONS; i++)
        buttons[i].press(buttons[i].contains(p.x, p.y));
    
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i].justPressed()) {
            moveHighlight(FIRST_BUTTON_Y+(i*BUTTON_SPACING)+scrollOffset, false);
            justPressed(i);
            break;
        }
    }
}

void fire() {
    index = (highlight - FIRST_BUTTON_Y - scrollOffset)/BUTTON_SPACING;
    justPressed(index);
}

void up() {
    // If we're already highlighting the top button, scroll up
    if ((highlight - BUTTON_SPACING) < 0) {
        // Don't scroll above first button
        if (scrollOffset < 0) {
            scrollOffset += BUTTON_SPACING;
            newOffset = true;
        }
    // Otherwise, hightlight the next button up
    } else {
        moveHighlight(highlight - BUTTON_SPACING);
    }
    
    // If there's a new offset, re-draw all buttons, but leave highlight
    if (newOffset) {
        for (int i = 0; i < NUM_BUTTONS; i++) {
            buttonName[7] = '1' + i;
            buttons[i].initButton(&tft, 120, 60+(i*BUTTON_SPACING)+scrollOffset, BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_TEXT_COLOR, BUTTON_COLOR, BUTTON_TEXT_COLOR, buttonName, 2);
            buttons[i].drawButton(inverted[i]);
        }
        
        // Reset flag
        newOffset = false;    
    }
}

void down() {
    // If we're already highlighting the bottom button, scroll down
    if ((highlight + BUTTON_SPACING) > 320) {
        // Don't scroll below last button
        if (scrollOffset > (-BUTTON_SPACING*(NUM_BUTTONS - 3))) {
            scrollOffset -= BUTTON_SPACING;
            newOffset = true;
        }
    // Otherwise, highlight the next button down
    } else {
        moveHighlight(highlight + BUTTON_SPACING);
    }
    
    // If there's a new offset, re-draw all buttons, but leave highlight
    if (newOffset) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttonName[7] = '1' + i;
        buttons[i].initButton(&tft, 120, 60+(i*BUTTON_SPACING)+scrollOffset, BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_TEXT_COLOR, BUTTON_COLOR, BUTTON_TEXT_COLOR, buttonName, 2);
        buttons[i].drawButton(inverted[i]);
    }
    
    // Reset flag
    newOffset = false;    
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
     
    // Draw Highlight around top button
    tft.fillRoundRect(120-((HIGHLIGHT_WIDTH)/2), highlight-(HIGHLIGHT_HEIGHT/2), HIGHLIGHT_WIDTH, HIGHLIGHT_HEIGHT, HIGHLIGHT_HEIGHT/4, HIGHLIGHT_COLOR); 
        
    // Draw buttons
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttonName[7] = '1' + i;
        buttons[i].initButton(&tft, 120, FIRST_BUTTON_Y+(i*BUTTON_SPACING), BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_TEXT_COLOR, BUTTON_COLOR, BUTTON_TEXT_COLOR, buttonName, 2);
        buttons[i].drawButton(inverted[i]);
    }
    
    // Attach ISRs to interrupt pins
    interrupt.fall(&touched);
    button.fall(&fire);
  
    // Spin and wait for interrupts from peripherals
    while (1);
}

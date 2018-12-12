#include "mbed.h"
#include "QEI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>

// Graphics Constants
#define HEIGHT            (240)
#define WIDTH             (320)
#define X_CENTER          (WIDTH / 2)
#define Y_CENTER          (HEIGHT / 2)
#define NUM_BUTTONS       (4)
#define BUTTON_HEIGHT     (60)
#define BUTTON_WIDTH      (120)
#define BUTTON_COLOR      (ILI9341_CYAN)
#define BUTTON_TEXT_COLOR (ILI9341_BLUE)

// Touchscreen Pins
#define TFT_MOSI         p5     // LCD Controller MOSI
#define TFT_MISO         p6     // LCD Controller MISO
#define TFT_SCLK         p7     // LCD Controller SCLK
#define TFT_CS           p8     // LCD Controller Chip Select
#define TFT_DC           p11    // LCD Controller Data/Command
#define TFT_RST          p12    // LCD Reset
#define TFT_SDA          p9     // Touch Controller SDA
#define TFT_SCL          p10    // Touch Controller SCL
#define TFT_INT          p13    // Touch Controller Interrupt Line
#define TFT_CARD_DETECT  p14    // SD Card Detect Pin
#define TFT_CARD_CS      p15    // SD Card Chip Select
#define TFT_BACKLITE     p21    // Touch Screen Backlight

// Solenoids
#define VALVE_1          p26    // MOSFET drive pin for solenoid 1
#define VALVE_2          p25    // MOSFET drive pin for solenoid 2
#define VALVE_3          p24    // MOSFET drive pin for solenoid 3
#define VALVE_4          p23    // MOSFET drive pin for solenoid 4

// Must be calibrated based on flow of drinks
#define TIME_SCALAR      10     // Scalar for timing of drink dispensing


void up();
void down();

// Solenoid Valve Conrols
DigitalOut valve1(VALVE_1);
DigitalOut valve2(VALVE_2);
DigitalOut valve3(VALVE_3);
DigitalOut valve4(VALVE_4);

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

// Global Variables
int scrollOffset = 0;
bool newOffset = false;
char buttonName[] = "Drink X";
int index;

Timer timer;

const int recipes[4][4] {
    // Drink 1 (Even mix of everything)
    { 1, 1, 1, 1},
    // Drink 2 (2:1 Ratio of Mixer 1 to Mixer 2)  
    { 2, 1, 0, 0},
    // Drink 3 (1:1 Ratio of Mixer 3 to Mixer 4)
    { 0, 0, 1, 1},
    // Drink 4 (Just Drink 4)
    { 0, 0, 0, 1}
};

// Function Declarations
void makeDrink(int);
void justPressed(int);
void touched();
void drawButtons();


void makeDrink(int num) {
    bool stop = false;
    float seconds;
    
    // Disable interrupts while dispensing
    interrupt.fall(NULL);
    
    // Start timer
    timer.start();
    
    // Turn on valves
    if (recipes[num][0])
        valve1 = 1;
    if (recipes[num][1])
        valve2 = 1;
    if (recipes[num][2])
        valve3 = 1;
    if (recipes[num][3])
        valve4 = 1;
    
    // Wait until drink is dispensed
    while (!stop) {
        // Sample the timer
        seconds = timer.read();
        
        stop = true;
        
        // Check each drink
        for (int i = 0; i < 4; i++) {
            // If the timer has expired for a mixer, shut off its valve
            if (seconds >= (TIME_SCALAR * recipes[num][i])) {
                switch (i) {
                    case 0:
                        valve1 = 0;
                        break;
                    case 1:
                        valve2 = 0;
                        break;
                    case 2:   
                        valve3 = 0;
                        break;
                    case 3: 
                        valve4 = 0;
                        break;
                }
            } else {
                // Otherwise, don't stop
                stop = false;
            }
        }
    }
    
    // Stop and reset timer
    timer.stop();
    timer.reset();
    
    // Re-draw buttons
    drawButtons();
    
    // Re-attach ISR to interrupt pin
    interrupt.fall(&touched);
}

// Action to take when button at specified index is pressed
void justPressed(int index) {
    // Clear Screen
    tft.fillScreen(ILI9341_BLACK);
    
    // Write text in white
    tft.setCursor(0, Y_CENTER);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.printf("    Dispensing Drink %d! ", index+1);
    
    // Make drink
    makeDrink(index);
}

void touched() {
    // Retrieve a point  
    TS_Point p = ctp.getPoint();
    
    // flip it around to match the screen.
//    p.x = HEIGHT - p.x;
    p.y = WIDTH - p.y;
    
    // Check if buttons were pressed
    for (int i = 0; i < NUM_BUTTONS; i++)
        // Swap X & Y because of screen rotation
        buttons[i].press(buttons[i].contains(p.y, p.x));
    
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i].justPressed()) {
            justPressed(i);
            break;
        }
    }
}

void drawButtons() {
     // Clear screen
     tft.fillScreen(ILI9341_BLACK);

    // Initialize and draw each button    
    for (int i = 0; i < NUM_BUTTONS/2; i++) {
        for (int j = 0; j < NUM_BUTTONS/2; j++) {      
            buttonName[6] = '1' + 2*i + j;
            buttons[2*i + j].initButton(&tft, X_CENTER/2+(j*X_CENTER), Y_CENTER/2+(i*Y_CENTER), BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_TEXT_COLOR, BUTTON_COLOR, BUTTON_TEXT_COLOR, buttonName, 2);
            buttons[2*i + j].drawButton();
        }
    }
}

int main() {
    printf("Cap Touch Button!\n\r");
    
    tft.begin();
    
    // Orient screen horizontally
    tft.setRotation(1);
    
    if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
        printf("Couldn't start FT6206 touchscreen controller\n\r");
        while (1);
    }
    
    printf("Capacitive touchscreen started\n\r");
        
    // Draw buttons
    drawButtons();
    
    // Attach ISR to interrupt pin
    interrupt.fall(&touched);
  
    // Spin and wait for interrupts
    while (1);
}

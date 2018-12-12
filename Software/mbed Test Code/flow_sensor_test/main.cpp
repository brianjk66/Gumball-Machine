#include "mbed.h"

// which pin to use for reading the sensor? can use any pin!
#define FLOWSENSORPIN p22

Ticker ticker;

DigitalIn sensor(FLOWSENSORPIN);

// count how many pulses!
volatile uint16_t pulses = 0;
// track the state of the pulse pin
volatile int lastflowpinstate;
// you can try to keep time of how long it is between pulses
volatile uint32_t lastflowratetimer = 0;
// and use that to calculate a flow rate
volatile float flowrate;
// Interrupt is called once a millisecond, looks for any pulses from the sensor!
void tick() {
  int x = sensor;
  
  if (x == lastflowpinstate) {
    lastflowratetimer++;
    return; // nothing changed!
  }
  
  if (x) {
    //low to high transition!
    pulses++;
  }
  lastflowpinstate = x;
  flowrate = 1000.0;
  flowrate /= lastflowratetimer;  // in hertz
  lastflowratetimer = 0;
}

int main() {
    lastflowpinstate = sensor;
    ticker.attach(&tick, 0.001);
    
    while(1) {
        printf("Pulses: %d Hz: %f\n\r", pulses, flowrate);
         
        // if a plastic sensor use the following calculation
        // Sensor Frequency (Hz) = 7.5 * Q (Liters/min)
        // Liters = Q * time elapsed (seconds) / 60 (seconds/minute)
        // Liters = (Frequency (Pulses/second) / 7.5) * time elapsed (seconds) / 60
        // Liters = Pulses / (7.5 * 60)
        float liters = pulses;
        liters /= 7.5;
        liters /= 60.0;
        
        printf("%f, Liters\n\r", liters);
        
        wait(0.1);
    }
}

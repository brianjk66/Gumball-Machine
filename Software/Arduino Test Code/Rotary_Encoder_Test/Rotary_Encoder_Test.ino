/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(22, 21);
//   avoid using pins with LEDs attached

void bang() {
   static unsigned long last_interrupt_time = 0;
   unsigned long interrupt_time = millis();
   // If interrupts come faster than 200ms, assume it's a bounce and ignore
   if (interrupt_time - last_interrupt_time > 200) 
   {
     Serial.println("Bang!");
   }
   last_interrupt_time = interrupt_time;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");

  pinMode(20, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(20), bang, FALLING);
}

long oldPosition  = -999;

void loop() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }
}

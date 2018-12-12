/*************************************************** 
  This is a library for the Adafruit Capacitive Touch Screens

  ----> http://www.adafruit.com/products/1947
 
  Check out the links above for our tutorials and wiring diagrams
  This chipset uses I2C to communicate

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include "mbed.h"
#include <Adafruit_FT6206.h>


//#define FT6206_DEBUG
//#define I2C_DEBUG


/**************************************************************************/
/*! 
    @brief  Instantiates a new FT6206 class
*/
/**************************************************************************/
// I2C, no address adjustments or pins
Adafruit_FT6206::Adafruit_FT6206(I2C *i2c) 
  : _i2c(i2c){
  _i2c->frequency(FT62XX_DEFAULT_FREQUENCY);
  touches = 0;
}


/**************************************************************************/
/*! 
    @brief  Setups the I2C interface and hardware, identifies if chip is found
    @param  thresh Optional threshhold-for-touch value, default is FT6206_DEFAULT_THRESSHOLD but you can try changing it if your screen is too/not sensitive.
    @returns True if an FT6206 is found, false on any failure
*/
/**************************************************************************/
bool Adafruit_FT6206::begin(char thresh) {

#ifdef FT6206_DEBUG
  printf("Vend ID: 0x%x\n\r", readRegister8(FT62XX_REG_VENDID));
  printf("Chip ID: 0x%x\n\r", readRegister8(FT62XX_REG_CHIPID));
  printf("Firm V: %d\n\r", readRegister8(FT62XX_REG_FIRMVERS));
  printf("Point Rate Hz: %d\n\r", readRegister8(FT62XX_REG_POINTRATE));
  printf("Thresh: %d\n\r", readRegister8(FT62XX_REG_THRESHHOLD));

  // dump all registers
  for (int16_t i=0; i<0x10; i++) {
    printf("I2C $%x = 0x%x\n\r", i, readRegister8(i));
  }
#endif

  // change threshhold to be higher/lower
  writeRegister8(FT62XX_REG_THRESHHOLD, thresh);
  
  if (readRegister8(FT62XX_REG_VENDID) != FT62XX_VENDID) {
    return false;
  }
  char id = readRegister8(FT62XX_REG_CHIPID);
  if ((id != FT6206_CHIPID) && (id != FT6236_CHIPID) && (id != FT6236U_CHIPID)) {
    return false;
  }

  return true;
}

/**************************************************************************/
/*! 
    @brief  Determines if there are any touches detected
    @returns Number of touches detected, can be 0, 1 or 2
*/
/**************************************************************************/
char Adafruit_FT6206::touched(void) {
  char n = readRegister8(FT62XX_REG_NUMTOUCHES);
  if (n > 2) {
    n = 0;
  }
  return n;
}

/**************************************************************************/
/*! 
    @brief  Queries the chip and retrieves a point data
    @param  n The # index (0 or 1) to the points we can detect. In theory we can detect 2 points but we've found that you should only use this for single-touch since the two points cant share the same half of the screen.
    @returns {@link TS_Point} object that has the x and y coordinets set. If the z coordinate is 0 it means the point is not touched. If z is 1, it is currently touched.
*/
/**************************************************************************/
TS_Point Adafruit_FT6206::getPoint(char n) {
  readData();
  if ((touches == 0) || (n > 1)) {
    return TS_Point(0, 0, 0);
  } else {
    return TS_Point(touchX[n], touchY[n], 1);
  }
}

/************ lower level i/o **************/

/**************************************************************************/
/*! 
    @brief  Reads the bulk of data from captouch chip. Fill in {@link touches}, {@link touchX}, {@link touchY} and {@link touchID} with results
*/
/**************************************************************************/
void Adafruit_FT6206::readData(void) {

  char i2cdat[16] = {0};

  _i2c->write((FT62XX_ADDR << 1), 0x00, 1);  
  _i2c->read((FT62XX_ADDR << 1), i2cdat, 16);

#ifdef FT6206_DEBUG
  for (int16_t i=0; i<16; i++) {
    printf("I2C $%x = 0x%x\n\r", i, i2cdat[i]);
  }
#endif

  touches = i2cdat[0x02];
  if ((touches > 2) || (touches == 0)) {
    touches = 0;
  }

#ifdef FT6206_DEBUG
  printf("# Touches: %d\n\r", touches);

  for (char i=0; i<16; i++) {
    printf("0x%x ", i2cdat[i]);
  }
  printf("\n\r");
  if (i2cdat[0x01] != 0x00) {
    printf("Gesture #%d\n\r", i2cdat[0x01]);
  }
#endif

  for (char i=0; i<2; i++) {
    touchX[i] = i2cdat[0x03 + i*6] & 0x0F;
    touchX[i] <<= 8;
    touchX[i] |= i2cdat[0x04 + i*6]; 
    touchY[i] = i2cdat[0x05 + i*6] & 0x0F;
    touchY[i] <<= 8;
    touchY[i] |= i2cdat[0x06 + i*6];
    touchID[i] = i2cdat[0x05 + i*6] >> 4;
  }

#ifdef FT6206_DEBUG
  printf("\n\r");
  for (char i=0; i<touches; i++) {
    printf("ID #%d\t(%d, %d) ", touchID[i], touchX[i], touchY[i]);
  }
  printf("\n\r");
#endif
}

char Adafruit_FT6206::readRegister8(char reg) {
  char x;

  // use i2c
  _i2c->write((FT62XX_ADDR << 1), &reg, 1, true);
  _i2c->read((FT62XX_ADDR << 1), &x, 1);

#ifdef I2C_DEBUG
  printf("$%x: 0x%x\n\r", reg, x); 
#endif

  return x;
}

void Adafruit_FT6206::writeRegister8(char reg, char val) {
  // use i2c
  char cmd[2];
  cmd[0] = reg;
  cmd[1] = val;
  _i2c->write(FT62XX_ADDR, cmd, 2);
}

/*

// DONT DO THIS - REALLY - IT DOESNT WORK
void Adafruit_FT6206::autoCalibrate(void) {
 writeRegister8(FT06_REG_MODE, FT6206_REG_FACTORYMODE);
 delay(100);
 //printf("Calibrating...\n\r");
 writeRegister8(FT6206_REG_CALIBRATE, 4);
 delay(300);
 for (char i = 0; i < 100; i++) {
   char temp;
   temp = readRegister8(FT6206_REG_MODE);
   printf("%x\n\r", temp);
   //return to normal mode, calibration finish 
   if (0x0 == ((temp & 0x70) >> 4))
     break;
 }
 delay(200);
 //printf("Calibrated\n\r");
 delay(300);
 writeRegister8(FT6206_REG_MODE, FT6206_REG_FACTORYMODE);
 delay(100);
 writeRegister8(FT6206_REG_CALIBRATE, 5);
 delay(300);
 writeRegister8(FT6206_REG_MODE, FT6206_REG_WORKMODE);
 delay(300);
}
*/

/****************/

/**************************************************************************/
/*! 
    @brief  Instantiates a new FT6206 class with x, y and z set to 0 by default
*/
/**************************************************************************/
TS_Point::TS_Point(void) {
  x = y = z = 0;
}

/**************************************************************************/
/*! 
    @brief  Instantiates a new FT6206 class with x, y and z set by params.
    @param  _x The X coordinate
    @param  _y The Y coordinate
    @param  _z The Z coordinate
*/
/**************************************************************************/

TS_Point::TS_Point(int16_t _x, int16_t _y, int16_t _z) {
  x = _x;
  y = _y;
  z = _z;
}

/**************************************************************************/
/*! 
    @brief  Simple == comparator for two TS_Point objects
    @returns True if x, y and z are the same for both points, False otherwise.
*/
/**************************************************************************/
bool TS_Point::operator==(TS_Point p1) {
  return  ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

/**************************************************************************/
/*! 
    @brief  Simple != comparator for two TS_Point objects
    @returns False if x, y and z are the same for both points, True otherwise.
*/
/**************************************************************************/
bool TS_Point::operator!=(TS_Point p1) {
  return  ((p1.x != x) || (p1.y != y) || (p1.z != z));
}




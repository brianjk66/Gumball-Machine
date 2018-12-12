#ifndef _ADAFRUIT_SPITFT_
#define _ADAFRUIT_SPITFT_

#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SPITFT_Macros.h"

/// A heavily optimized SPI display subclass of GFX. Manages SPI bitbanging, transactions, DMA, etc! Despite being called SPITFT, the classic SPI data/command interface is also used by OLEDs.
class Adafruit_SPITFT : public Adafruit_GFX {
    public:
        Adafruit_SPITFT(uint16_t w, uint16_t h, PinName _CS, PinName _DC, PinName _MOSI, PinName _SCLK, PinName _RST = NC, PinName _MISO = NC);
        Adafruit_SPITFT(uint16_t w, uint16_t h, SPI *spi, PinName _CS, PinName _DC, PinName _RST = NC);
        
        virtual void begin(uint32_t freq) = 0;  ///< Virtual begin() function to set SPI frequency, must be overridden in subclass. @param freq Maximum SPI hardware clock speed

        void initSPI(uint32_t freq);

        // Required Non-Transaction
        virtual void drawPixel(int16_t x, int16_t y, uint16_t color);

        // Transaction API
        virtual void startWrite(void);
        virtual void endWrite(void);

        virtual void writePixel(int16_t x, int16_t y, uint16_t color);
        virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

        // Transaction API not used by GFX

        /*!
          @brief   SPI displays set an address window rectangle for blitting pixels
          @param  x  Top left corner x coordinate
          @param  y  Top left corner x coordinate
          @param  w  Width of window
          @param  h  Height of window
        */
        virtual void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;

        /*!
          @brief   Write a 2-byte color  (must have a transaction in progress)
          @param    color 16-bit 5-6-5 Color to draw
        */
        void inline writePixel(uint16_t color) { SPI_WRITE16(color); }
        void        writePixels(uint16_t * colors, uint32_t len);
        void        writeColor(uint16_t color, uint32_t len);
        void        pushColor(uint16_t color);

        // Recommended Non-Transaction
        virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

        using Adafruit_GFX::drawRGBBitmap; // Check base class first

                void drawRGBBitmap(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h);
        virtual void invertDisplay(bool i);

        uint16_t  color565(uint8_t r, uint8_t g, uint8_t b);

    protected:
        SPI        *_spi;           ///< The SPI device we want to use (set in constructor)
        uint32_t    _freq;          ///< SPI clock frequency (for hardware SPI)
        PinName     _cs;            ///< mbed PinName for chip-select pin
        DigitalOut *_csPin;         ///< mbed chip-select DigitalOut
        PinName     _dc;            ///< mbed PinName for data-command pin
        DigitalOut *_dcPin;         ///< mbed data-command DigitalOut
        PinName     _rst;           ///< mbed PinName for reset pin
        DigitalOut *_rstPin;        ///< mbed reset DigitalOut 
        PinName     _sclk;          ///< mbed PinName for SPI clock pin 
        PinName     _mosi;          ///< mbed PinName for SPI MOSI pin 
        PinName     _miso;          ///< mbed PinName for SPI MISO pin 

        void        writeCommand(uint8_t cmd);
        void        spiWrite(uint8_t v);
        uint8_t     spiRead(void);

        uint8_t   invertOnCommand;      ///<  SPI command byte to turn on invert
        uint8_t   invertOffCommand;     ///<  SPI command byte to turn off invert
        uint16_t   _xstart;             ///< Many displays don't have pixels starting at (0,0) of the internal framebuffer, this is the x offset from 0 to align
        uint16_t   _ystart;             ///< Many displays don't have pixels starting at (0,0) of the internal framebuffer, this is the y offset from 0 to align
};

#endif // !_ADAFRUIT_SPITFT_

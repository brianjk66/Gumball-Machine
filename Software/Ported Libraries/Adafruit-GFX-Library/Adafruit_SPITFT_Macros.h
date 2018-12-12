#ifndef _ADAFRUIT_SPITFT_MACROS
#define _ADAFRUIT_SPITFT_MACROS

/*
 * Control Pins
 * */
#define SPI_DC_HIGH() 	*_dcPin = 1
#define SPI_DC_LOW()    *_dcPin = 0
#define SPI_CS_HIGH()   *_csPin = 1
#define SPI_CS_LOW()    *_csPin = 0

/*
 * Hardware SPI Macros
 * */
#define HSPI_SET_CLOCK() _spi->frequency(_freq)

#define HSPI_BEGIN_TRANSACTION() _spi->format(8, 0)
#define HSPI_END_TRANSACTION()   

#define HSPI_WRITE(b)            _spi->write((uint8_t)(b))
#define HSPI_READ()              HSPI_WRITE(0)
#define HSPI_WRITE16(s)          HSPI_WRITE((s) >> 8); HSPI_WRITE(s)
#define HSPI_WRITE32(l)          HSPI_WRITE((l) >> 24); HSPI_WRITE((l) >> 16); HSPI_WRITE((l) >> 8); HSPI_WRITE(l)
#define HSPI_WRITE_PIXELS(c,l)   for(uint32_t i=0; i<(l); i+=2){ HSPI_WRITE(((uint8_t*)(c))[i+1]); HSPI_WRITE(((uint8_t*)(c))[i]); }

#define SPI_BEGIN()             HSPI_SET_CLOCK()
#define SPI_BEGIN_TRANSACTION() HSPI_BEGIN_TRANSACTION()
#define SPI_END_TRANSACTION()   HSPI_END_TRANSACTION()
#define SPI_WRITE16(s)          HSPI_WRITE16(s)
#define SPI_WRITE32(l)          HSPI_WRITE32(l)
#define SPI_WRITE_PIXELS(c,l)   HSPI_WRITE_PIXELS(c,l)

#endif // _ADAFRUIT_SPITFT_MACROS

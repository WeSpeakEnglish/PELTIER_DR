#include "stm32F10x.h"

typedef union {
    int16_t value;
    uint8_t bytes[2];
} int16andUint8_t;

typedef union {
    int32_t value;
    uint8_t bytes[4];
} int32andUint8_t;

typedef union {
    uint16_t value;
    uint8_t bytes[2];
} uint16andUint8_t;

///////////////////////////////////////////////////////////////////////////////
// BMP085 Variables
///////////////////////////////////////////////////////////////////////////////


#define BMP085_ADDRESS 0x77


///////////////////////////////////


//#define OSS 0  //  4.5 mSec conversion time (222.22 Hz)
#define OSS 1                   //  7.5 mSec conversion time (133.33 Hz)
//#define OSS 2  // 13.5 mSec conversion time ( 74.07 Hz)
//#define OSS 3  // 25.5 mSec conversion time ( 39.22 Hz)


///////////////////////////////////////


#define BMP085_PROM_START_ADDR  0xAA
#define BMP085_PROM_DATA_LEN    22


#define BMP085_T_MEASURE        0x2E    // temperature measurement
#define BMP085_P_MEASURE        0x34 + (OSS<<6) // pressure measurement
#define BMP085_CTRL_MEAS_REG    0xF4
#define BMP085_ADC_OUT_MSB_REG  0xF6

void calculatePressureAltitude(void);


///////////////////////////////////////////////////////////////////////////////
// Pressure Initialization
///////////////////////////////////////////////////////////////////////////////

//void initPressure(void);

void bmp085_init(void);
int16_t bmp085_read_temperature(void);
int32_t bmp085_read_pressure();
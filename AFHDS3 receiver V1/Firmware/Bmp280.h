#if !defined(BMP280_H)
#define BMP280_H

#include "System definitions.h"
#include "i-Bus.h"

#define BMP280_ADDR				        (0x76)
#define BMP280_DEFAULT_CHIP_ID			(0x58)

#define BMP280_CHIP_ID					(0xD0)  /* Chip ID Register */
#define BMP280_RST_REG					(0xE0)  /* Softreset Register */
#define BMP280_STAT_REG					(0xF3)  /* Status Register */
#define BMP280_CTRL_MEAS_REG			(0xF4)  /* Ctrl Measure Register */
#define BMP280_CONFIG_REG				(0xF5)  /* Configuration Register */
#define BMP280_PRESSURE_MSB_REG			(0xF7)  /* Pressure MSB Register */
#define BMP280_PRESSURE_LSB_REG			(0xF8)  /* Pressure LSB Register */
#define BMP280_PRESSURE_XLSB_REG		(0xF9)  /* Pressure XLSB Register */
#define BMP280_TEMPERATURE_MSB_REG		(0xFA)  /* Temperature MSB Reg */
#define BMP280_TEMPERATURE_LSB_REG		(0xFB)  /* Temperature LSB Reg */
#define BMP280_TEMPERATURE_XLSB_REG		(0xFC)  /* Temperature XLSB Reg */

#define BMP280_SLEEP_MODE				(0x00)
#define BMP280_FORCED_MODE				(0x01)
#define BMP280_NORMAL_MODE				(0x03)

#define BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG             (0x88)
#define BMP280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH       (24)
#define BMP280_DATA_FRAME_SIZE			(6)


// osrs_p / osrs_t
#define BMP280_OVERSAMP_SKIPPED			(0x00)
#define BMP280_OVERSAMP_1X				(0x01)
#define BMP280_OVERSAMP_2X				(0x02)
#define BMP280_OVERSAMP_4X				(0x03)
#define BMP280_OVERSAMP_8X				(0x04)
#define BMP280_OVERSAMP_16X				(0x05)


// IIR filter
#define BMP280_FILTER_OFF				(0x00)
#define BMP280_FILTER_2					(0x01)
#define BMP280_FILTER_4					(0x02)
#define BMP280_FILTER_8					(0x03)
#define BMP280_FILTER_16				(0x04)


// t_sb setting, resolution is 0.1 ms
#define BMP280_TSB_05					(0x00)
#define BMP280_TSB_625					(0x01)
#define BMP280_TSB_1250					(0x02)
#define BMP280_TSB_2500					(0x03)
#define BMP280_TSB_5000					(0x04)
#define BMP280_TSB_10000				(0x05)
#define BMP280_TSB_20000				(0x06)
#define BMP280_TSB_40000				(0x07)

typedef struct __attribute__((packed))
{
    unsigned short  dig_T1; /* calibration T1 data */
    signed short    dig_T2; /* calibration T2 data */
    signed short    dig_T3; /* calibration T3 data */
    unsigned short  dig_P1; /* calibration P1 data */
    signed short    dig_P2; /* calibration P2 data */
    signed short    dig_P3; /* calibration P3 data */
    signed short    dig_P4; /* calibration P4 data */
    signed short    dig_P5; /* calibration P5 data */
    signed short    dig_P6; /* calibration P6 data */
    signed short    dig_P7; /* calibration P7 data */
    signed short    dig_P8; /* calibration P8 data */
    signed short    dig_P9; /* calibration P9 data */
    signed long     t_fine; /* calibration t_fine data */
} sBmp280_Calib;

extern sIB_AS_Pressure         Bmp280_AS_Pressure;

BOOL Bmp280_Init(void);
void Bmp280_GetData(unsigned long* pPressure, signed long* pTemperature);
void Bmp280_Background(void);
#endif // !defined(BMP280_H)


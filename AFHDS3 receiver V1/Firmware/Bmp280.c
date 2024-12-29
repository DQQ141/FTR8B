#include <stm32g0xx.h>
#include "System definitions.h"
#include "System.h"
#include <math.h>
#include "stdint.h"

#include "I2C.h"

#include "Bmp280.h"


sIB_AS_Pressure         Bmp280_AS_Pressure;
static unsigned long  	Bmp280_Pressure;
static signed long		Bmp280_Temperature;
static unsigned long  	Bmp280_SysTickMs;
static sBmp280_Calib  	Bmp280_Calib;


static BOOL Bmp280_Read(unsigned long Address,void *pData,unsigned long DataLength)
{
	if (!I2C_Write(BMP280_ADDR,&Address,1,FALSE) ||
		!I2C_Read(BMP280_ADDR,pData,DataLength,TRUE))
		return FALSE;
	else
		return TRUE;
}

static BOOL Bmp280_Write(unsigned char *pData,unsigned long Length)
{	
    I2C_Write(BMP280_ADDR,pData,Length,TRUE);
	return TRUE;
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of "5123" equals 51.23 DegC
// t_fine carries fine temperature as global value
static int Bmp280_CompensateT(int adcT)
{
    int var1, var2, T;

    var1 = ((((adcT >> 3) - ((int)Bmp280_Calib.dig_T1 << 1))) * ((int)Bmp280_Calib.dig_T2)) >> 11;
    var2  = (((((adcT >> 4) - ((int)Bmp280_Calib.dig_T1)) * ((adcT >> 4) - ((int)Bmp280_Calib.dig_T1))) >> 12) * ((int)Bmp280_Calib.dig_T3)) >> 14;
    Bmp280_Calib.t_fine = var1 + var2;	
    T = (Bmp280_Calib.t_fine * 5 + 128) >> 8;
    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of "24674867" represents 24674867/256 = 96386.2 Pa = 963.862 hPa
static unsigned int Bmp280_CompensateP(int adcP)
{
    int64_t var1, var2, p;
    var1 = ((int64_t)Bmp280_Calib.t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)Bmp280_Calib.dig_P6;
    var2 = var2 + ((var1*(int64_t)Bmp280_Calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)Bmp280_Calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)Bmp280_Calib.dig_P3) >> 8) + ((var1 * (int64_t)Bmp280_Calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)Bmp280_Calib.dig_P1) >> 33;
    if (var1 == 0)
        return 0;
    p = 1048576 - adcP;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)Bmp280_Calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)Bmp280_Calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)Bmp280_Calib.dig_P7) << 4);
    return (unsigned int)p;
}

// Converts pressure to altitude above sea level (ASL) in meters
#define CONST_PF 0.1902630958	//(1/5.25588f) Pressure factor
#define FIX_TEMP 25				// Fixed Temperature. ASL is a function of pressure and temperature, but as the temperature changes so much (blow a little towards the flie and watch it drop 5 degrees) it corrupts the ASL estimates.
								// TLDR: Adjusting for temp changes does more harm than good.
static volatile double Bmp280_Altitude;
static double Bmp280_PressureToAltitude(double pressure)
{
    if (pressure > 0)
    {
        return ((pow((1015.7 / pressure), CONST_PF) - 1.0) * (FIX_TEMP + 273.15)) / 0.0065;
    }
    else
    {
        return 0;
    }
}




BOOL Bmp280_Init(void)
{		
    /* Read bmp280 ID*/
	{
		unsigned char bmp280ID ;
		Bmp280_Read(BMP280_CHIP_ID,&bmp280ID,sizeof(bmp280ID));
		if(bmp280ID != BMP280_DEFAULT_CHIP_ID)
		{
			 return FALSE;       
		}
	}
	
    /* Read Calibration data */
    Bmp280_Read(BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG,&Bmp280_Calib,sizeof(sBmp280_Calib));
    
    /* Reset */
	{
		unsigned char RST_Value[2]={BMP280_RST_REG,0xB6};
		Bmp280_Write(RST_Value,sizeof(RST_Value));
    }
    
    /* work mode */
	{
		unsigned char BMP280_MODE[2];		
		BMP280_MODE[0]=BMP280_CTRL_MEAS_REG;
		// Temperature 2X(0.0028 DegC) /  Pressure 16X(0.16 Pa)  /  Normal mode 
		BMP280_MODE[1]=( BMP280_OVERSAMP_2X << 5 | BMP280_OVERSAMP_16X << 2 |BMP280_NORMAL_MODE);
		Bmp280_Write(BMP280_MODE,sizeof(BMP280_MODE));
	}
    
    /*IIR Filter*/
	{
		unsigned char IIT_Filter[2];		
		IIT_Filter[0]=BMP280_CONFIG_REG;
		IIT_Filter[1]=BMP280_TSB_05<<5 | BMP280_FILTER_16<<2;
		Bmp280_Write(IIT_Filter,sizeof(IIT_Filter));
	}   	
    return TRUE;
}

void Bmp280_GetData(unsigned long* pPressure, signed long* pTemperature)
{	
	unsigned char data[BMP280_DATA_FRAME_SIZE];
	signed long bmp280RawPressure ;
	signed long bmp280RawTemperature;
	
	// read data from sensor   
    Bmp280_Read(BMP280_PRESSURE_MSB_REG,&data[0],sizeof(data));
    bmp280RawPressure = (int)((((uint32_t)(data[0])) << 12) | (((uint32_t)(data[1])) << 4) | ((uint32_t)data[2] >> 4));
    bmp280RawTemperature = (int)((((uint32_t)(data[3])) << 12) | (((uint32_t)(data[4])) << 4) | ((uint32_t)data[5] >> 4));

	*pPressure = Bmp280_CompensateP(bmp280RawPressure)>>8;
	*pTemperature = Bmp280_CompensateT(bmp280RawTemperature);	
	return;	
}

void Bmp280_Background(void)
{
	if(SYS_SysTickMs-Bmp280_SysTickMs>50)
	{
		Bmp280_SysTickMs=SYS_SysTickMs;
		Bmp280_GetData(&Bmp280_Pressure,&Bmp280_Temperature);
		Bmp280_AS_Pressure.Pressure=Bmp280_Pressure;
		if(Bmp280_Temperature<-4000)
		{
			Bmp280_AS_Pressure.Temperature=0;
		}
		else
		{
			Bmp280_AS_Pressure.Temperature=(unsigned long)((Bmp280_Temperature+4000)/10);
		}	
		
		//!!! TEST 
		/*×ª»»³Éº£°Î*/	
		Bmp280_Altitude=Bmp280_PressureToAltitude((double)Bmp280_AS_Pressure.Pressure/100.0);
	}
	return;
}




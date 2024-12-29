#ifndef __GYROFC_H_
#define __GYROFC_H_

#include "SES AFHDS3.h"
#include "Main.h"


#define	GYROFC_ADDRESS	0x6A

#define FILTER_NUM 10
enum 
{
	Gx=0,
	Gy,
	Gz,
	Ax,
	Ay,
	Az,
	ITEM,
};

//InstallationDirection 
typedef enum
{
    LevelForward=0,
    LeftForward,
    RightForward 
}eSES_InstallDir;

typedef struct __attribute__((packed))
{
	signed short x;
	signed short y;
	signed short z;
	signed short PAD;
}sSensor_ShotrData;


typedef struct __attribute__((packed))
{
	float x;
	float y;
	float z;
}sSensor_FloatData;

typedef struct __attribute__((packed))
{	
	sSensor_ShotrData  OriginalGyro;
	sSensor_ShotrData  OriginalAcc;

	sSensor_ShotrData  OriginalGyroPre;
	sSensor_ShotrData  OriginalAccPre;
	
	sSensor_ShotrData  CalibratGyro;
	sSensor_ShotrData  CalibratAcc;
	
	sSensor_FloatData  OutGyro;
	sSensor_FloatData  OutAcc;
    
    eSES_InstallDir Installdir;

	unsigned char  CalibrationFlg:4;
	unsigned char  CalibratDataSaveFlg:4;


} sFC_SensorData;



extern void GyroFC_Init(void);
extern void GyroFC_DataDeal(sFC_SensorData *FC_GyroData);


#endif


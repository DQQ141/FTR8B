#ifndef __GYRO_H_
#define __GYRO_H_

#include "SES AFHDS3.h"
#include "Main.h"

#define	GYRO_ADDRESS	0x68

#define STEER_LOCKMODE_KEPP_BUF 350  	// about >10us
#define THR_BREAK_MIDDLE_BUF 350     	// about >10us
#define BREAK_STOP_SVC_MSNUM 500  		// 1s
#define GYRO_ZDATA_INTERATION_MAX 1638
#define THROTTLE_MIDDLE_MAX 500  		// 1s

#define KP_THROTTLE  1
#define KI_THROTTLE  1
#define KD_THROTTLE  0.5

#define KP_STEERING  1
#define KI_STEERING  1
#define KD_STEERING  1

typedef enum 
{
  MODE_STOP=0,
  MODE_FORWARD , 
  MODE_BACK,   
  MODE_BRAKE 
}uMoveMode;


typedef enum 
{
  SVC=0,
  GYROSCOPE 
}uCalibrationType;

typedef enum 
{
  CHECK_STATUS=0,
  START_CATIBRETIN
}uCalibrationCMD;


typedef struct
{
	unsigned long 	Length;
	signed short 	SteeringChMiddle; 
    signed short 	ThrottleChMiddle;  
    signed short 	GyroZCalibateData;
	unsigned short 	PAD;
	unsigned long	CRC32;
} sGyro_SVC_Calibration;

typedef struct
{	
    sSES_CA_SVC   *pSVC;
    sGyro_SVC_Calibration SVC_Calibration;

    signed short 	GyroZdata;
    signed short 	SteeringChInput; 
    signed short 	ThrottleChInput;   
    signed short 	SteeringChOutput;
    signed short 	ThrottleChOutput;    	
    signed short	GyroZDataTempPre;

    unsigned long	GyroCalibateCnt;	
    unsigned long  	Break_Num;	
    unsigned long  	AheadEndNum;
    unsigned long  	ThrottleChMiddleNUN;
    unsigned long   SteeringChMiddleNUN;
    signed   long 	GyroZdataUseIntegration;
    unsigned long 	Gyro_TaskMs;
    unsigned long 	Gyro_TaskMsPID;

    uMoveMode      	MoveMode;
    BOOL			SVC_CalibateFlag;	
    BOOL			Gyro_CalibateFlag;
    eSES_CA_CalibrateSVCStatus CalibrateSVCStatus;
    eSES_CA_CalibrateSVCStatus CalibrateGYROStatus;  
    BOOL             GyroConnecedFlg;  
    BOOL             GyroInitFlg;
    BOOL             CalibrationMidleRestFlg;	
	
//    unsigned char Reserved[1];
    
} sGyro_SVC_Vars;


 
extern sGyro_SVC_Vars  Gyro_SVC_Vars;


extern void Gyro_Init(void);
extern void Gyro_SVC_Background(void);
extern void Gyro_SVC_CalibrationBackground(sGyro_SVC_Vars *pVars, 
sSES_CA_CalibrateSVC *pCalibrateSVC, sSES_CA_CalibrateSVCResponse *pCalibrateSVCResponse);

#endif


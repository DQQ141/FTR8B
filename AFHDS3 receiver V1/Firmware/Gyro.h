#ifndef __GYRO_H_
#define __GYRO_H_

#include "SES AFHDS3.h"
#include "Main.h"

#define	GYRO_ADDRESS_MPU6887	0x68
#define	GYRO_ADDRESS_LSM6DSDTR	0x6A


#define STEERING_SLOW_TRIGGER       6553 // 16384/500*200
#define STEER_LOCKMODE_KEPP_BUF 350  	// about >10us
#define THR_BREAK_MIDDLE_BUF 350     	// about >10us
#define BREAK_STOP_SVC_MSNUM 500  		// 1s
#define GYRO_ZDATA_INTERATION_MAX 1638
#define THROTTLE_MIDDLE_MAX 600  		// 1s

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
  TURN=0,
  RETURN 
}uSteeringType;

typedef enum 
{
  CHECK_STATUS=0,
  START_CATIBRETIN
}uCalibrationCMD;

typedef enum 
{
  None=0,
  MPU6887=1,
  LSM6DSDTR
}uGyroType;

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
    sSES_CA_ADVANCED_SVC *pAdvancedSVC;
    
    signed short 	GyroZdata;
    signed short 	SteeringChInput; 
    signed short 	ThrottleChInput;  
	signed short	CH3;
	signed short    CH4;
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
    unsigned long 	Gyro_TaskMs1;
    unsigned long 	Gyro_TaskMs2;
    unsigned long 	Gyro_TaskMsPID;

    uMoveMode      	MoveMode;
    BOOL			SVC_CalibateFlag;	
    BOOL			Gyro_CalibateFlag;
    eSES_CA_CalibrateSVCStatus CalibrateSVCStatus;
    eSES_CA_CalibrateSVCStatus CalibrateGYROStatus;  
    BOOL             GyroConnecedFlg;  
    BOOL             GyroInitFlg;
    BOOL             CalibrationMidleRestFlg;
    unsigned char    *RXType;// 0=1-way transmitter, 1=feedback every cycle, 2=feedback every 2 cycles... up to 15
	uGyroType GyroType;	
    uSteeringType SteeringType;

    unsigned char SupportAdvancedSVC;
    unsigned char Reserved;   
} sGyro_SVC_Vars;


 
extern sGyro_SVC_Vars  Gyro_SVC_Vars;

extern void Gyro_Init(void);
extern void Gyro_SVC_Background(void);
extern void Gyro_SVC_CalibrationBackground(sGyro_SVC_Vars *pVars, 
sSES_CA_CalibrateSVC *pCalibrateSVC, sSES_CA_CalibrateSVCResponse *pCalibrateSVCResponse);

#endif


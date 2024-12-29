#ifndef __FC_H_
#define __FC_H_

#include "GyroFC.h"
#include "IMU.h"
#include "PID FC.h"
#include "RX flight controller.h"

//#define  LIMIT(In,Max,Min)  (((In)>(Max))?(Max):(((In)<(Min))?(Min):(In)))
#define  ABS(X)  (((X)>0)?(X):-(X))

#define AGLE_ROLL_MAX   60

#define AGLE_PITCH_MAX 60

#define AGLE_PITCH_MAX_UP   50
#define AGLE_PITCH_MAX_DOWN 30

#define AGLE_YAW_CHINPUT_ZONE 50

#define GYRO_ROLL_MAX   250  
#define ANGLE_ROLLOUTPUT_MAX  500 //GYRO_ROLL_MAX*0.1

#define GYRO_PITCH_MAX  250 
#define ANGLE_PITCHOUTPUT_MAX 500 
#define ANGLE_PITCHOUTPUT_MAX_DOWN 250

#define GYRO_YAW_MAX    100  
#define ANGLE_YAWOUTPUT_MAX 250 

#define CHMAX 16384

enum 
{
	ROLL=0,
	PITCH,
	THROTTLE,
	YAW,
	FC_CH_ITEM
};
//
typedef struct __attribute__((packed))
{
	unsigned short Kp; // From 0 to 655.35  0.01
	unsigned short Ki; // From 0 to 655.35  0.01
	unsigned short Kd; // From 0 to 655.35  0.01
} sPIDParameter ;

//FlykMode
typedef enum
{
	Manual=0,
	Stabilizing,
	Attitude	
}eSES_CMD_FlykMode;


////////////////////////////////////////////////////////////////////////////////////////////////////
//
typedef enum
{
	SES_CMD_FCTYPE_SYNCHRONIZE,
	SES_CMD_FCTYPE_SET,
	SES_CMD_FCTYPE_REPONSE
} eSES_CMD_FCType;

//CMD List
typedef enum
{
    SES_CMD_FC_DIRECTION,
    SES_CMD_FC_CALIBRATION,
    SES_CMD_FC_MODE_CHN,
    SES_CMD_FC_FLY_MODE,
    SES_CMD_FC_SWITCH, 
    SES_CMD_FC_GAIN,
    SES_CMD_FC_GAIN_REDUCE,
    SES_CMD_FC_CH_GAIN,
    SES_CMD_FC_CH_MIDDLE,
	SES_CMD_FC_CH_ENDPOINT,	
	SES_CMD_FC_REVERSE,  
    SES_CMD_FC_ROLL_RECUP,
    SES_CMD_FC_PITCH_RECUP,
    SES_CMD_NUM   
} eSES_CMD_FCList;


//InstallationDirection 
//typedef enum
//{
//    LevelForward=0,
//    LeftForward,
//    RightForward 
//}eSES_InstallDir;

//Calibration
typedef enum
{
	SES_FC_NO_CALIBRATION=0,     // No calibration has been initiated
    SES_FC_START_CALIBRATION,  // Start calibration
	SES_FC_IN_PROGRESS,        // Calibration in progress
    SES_FC_END_CALIBRATION,    // Over Calibration
	SES_FC_SUCCESSFUL,         // Calibration successful
	SES_FC_FAILED             // Calibration failed
} eSES_CA_CalibrateStatus;

//
typedef struct __attribute__((packed))
{
	unsigned char Roll;
    unsigned char Pitch;
    unsigned char Yaw;
} sSES_FC_Switch;

//
typedef struct __attribute__((packed))
{
   	unsigned char Basic_Roll;
    unsigned char Basic_Pitch;
    unsigned char Basic_Yaw;
 
   	unsigned char Attitude_Roll;
    unsigned char Attitude_Pitch;
    unsigned char Attitude_Yaw;
} sSES_FC_Gain;

//GainReduce
typedef enum
{
    None=0,
    Stable,
    Odrinary,
    Sport,
    Stunts_3D
}eSES_GainReduceType; 
typedef struct __attribute__((packed))
{
	eSES_GainReduceType eRoll;
    eSES_GainReduceType ePitch;
    eSES_GainReduceType eYaw; 
} sSES_FC_GainReduce;

//CH_GAIN
typedef struct __attribute__((packed))
{
	unsigned char Roll;
    unsigned char Pitch;
    unsigned char Yaw;
} sSES_FC_ChInput_Gain;

//
typedef struct __attribute__((packed))
{
	signed short Roll;
    signed short Pitch;
    signed short Yaw;
} sSES_FC_ChInput_Middle;

//
typedef struct __attribute__((packed))
{
    unsigned char High_Roll;
    unsigned char High_Pitch;
    unsigned char High_Yaw;
    
    unsigned char Low_Roll;
    unsigned char Low_Pitch;
    unsigned char Low_Yaw;
} sSES_FC_ChInput_Endpoint;


//
typedef struct __attribute__((packed))
{
   	unsigned char Roll;
    unsigned char Pitch;
    unsigned char Yaw;
} sSES_FC_ChInput_Reverse;

//
typedef struct __attribute__((packed))
{
    eSES_InstallDir           InstallDir;
    eSES_CA_CalibrateStatus   CalibrateStatus;
    unsigned char             ModeSwitchChn;
    eSES_CMD_FlykMode         FlyMode;
    sSES_FC_Switch            Switch;
    sSES_FC_Gain              Gain;
    sSES_FC_GainReduce        GainReduce;
    sSES_FC_ChInput_Gain      CH_Gain;
    sSES_FC_ChInput_Middle    CH_Middle;
    sSES_FC_ChInput_Endpoint  CH_Endpoint;
    sSES_FC_ChInput_Reverse   CH_Reverse;
    signed char               Roll_Recup; //-15to15
    signed char               Pitch_Recup; //-15to15
} sSES_FC_TXCMD;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct __attribute__((packed))
{
	eSES_CMD_FCList FCList;
	eSES_CMD_FCType FCType;
	void* FC_CMD_Data;
	unsigned char FC_CMD_DataLenth;
} sSES_CA_FC;

typedef struct __attribute__((packed))
{
	signed short 	CalibratGyroX;
	signed short 	CalibratGyroY;
	signed short 	CalibratGyroZ;	
	signed short 	CalibratAccX;
	signed short 	CalibratAccY;
	signed short 	CalibratAccZ;
} sSensor_Calibration_Save;

typedef struct __attribute__((packed))
{
	unsigned long 	Length;	
	sSensor_Calibration_Save Sensor_Calibration_Save;
	sSES_FC_TXCMD   FC_TxSet_Save;	
	unsigned long	CRC32;
} sFC_Data_Save;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct __attribute__((packed))
{
	unsigned long FC_SysTickMs;
	signed short RollChInput;
	signed short PitchChInput;
	signed short ThrottleChInput;
	signed short YawChInput;
	signed short ModeChInput;


	signed short RollChOutput;
	signed short PitchChOutput;
	signed short ThrottleCHOutput;
	signed short YawChOutput;
	
	
	signed short RollChOutputMax_H;
	signed short RollChOutputMax_L;
	signed short PitchChOutputMax_H;
	signed short PitchChOutputMax_L;
	signed short YawChOutputMax_H;
	signed short YawChOutputMax_L;
	
	sPID_Controller AngleRoll;
	sPID_Controller AnglePitch;

	sPID_Controller GyroRoll; 
	sPID_Controller GyroPitch; 
	sPID_Controller GyroYaw; 
    
    sPID_Controller GyroRoll_Stabiliz;
    sPID_Controller GyroPitch_Stabiliz;
  	sPID_Controller GyroYaw_Stabiliz;   
    BOOL	SensorCalibrationFlg;	
	eSES_CMD_FlykMode FlyMode;
	
} sControl_Vars;



typedef struct __attribute__((packed))
{
	sFC_Data_Save   FC_Data_Save;
	BOOL DataSaveFlg;
	sSES_FC_TXCMD	FC_TXCMD;
	sFC_SensorData  Sensor;
	sIMU_Data       Imu;
	sControl_Vars   Control;
}sFC_Vars;

extern sFC_Vars FC_Vars;
extern void FC_Init(void);
extern void FC_Background(void);
extern void FC_TxSetBackground(const void *pArgument,unsigned long ArgumentLength,unsigned long* Response,unsigned char *ResponseLenth);

#endif


#if defined(PN_INR6_FC)
#include <stm32g0xx.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "I2C.h"
#include "EEPROM.h"
#include "AFHDS3 CRC library.h"
#include "FC.h"

#ifdef DEBUG

#include <stm32g0xx.h>
#include <string.h>
#include "System.h"


#define UART_DMAMUX1_RX          	DMAMUX1_Channel0
#define UART_DMAMUX1_TX          	DMAMUX1_Channel1

#define UART_DMA_CHANNEL_RX          DMA1_Channel1
#define UART_DMA_CHANNEL_TX          DMA1_Channel2

#define USART1_BUFFER_SIZE 			    128
extern unsigned char USART_RXBuffer[USART1_BUFFER_SIZE];
extern unsigned char USART_TXBuffer[USART1_BUFFER_SIZE];


static _Bool USART_Initialized;
unsigned char USART_TXBuffer[USART1_BUFFER_SIZE];
unsigned char USART_RXBuffer[USART1_BUFFER_SIZE];

static void USART_Init1( void )
{
    USART_Initialized=FALSE;
    // Set PA10, PA10 to alternate USART_TX USART_RX mode
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(9*2))) | (2<<(9*2));
    GPIOA->MODER=(GPIOA->MODER&~(3UL<<(10*2))) | (0<<(10*2));
	GPIOA->AFR[1]=(GPIOA->AFR[1]&~(0x0FUL<<(1*4)))| (1<<(1*4));

    // Enable the USART1 module clock
	RCC->APBENR2 |= RCC_APBENR2_USART1EN;
	// Reset USART1
	RCC->APBRSTR2 |= RCC_APBRSTR2_USART1RST;
	RCC->APBRSTR2 &= ~RCC_APBRSTR2_USART1RST;
  
    USART1->CR1 = 0;
    USART1->CR2 = 0;   
    USART1->CR3 |= USART_CR3_ONEBIT| USART_CR3_OVRDIS;// | USART_CR3_TXFTCFG_0 | USART_CR3_TXFTCFG_2| USART_CR3_RXFTCFG_0 | USART_CR3_RXFTCFG_2;
     
    USART1->BRR=(SYSCLK+500000/2)/500000;
    // Initialize parameters for DMAMUX Channel
	UART_DMAMUX1_TX->CCR =51; //USART1_TX	
       
    USART1->CR1 |= USART_CR1_TE ;
    USART1->CR1 |= USART_CR1_UE;
    USART_Initialized=TRUE;
}

static void UART_TransmitData1( const uint8_t *pData, uint16_t Length )
{
    if( !USART_Initialized ) 
    {
        USART_Init1();
    }
    if( !Length || UART_DMA_CHANNEL_TX->CNDTR!=0) return;
    
    // Disable TX DMA
    //USART1->CR1 &= ~USART_CR1_TE;
	USART1->CR3 &= ~USART_CR3_DMAT;
	// Set up the TX DMA
	// Disable DMA
	UART_DMA_CHANNEL_TX->CCR&=~DMA_CCR_EN;
	UART_DMA_CHANNEL_TX->CPAR=(uint32_t)&USART1->TDR;
	UART_DMA_CHANNEL_TX->CMAR=(uint32_t)pData;
	UART_DMA_CHANNEL_TX->CNDTR=Length;
	// EN=1 DMA channel enabled
	// TCIE=0 Transfer complete interrupt disabled
	// HTIE=0 Half transfer interrupt disabled
	// TEIE=0 Transfer error interrupt disabled
	// DIR=1 Read from memory
	// CIRC=0 Circular mode disabled
	// PINC=0 Peripheral increment mode disabled
	// MINC=1 Memory increment mode enabled
	// PSIZE=0 8-bit peripheral size
	// MSIZE=0 8-bit memory size
	// PL=0 Low priority
	// MEM2MEM=0 Memory to memory mode disabled
	UART_DMA_CHANNEL_TX->CCR=DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_MINC;
    
    USART1->CR3 |= USART_CR3_DMAT; 
    //USART1->CR1 |= USART_CR1_TE; 
}

#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)		) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )


static unsigned char data_to_send[50];	

static void ANO_DT_Send_Senser(signed short a_x,signed short a_y,signed short a_z,signed short g_x,signed short g_y,signed short g_z)
{
	unsigned char  _cnt=0;
	signed short _temp;
	
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x02;
	data_to_send[_cnt++]=0;
	
	_temp = a_x;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = a_y;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = a_z;	
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	_temp = g_x;	
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = g_y;	
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = g_z;	
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	_temp = 0;	
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = 0;	
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = 0;	
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	data_to_send[3] = _cnt-4;
	
	unsigned char sum = 0;
	for(unsigned char i=0;i<_cnt;i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;
	
	UART_TransmitData1(data_to_send, _cnt);
}

static void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw)
{
	unsigned char _cnt=0;
	int _temp;
	signed long _temp2=0;
	
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x01;
	data_to_send[_cnt++]=0;
	
	_temp = (int)(angle_rol*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(angle_pit*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(angle_yaw*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	data_to_send[_cnt++]=BYTE3(_temp2);
	data_to_send[_cnt++]=BYTE2(_temp2);
	data_to_send[_cnt++]=BYTE1(_temp2);
	data_to_send[_cnt++]=BYTE0(_temp2);
	
	data_to_send[_cnt++] = 0;
	
	data_to_send[_cnt++] = 0;
	
	data_to_send[3] = _cnt-4;
	
	unsigned char sum = 0;
	for(unsigned char i=0;i<_cnt;i++)
		sum += data_to_send[i];
	data_to_send[_cnt++]=sum;
	
	UART_TransmitData1(data_to_send, _cnt);
}
#endif



sFC_Vars FC_Vars;
//
static sFC_Vars FC_Vars_Default={
    .FC_Data_Save=
    {
        .Sensor_Calibration_Save=
        {
            .CalibratGyroX=0,
            .CalibratGyroY=0,
            .CalibratGyroZ=0,
            .CalibratAccX=0,
            .CalibratAccY=0,
            .CalibratAccZ=0,
        },			
        .FC_TxSet_Save=
        {
            .InstallDir = LevelForward,
            .ModeSwitchChn = 0,
            .Switch.Roll = FALSE,
            .Switch.Pitch = FALSE,
            .Switch.Yaw = FALSE,
            .Gain.Basic_Roll=50,
            .Gain.Basic_Pitch=50,
            .Gain.Basic_Yaw=50,
            .Gain.Attitude_Roll=50,
            .Gain.Attitude_Pitch=50,
            .Gain.Attitude_Yaw=0,
            .GainReduce.eRoll=None,
            .GainReduce.ePitch=None,
            .GainReduce.eYaw=None,
            .CH_Gain.Roll=100,
            .CH_Gain.Pitch=100,
            .CH_Gain.Yaw=100,
            .CH_Middle.Roll=0,
            .CH_Middle.Pitch=0,
            .CH_Middle.Yaw=0,
            .CH_Endpoint.High_Roll=100,
            .CH_Endpoint.High_Pitch=100,
            .CH_Endpoint.High_Yaw=100,
            .CH_Endpoint.Low_Roll=100,
            .CH_Endpoint.Low_Pitch=100,
            .CH_Endpoint.Low_Yaw=100,
            .CH_Reverse.Roll=FALSE,
            .CH_Reverse.Pitch=FALSE,
            .CH_Reverse.Yaw=FALSE,
            .Roll_Recup=0,
            .Pitch_Recup=0,
        }
    },
	.Control=
	{
		.AngleRoll=
		{
			.ErrorLimitFlag = TRUE,
			.IntegrateLimitFlag = TRUE,
			.IntegrateSeparationFlag = TRUE,
			.Diff_ForwardFlag = TRUE,		
			.Error_Max = AGLE_ROLL_MAX,			
			.IntegrateSeparationError = 5,
			.Integrate_Max = 25,			
			.OutPut_Limit = ANGLE_ROLLOUTPUT_MAX,			
		},
		.AnglePitch=
		{
			.ErrorLimitFlag = TRUE,
			.IntegrateLimitFlag = TRUE,
			.IntegrateSeparationFlag = TRUE,
			.Diff_ForwardFlag = FALSE,		
			.Error_Max = AGLE_PITCH_MAX,			
			.IntegrateSeparationError = 5,
			.Integrate_Max = 25,			
			.OutPut_Limit = ANGLE_PITCHOUTPUT_MAX,			
		},
		.GyroRoll=
		{
			.ErrorLimitFlag = TRUE,
			.IntegrateLimitFlag = TRUE,
			.IntegrateSeparationFlag = TRUE,
			.Diff_ForwardFlag = TRUE,		
			.Error_Max = GYRO_ROLL_MAX,			
			.IntegrateSeparationError = 50,
			.Integrate_Max = 250,			
			.OutPut_Limit = 500,			
		},		
		.GyroPitch=
		{
			.ErrorLimitFlag = TRUE,
			.IntegrateLimitFlag = TRUE,
			.IntegrateSeparationFlag = TRUE,
			.Diff_ForwardFlag = TRUE,		
			.Error_Max = GYRO_PITCH_MAX,			
			.IntegrateSeparationError = 50,
			.Integrate_Max = 250,			
			.OutPut_Limit = 500,			
		},			
		.GyroYaw=
		{
			.ErrorLimitFlag = TRUE,
			.IntegrateLimitFlag = TRUE,
			.IntegrateSeparationFlag = TRUE,
			.Diff_ForwardFlag = TRUE,		
			.Error_Max = GYRO_YAW_MAX,			
			.IntegrateSeparationError = 50,
			.Integrate_Max = 250,			
			.OutPut_Limit = 500,			
		},	
		.GyroRoll_Stabiliz=
		{
			.ErrorLimitFlag = TRUE,
			.IntegrateLimitFlag = TRUE,
			.IntegrateSeparationFlag = TRUE,
			.Diff_ForwardFlag = TRUE,		
			.Error_Max = GYRO_ROLL_MAX,			
			.IntegrateSeparationError = 50,
			.Integrate_Max = 250,			
			.OutPut_Limit = 500,			
		},		
		.GyroPitch_Stabiliz=
		{
			.ErrorLimitFlag = TRUE,
			.IntegrateLimitFlag = TRUE,
			.IntegrateSeparationFlag = TRUE,
			.Diff_ForwardFlag = TRUE,		
			.Error_Max = GYRO_PITCH_MAX,			
			.IntegrateSeparationError = 50,
			.Integrate_Max = 250,			
			.OutPut_Limit = 500,			
		},
		.GyroYaw_Stabiliz=
		{
			.ErrorLimitFlag = TRUE,
			.IntegrateLimitFlag = TRUE,
			.IntegrateSeparationFlag = TRUE,
			.Diff_ForwardFlag = TRUE,		
			.Error_Max = GYRO_YAW_MAX,			
			.IntegrateSeparationError = 50,
			.Integrate_Max = 250,			
			.OutPut_Limit = 500,			
		}        
	}
};
static sSES_CA_FC SES_CA_FC[SES_CMD_NUM]={
	
    {SES_CMD_FC_DIRECTION,   NULL, &FC_Vars.FC_TXCMD.InstallDir,       sizeof(FC_Vars.FC_TXCMD.InstallDir)},
    {SES_CMD_FC_CALIBRATION, NULL, &FC_Vars.FC_TXCMD.CalibrateStatus,  sizeof(FC_Vars.FC_TXCMD.CalibrateStatus)}, 
    {SES_CMD_FC_MODE_CHN,    NULL, &FC_Vars.FC_TXCMD.ModeSwitchChn,    sizeof(FC_Vars.FC_TXCMD.ModeSwitchChn)},   
    {SES_CMD_FC_FLY_MODE,    NULL, &FC_Vars.FC_TXCMD.FlyMode,          sizeof(FC_Vars.FC_TXCMD.FlyMode)},    
    {SES_CMD_FC_SWITCH,      NULL, &FC_Vars.FC_TXCMD.Switch,           sizeof(FC_Vars.FC_TXCMD.Switch)},
    {SES_CMD_FC_GAIN,        NULL, &FC_Vars.FC_TXCMD.Gain,             sizeof(FC_Vars.FC_TXCMD.Gain)},
    {SES_CMD_FC_GAIN_REDUCE, NULL, &FC_Vars.FC_TXCMD.GainReduce,       sizeof(FC_Vars.FC_TXCMD.GainReduce)},   
    {SES_CMD_FC_CH_GAIN,     NULL, &FC_Vars.FC_TXCMD.CH_Gain,          sizeof(FC_Vars.FC_TXCMD.CH_Gain)},
    {SES_CMD_FC_CH_MIDDLE,   NULL, &FC_Vars.FC_TXCMD.CH_Middle,        sizeof(FC_Vars.FC_TXCMD.CH_Middle)},
    {SES_CMD_FC_CH_ENDPOINT, NULL, &FC_Vars.FC_TXCMD.CH_Endpoint,      sizeof(FC_Vars.FC_TXCMD.CH_Endpoint)},
    {SES_CMD_FC_REVERSE,     NULL, &FC_Vars.FC_TXCMD.CH_Reverse,       sizeof(FC_Vars.FC_TXCMD.CH_Reverse)},
    {SES_CMD_FC_ROLL_RECUP,  NULL, &FC_Vars.FC_TXCMD.Roll_Recup,       sizeof(FC_Vars.FC_TXCMD.Roll_Recup)},
    {SES_CMD_FC_PITCH_RECUP, NULL, &FC_Vars.FC_TXCMD.Pitch_Recup,      sizeof(FC_Vars.FC_TXCMD.Pitch_Recup)}
};


static void ContrDataUpdate(void)
{
	memcpy(&FC_Vars.Control.AngleRoll,&FC_Vars_Default.Control.AngleRoll,(sizeof(FC_Vars.Control.AngleRoll)));
	memcpy(&FC_Vars.Control.AnglePitch,&FC_Vars_Default.Control.AnglePitch,(sizeof(FC_Vars.Control.AnglePitch)));
    
	memcpy(&FC_Vars.Control.GyroRoll,&FC_Vars_Default.Control.GyroRoll,(sizeof(FC_Vars.Control.GyroRoll)));
	memcpy(&FC_Vars.Control.GyroPitch,&FC_Vars_Default.Control.GyroPitch,(sizeof(FC_Vars.Control.GyroPitch)));
	memcpy(&FC_Vars.Control.GyroYaw,&FC_Vars_Default.Control.GyroYaw,(sizeof(FC_Vars.Control.GyroYaw)));
    
	memcpy(&FC_Vars.Control.GyroRoll_Stabiliz,&FC_Vars_Default.Control.GyroRoll_Stabiliz,(sizeof(FC_Vars.Control.GyroRoll_Stabiliz)));
	memcpy(&FC_Vars.Control.GyroPitch_Stabiliz,&FC_Vars_Default.Control.GyroPitch_Stabiliz,(sizeof(FC_Vars.Control.GyroPitch_Stabiliz)));
	memcpy(&FC_Vars.Control.GyroYaw_Stabiliz,&FC_Vars_Default.Control.GyroYaw_Stabiliz,(sizeof(FC_Vars.Control.GyroYaw_Stabiliz)));
    
	FC_Vars.Control.AngleRoll.PID.Kp = (float)(10.0f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Attitude_Roll)*0.01f;
	FC_Vars.Control.AngleRoll.PID.Ki = 0;
	FC_Vars.Control.AngleRoll.PID.Kd = (float)(1.0f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Attitude_Roll)*0.01f;
	
	FC_Vars.Control.AnglePitch.PID.Kp = (float)(10.0f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Attitude_Pitch)*0.01f;
	FC_Vars.Control.AnglePitch.PID.Ki = 0;
	FC_Vars.Control.AnglePitch.PID.Kd = (float)(1.0f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Attitude_Pitch)*0.01f;

	FC_Vars.Control.GyroRoll.PID.Kp = (float)(2.50f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Roll)*0.01f;
	FC_Vars.Control.GyroRoll.PID.Ki = 0;
	FC_Vars.Control.GyroRoll.PID.Kd = (float)(0.80f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Roll)*0.01f;

	FC_Vars.Control.GyroPitch.PID.Kp = (float)(2.5f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Pitch)*0.01f;
	FC_Vars.Control.GyroPitch.PID.Ki = 0;
	FC_Vars.Control.GyroPitch.PID.Kd = (float)(0.80f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Pitch)*0.01f;

	FC_Vars.Control.GyroYaw.PID.Kp = (float)(7.50f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Yaw)*0.01f;
	FC_Vars.Control.GyroYaw.PID.Ki = 0;
	FC_Vars.Control.GyroYaw.PID.Kd = (float)(0.80f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Yaw)*0.01f;


	FC_Vars.Control.GyroRoll_Stabiliz.PID.Kp = (float)(2.50f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Roll)*0.01f;
	FC_Vars.Control.GyroRoll_Stabiliz.PID.Ki = 0;
	FC_Vars.Control.GyroRoll_Stabiliz.PID.Kd = (float)(0.80f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Roll)*0.01f;

	FC_Vars.Control.GyroPitch_Stabiliz.PID.Kp = (float)(2.5f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Pitch)*0.01f;
	FC_Vars.Control.GyroPitch_Stabiliz.PID.Ki = 0;
	FC_Vars.Control.GyroPitch_Stabiliz.PID.Kd = (float)(0.80f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Pitch)*0.01f;
    
	FC_Vars.Control.GyroYaw_Stabiliz.PID.Kp = (float)(7.50f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Yaw)*0.01f;
	FC_Vars.Control.GyroYaw_Stabiliz.PID.Ki = 0;
	FC_Vars.Control.GyroYaw_Stabiliz.PID.Kd = (float)(0.80f*FC_Vars.FC_Data_Save.FC_TxSet_Save.Gain.Basic_Yaw)*0.01f;

	FC_Vars.Control.RollChOutputMax_H  = (16384*FC_Vars.FC_Data_Save.FC_TxSet_Save.CH_Endpoint.High_Roll+50)/100;
	FC_Vars.Control.RollChOutputMax_L  = -(16384*FC_Vars.FC_Data_Save.FC_TxSet_Save.CH_Endpoint.Low_Roll+50)/100;
	FC_Vars.Control.PitchChOutputMax_H = (16384*FC_Vars.FC_Data_Save.FC_TxSet_Save.CH_Endpoint.High_Pitch+50)/100;
	FC_Vars.Control.PitchChOutputMax_L = -(16384*FC_Vars.FC_Data_Save.FC_TxSet_Save.CH_Endpoint.Low_Pitch+50)/100;
	FC_Vars.Control.YawChOutputMax_H   = (16384*FC_Vars.FC_Data_Save.FC_TxSet_Save.CH_Endpoint.High_Yaw+50)/100;
	FC_Vars.Control.YawChOutputMax_L   = -(16384*FC_Vars.FC_Data_Save.FC_TxSet_Save.CH_Endpoint.Low_Yaw+50)/100;

	FC_Vars.FC_TXCMD.CH_Middle.Roll  = FC_Vars.FC_Data_Save.FC_TxSet_Save.CH_Middle.Roll;
	FC_Vars.FC_TXCMD.CH_Middle.Pitch = FC_Vars.FC_Data_Save.FC_TxSet_Save.CH_Middle.Pitch;
	FC_Vars.FC_TXCMD.CH_Middle.Yaw   = FC_Vars.FC_Data_Save.FC_TxSet_Save.CH_Middle.Yaw;
    
    FC_Vars.Sensor.Installdir = FC_Vars.FC_TXCMD.InstallDir;
    
}


void FC_TxSetBackground(const void *pArgument,unsigned long ArgumentLength,unsigned long* Response,unsigned char *ResponseLenth)
{
	if((*(const unsigned char*)pArgument)>SES_CMD_NUM)
	{
		return;
	}
	for(unsigned char i=0;i<SES_CMD_NUM;i++)
	{
		if((*(const unsigned char*)pArgument)==SES_CA_FC[i].FCList && ArgumentLength == SES_CA_FC[i].FC_CMD_DataLenth+2)
		{
            SES_CA_FC[i].FCType= *((const unsigned char*)pArgument+1);

            if(SES_CA_FC[i].FCType==SES_CMD_FCTYPE_SET && i!=SES_CMD_FC_FLY_MODE)
            {
                memcpy(SES_CA_FC[i].FC_CMD_Data,(const unsigned char*)pArgument+2,SES_CA_FC[i].FC_CMD_DataLenth);
                if(SES_CA_FC[i].FCList==SES_CMD_FC_CALIBRATION)
                {	
                    if(FC_Vars.FC_TXCMD.CalibrateStatus==SES_FC_START_CALIBRATION)   
                    {
                        FC_Vars.FC_TXCMD.CalibrateStatus =SES_FC_IN_PROGRESS;
                        FC_Vars.Sensor.CalibrationFlg=TRUE;
                        LED_SetPattern(&LED_FC_Calibration);
                    } 
                    else if(FC_Vars.FC_TXCMD.CalibrateStatus==SES_FC_END_CALIBRATION) 
                    {
                        FC_Vars.FC_TXCMD.CalibrateStatus =SES_FC_FAILED;
                        FC_Vars.Sensor.CalibrationFlg=FALSE;
                        LED_SetPattern(&LED_Synchronized);                          
                    }    
                }
                else
                {
                    FC_Vars.DataSaveFlg = TRUE;
                }
            }			
            *Response= (unsigned long)&SES_CA_FC[i].FCList;				
            *ResponseLenth = SES_CA_FC[i].FC_CMD_DataLenth+2;	
			break;
		}	
	}	
}


static void FC_LoadFC_SaveData(void)
{
	if (!EEPROM_Read(sizeof(sAPP_Config)+16,&FC_Vars.FC_Data_Save,sizeof(FC_Vars.FC_Data_Save)) ||
	FC_Vars.FC_Data_Save.Length!=sizeof(FC_Vars.FC_Data_Save) ||
	FC_Vars.FC_Data_Save.CRC32!= SYS_CalcCRC32Table(&FC_Vars.FC_Data_Save,sizeof(FC_Vars.FC_Data_Save)-4))
	{
		memcpy(&FC_Vars.FC_Data_Save,&FC_Vars_Default.FC_Data_Save,sizeof(FC_Vars_Default.FC_Data_Save));
	}
	ContrDataUpdate();	
	return;
}


static void FC_SaveFC_SaveData(void)
{
	memcpy(&FC_Vars.FC_Data_Save.Sensor_Calibration_Save,&FC_Vars.Sensor.CalibratGyro,(sizeof(FC_Vars.Sensor.CalibratGyro)*6));
	memcpy(&FC_Vars.FC_Data_Save.FC_TxSet_Save,&FC_Vars.FC_TXCMD,(sizeof(FC_Vars.FC_TXCMD)));
	
	FC_Vars.FC_Data_Save.Length = sizeof(FC_Vars.FC_Data_Save);
	FC_Vars.FC_Data_Save.CRC32 = SYS_CalcCRC32Table(&FC_Vars.FC_Data_Save,sizeof(FC_Vars.FC_Data_Save)-4);
	EEPROM_Write(sizeof(sAPP_Config)+16,&FC_Vars.FC_Data_Save,sizeof(FC_Vars.FC_Data_Save));	
	LED_SetPattern(&LED_Synchronized);
	ContrDataUpdate();	
	return;
}


void FC_Init(void)
{
	GyroFC_Init();
	FC_LoadFC_SaveData();
	memcpy(&FC_Vars.Sensor.CalibratGyro,&FC_Vars.FC_Data_Save.Sensor_Calibration_Save,(sizeof(FC_Vars.Sensor.CalibratGyro)*6));
	memcpy(&FC_Vars.FC_TXCMD,&FC_Vars.FC_Data_Save.FC_TxSet_Save,(sizeof(FC_Vars.FC_TXCMD)));	
    FC_Vars.Sensor.Installdir = FC_Vars.FC_TXCMD.InstallDir;
    FC_Vars.Sensor.CalibrationFlg=0;
    FC_Vars.Sensor.CalibratDataSaveFlg=0;
}

static long Limtlong(long DataIn,long DataMax,long DataMin)
{	
	if(DataIn>DataMax) 
	{
		return DataMax;
	}
	else if(DataIn<DataMin)
	{
		return DataMin;
	}
	else
	{
		return DataIn;
	}
}

static short LimtShort(short DataIn,short DataMax,short DataMin)
{	
	if(DataIn>DataMax) 
	{
		return DataMax;
	}
	else if(DataIn<DataMin)
	{
		return DataMin;
	}
	else
	{
		return DataIn;
	}
}

static float LimtFloat(float DataIn,float DataMax,float DataMin)
{	
	if(DataIn>DataMax) 
	{
		return DataMax;
	}
	else if(DataIn<DataMin)
	{
		return DataMin;
	}
	else
	{
		return DataIn;
	}
}


static char GainReduceDeal(short ChInput,short ChOutputMax_H,short ChOutputMax_L,short ChMiddle,eSES_GainReduceType ReduceType)
{
    signed char ChScale=0,GainScal=0; 

    if(ChInput>=ChMiddle)
    {
        ChScale = (signed char)(100*(ChInput - ChMiddle)/(ChOutputMax_H-ChMiddle));
    }
    else
    {
        ChScale =(signed char)(100*(ChInput - ChMiddle)/(ChOutputMax_L-ChMiddle));
    }
    switch((int)ReduceType)
    {
        case None:
            GainScal=100;
            break;
        case Stable:
            if(ChScale>-13 && ChScale<13)
            {
                GainScal=100;
            }
            else if(ChScale<=-13)
            {
                GainScal=(signed char)((float)(ChScale*0.5747+107.47));
            }
            else if(ChScale>=13)
            {
                GainScal=(signed char)((float)(-ChScale*0.5747+107.47));
            }
            break;
        case Odrinary:
            if(ChScale>-13 && ChScale<13)
            {
                GainScal=100;
            }
            else if(ChScale<=-13)
            {
                GainScal=(signed char)((float)(ChScale*1.149+114.9));
            }
            else if(ChScale>=13)
            {
                GainScal=(signed char)((float)(-ChScale*1.149+114.9));
            }
            break;
        case Sport:
            if(ChScale>-13 && ChScale<13)
            {
                GainScal=100;
            }
            else if(ChScale<=-13 && ChScale>=-60)
            {
                GainScal=(signed char)((float)(ChScale*2.127+127.65));
            }
            else if(ChScale>=13 && ChScale<=60)
            {
                GainScal=(signed char)((float)(-ChScale*2.127+127.65));
            }
            else
            {
                GainScal=0;
            }          
            break;
        case Stunts_3D:
             if(ChScale<0)
             {
                 GainScal=(signed char)((float)(ChScale*2+100));
             }
             else
             {
                 GainScal=(signed char)((float)(-ChScale*2+100));
             }
           
            break;                
        default:

            break;
    }  
    if(GainScal<0)
    {
        GainScal=0;
    }
    return (char)GainScal;
}


static void FC_Task(void)
{
	signed short CHDataTem[FC_CH_ITEM]={0};
	//
	if(FC_Vars.DataSaveFlg==TRUE || FC_Vars.Sensor.CalibratDataSaveFlg==TRUE)
	{
        if(FC_Vars.Sensor.CalibratDataSaveFlg==TRUE)
        {
            FC_Vars.Sensor.CalibratDataSaveFlg = FALSE;
            FC_Vars.FC_TXCMD.CalibrateStatus = SES_FC_SUCCESSFUL;	
        }
		FC_SaveFC_SaveData();
		FC_Vars.DataSaveFlg=FALSE;        
	}	

	//
    if(FC_Vars.FC_TXCMD.ModeSwitchChn>4)
    {
        if(FC_Vars.Control.ModeChInput<-1000)
        {
            FC_Vars.Control.FlyMode = Manual;		
        }
        else if(FC_Vars.Control.ModeChInput>-1000 && FC_Vars.Control.ModeChInput<1000)
        {
            FC_Vars.Control.FlyMode = Stabilizing;
        }
        else
        {
            FC_Vars.Control.FlyMode = Attitude;
        }       
    }
    else
    {
        FC_Vars.Control.FlyMode = Manual;
    }
    FC_Vars.FC_TXCMD.FlyMode = FC_Vars.Control.FlyMode;
    
	// Manual modle
	if(FC_Vars.Control.FlyMode == Manual)
	{

        memcpy(&FC_Vars.Control.RollChOutput,&FC_Vars.Control.RollChInput,4*sizeof(FC_Vars.Control.RollChInput));
		return;
	}
    
	//ROLL	
	//Gyro_Roll  Close
	if(FC_Vars.FC_TXCMD.Switch.Roll==0)
	{
		memcpy(&CHDataTem[ROLL],&FC_Vars.Control.RollChInput,sizeof(FC_Vars.Control.RollChInput));
	}
	else
	{
        signed long RollChInputTemplong;
        signed short RollChInputTemp;
        RollChInputTemplong =(FC_Vars.Control.RollChInput-FC_Vars.FC_TXCMD.CH_Middle.Roll)*FC_Vars.FC_TXCMD.CH_Gain.Roll/100;
        RollChInputTemp=(short)Limtlong(RollChInputTemplong,(long)FC_Vars.Control.RollChOutputMax_H,(long)FC_Vars.Control.RollChOutputMax_L);
		if(FC_Vars.Control.FlyMode == Attitude)
		{
            //Roll Reverse
            if(FC_Vars.FC_TXCMD.CH_Reverse.Roll)
            {
                FC_Vars.Control.GyroRoll.FeedBack = -FC_Vars.Imu.Gyro.x;
                FC_Vars.Control.AngleRoll.FeedBack = -(FC_Vars.Imu.Roll-FC_Vars.FC_TXCMD.Roll_Recup);
            }
            else
            {
                FC_Vars.Control.GyroRoll.FeedBack = FC_Vars.Imu.Gyro.x;
                FC_Vars.Control.AngleRoll.FeedBack = FC_Vars.Imu.Roll-FC_Vars.FC_TXCMD.Roll_Recup;
            }
            //Gyro_Roll FeedBack Limit
            FC_Vars.Control.GyroRoll.FeedBack = LimtFloat(FC_Vars.Control.GyroRoll.FeedBack,GYRO_ROLL_MAX,-GYRO_ROLL_MAX);
           
			//Angle_Roll Expect
            if(RollChInputTemp>FC_Vars.FC_TXCMD.CH_Middle.Roll)
            {
                FC_Vars.Control.AngleRoll.Expect = RollChInputTemp*AGLE_ROLL_MAX/(FC_Vars.Control.RollChOutputMax_H-FC_Vars.FC_TXCMD.CH_Middle.Roll);
            }
            else
            {
                FC_Vars.Control.AngleRoll.Expect = RollChInputTemp*AGLE_ROLL_MAX/(FC_Vars.FC_TXCMD.CH_Middle.Roll-FC_Vars.Control.RollChOutputMax_L);
            }
			// PID Control
			PID_Control(&FC_Vars.Control.AngleRoll);
			
            //Gyro_Roll Expect 
			FC_Vars.Control.GyroRoll.Expect = FC_Vars.Control.AngleRoll.OutPut;	  
            // PID Control 
            PID_Control(&FC_Vars.Control.GyroRoll);	
            //
            CHDataTem[ROLL]=(signed short)(((long)FC_Vars.Control.GyroRoll.OutPut <<15)/1000); 
            
		}
		else if(FC_Vars.Control.FlyMode == Stabilizing)
		{
            //Gyro_Roll Expect
            FC_Vars.Control.GyroRoll_Stabiliz.Expect = 0;
             //Roll Reverse
            if(FC_Vars.FC_TXCMD.CH_Reverse.Roll)
            {
                FC_Vars.Control.GyroRoll_Stabiliz.FeedBack = -FC_Vars.Imu.Gyro.x;
            }
            else
            {
                FC_Vars.Control.GyroRoll_Stabiliz.FeedBack = FC_Vars.Imu.Gyro.x;
            }
            //Gyro_Roll FeedBack Limit
            FC_Vars.Control.GyroRoll_Stabiliz.FeedBack = LimtFloat(FC_Vars.Control.GyroRoll_Stabiliz.FeedBack,GYRO_ROLL_MAX,-GYRO_ROLL_MAX);  
            
            FC_Vars.Control.GyroRoll_Stabiliz.PID.Kp  = FC_Vars.Control.GyroRoll.PID.Kp * GainReduceDeal(FC_Vars.Control.RollChInput,
                                                                                                         FC_Vars.Control.RollChOutputMax_H,
                                                                                                         FC_Vars.Control.RollChOutputMax_L,
                                                                                                         FC_Vars.FC_TXCMD.CH_Middle.Roll,
                                                                                                         FC_Vars.FC_TXCMD.GainReduce.eRoll)/100;
            FC_Vars.Control.GyroRoll_Stabiliz.PID.Kd=FC_Vars.Control.GyroRoll_Stabiliz.PID.Kp/3.0f;
            // PID Control	            
            PID_Control(&FC_Vars.Control.GyroRoll_Stabiliz);	
            //
            CHDataTem[ROLL]=(signed short)(((long)FC_Vars.Control.GyroRoll_Stabiliz.OutPut <<15)/1000); 
            CHDataTem[ROLL]+=RollChInputTemp;
		}				
		//Limit output
		CHDataTem[ROLL]=LimtShort(CHDataTem[ROLL],FC_Vars.Control.RollChOutputMax_H,FC_Vars.Control.RollChOutputMax_L);
	}
	
	//PITCH	
	//Gyro_Pitch  Close
	if(FC_Vars.FC_TXCMD.Switch.Pitch==0)
	{
		memcpy(&CHDataTem[PITCH],&FC_Vars.Control.PitchChInput,sizeof(FC_Vars.Control.PitchChInput));
	}
	else
	{
        //
        signed long PitchChInputTemplong;
        signed short PitchChInputTemp;
        PitchChInputTemplong =(signed long )(FC_Vars.Control.PitchChInput-FC_Vars.FC_TXCMD.CH_Middle.Pitch)*FC_Vars.FC_TXCMD.CH_Gain.Pitch/100;        
        PitchChInputTemp = (short)Limtlong(PitchChInputTemplong,(long)FC_Vars.Control.PitchChOutputMax_H,(long)FC_Vars.Control.PitchChOutputMax_L);
             
		if(FC_Vars.Control.FlyMode == Attitude)
		{
			//Angle_Pitch
				
			//Angle_Pitch Reverse
			if(FC_Vars.FC_TXCMD.CH_Reverse.Pitch)
			{
                if(PitchChInputTemp>FC_Vars.FC_TXCMD.CH_Middle.Pitch)
                {
                    FC_Vars.Control.AnglePitch.Expect = PitchChInputTemp*AGLE_PITCH_MAX_UP/(FC_Vars.Control.PitchChOutputMax_H-FC_Vars.FC_TXCMD.CH_Middle.Pitch);
                }
                else
                {
                    FC_Vars.Control.AnglePitch.Expect = PitchChInputTemp*AGLE_PITCH_MAX_DOWN/(FC_Vars.FC_TXCMD.CH_Middle.Pitch-FC_Vars.Control.PitchChOutputMax_L);
                } 
                
				FC_Vars.Control.AnglePitch.FeedBack = FC_Vars.Imu.Pitch-FC_Vars.FC_TXCMD.Pitch_Recup;               
                FC_Vars.Control.GyroPitch.FeedBack = FC_Vars.Imu.Gyro.y;
                if(FC_Vars.Control.GyroPitch.FeedBack<0)
                {
                    FC_Vars.Control.GyroPitch.FeedBack *=1.6f;
                }
			}
			else
			{
                if(PitchChInputTemp>FC_Vars.FC_TXCMD.CH_Middle.Pitch)
                {
                    FC_Vars.Control.AnglePitch.Expect = PitchChInputTemp*AGLE_PITCH_MAX_DOWN/(FC_Vars.Control.PitchChOutputMax_H-FC_Vars.FC_TXCMD.CH_Middle.Pitch);
                }
                else
                {
                    FC_Vars.Control.AnglePitch.Expect = PitchChInputTemp*AGLE_PITCH_MAX_UP/(FC_Vars.FC_TXCMD.CH_Middle.Pitch-FC_Vars.Control.PitchChOutputMax_L);
                } 
				FC_Vars.Control.AnglePitch.FeedBack = -FC_Vars.Imu.Pitch+FC_Vars.FC_TXCMD.Pitch_Recup; 
                FC_Vars.Control.GyroPitch.FeedBack = -FC_Vars.Imu.Gyro.y;
                if(FC_Vars.Control.GyroPitch.FeedBack>0)
                {
                    FC_Vars.Control.GyroPitch.FeedBack *=1.6f;
                }
			}
           
			FC_Vars.Control.GyroPitch.FeedBack = LimtFloat(FC_Vars.Control.GyroPitch.FeedBack,GYRO_PITCH_MAX,-GYRO_PITCH_MAX);
			// PID Control
			PID_Control(&FC_Vars.Control.AnglePitch);
            if(FC_Vars.FC_TXCMD.CH_Reverse.Pitch)
            {
               FC_Vars.Control.AnglePitch.OutPut = LimtFloat(FC_Vars.Control.AnglePitch.OutPut,ANGLE_PITCHOUTPUT_MAX,-ANGLE_PITCHOUTPUT_MAX_DOWN);
            }
            else
            {    
                FC_Vars.Control.AnglePitch.OutPut = LimtFloat(FC_Vars.Control.AnglePitch.OutPut,ANGLE_PITCHOUTPUT_MAX_DOWN,-ANGLE_PITCHOUTPUT_MAX);
            }
			FC_Vars.Control.GyroPitch.Expect = FC_Vars.Control.AnglePitch.OutPut;	
            // PID Control
            PID_Control(&FC_Vars.Control.GyroPitch);
            if(FC_Vars.FC_TXCMD.CH_Reverse.Pitch)
            {
               FC_Vars.Control.GyroPitch.OutPut = LimtFloat(FC_Vars.Control.GyroPitch.OutPut,ANGLE_PITCHOUTPUT_MAX,-ANGLE_PITCHOUTPUT_MAX_DOWN);
            }
            else
            {   
                FC_Vars.Control.GyroPitch.OutPut = LimtFloat(FC_Vars.Control.GyroPitch.OutPut,ANGLE_PITCHOUTPUT_MAX_DOWN,-ANGLE_PITCHOUTPUT_MAX);
                
            }                        
            CHDataTem[PITCH]=(signed short)(((long)FC_Vars.Control.GyroPitch.OutPut <<15)/1000);              
		}
		else if(FC_Vars.Control.FlyMode == Stabilizing)
		{
            //
            FC_Vars.Control.GyroPitch_Stabiliz.Expect =0;      
            //Gyro_Pitch Reverse
            if(FC_Vars.FC_TXCMD.CH_Reverse.Pitch)
            {
                FC_Vars.Control.GyroPitch_Stabiliz.FeedBack = FC_Vars.Imu.Gyro.y;
            }
            else
            {
                FC_Vars.Control.GyroPitch_Stabiliz.FeedBack = -FC_Vars.Imu.Gyro.y;
            } 
            FC_Vars.Control.GyroPitch_Stabiliz.FeedBack = LimtFloat(FC_Vars.Control.GyroPitch_Stabiliz.FeedBack,GYRO_PITCH_MAX,-GYRO_PITCH_MAX);   
            FC_Vars.Control.GyroPitch_Stabiliz.PID.Kp = FC_Vars.Control.GyroPitch.PID.Kp * GainReduceDeal(FC_Vars.Control.PitchChInput,
                                                                                                          FC_Vars.Control.PitchChOutputMax_H,
                                                                                                          FC_Vars.Control.PitchChOutputMax_L,
                                                                                                          FC_Vars.FC_TXCMD.CH_Middle.Pitch,
                                                                                                          FC_Vars.FC_TXCMD.GainReduce.ePitch)/100;
            FC_Vars.Control.GyroPitch_Stabiliz.PID.Kd=FC_Vars.Control.GyroPitch_Stabiliz.PID.Kp/3.0f;
            // PID Control
            PID_Control(&FC_Vars.Control.GyroPitch_Stabiliz);	
            CHDataTem[PITCH]=(signed short)(((long)FC_Vars.Control.GyroPitch_Stabiliz.OutPut <<15)/1000);
            CHDataTem[PITCH]+=PitchChInputTemp;
		}
		//Limit output
		CHDataTem[PITCH]=LimtShort(CHDataTem[PITCH],FC_Vars.Control.PitchChOutputMax_H,FC_Vars.Control.PitchChOutputMax_L);		
	}	
	
	//YAW
	//Gyro Yaw Close
	if(FC_Vars.FC_TXCMD.Switch.Yaw==0)
	{
		memcpy(&CHDataTem[YAW],&FC_Vars.Control.YawChInput,sizeof(FC_Vars.Control.YawChOutput));
	}
	else
	{
        //
        signed long YawChInputTemplong;
        signed short YawChInputTemp;
        YawChInputTemplong =(signed long )(FC_Vars.Control.YawChInput-FC_Vars.FC_TXCMD.CH_Middle.Yaw)*FC_Vars.FC_TXCMD.CH_Gain.Yaw/100;
        YawChInputTemp = (short)Limtlong(YawChInputTemplong,(long)FC_Vars.Control.YawChOutputMax_H,(long)FC_Vars.Control.YawChOutputMax_L);    
        //
        FC_Vars.Control.GyroYaw_Stabiliz.Expect=0;       
		//Gyro_Yaw Reverse
		if(FC_Vars.FC_TXCMD.CH_Reverse.Yaw==0)
		{
			FC_Vars.Control.GyroYaw_Stabiliz.FeedBack = FC_Vars.Imu.Gyro.z;
		}
		else
		{
			FC_Vars.Control.GyroYaw_Stabiliz.FeedBack = -FC_Vars.Imu.Gyro.z;
		}
		FC_Vars.Control.GyroYaw_Stabiliz.FeedBack = LimtFloat(FC_Vars.Control.GyroYaw_Stabiliz.FeedBack,GYRO_YAW_MAX,-GYRO_YAW_MAX);
        //
        FC_Vars.Control.GyroYaw_Stabiliz.PID.Kp = FC_Vars.Control.GyroYaw.PID.Kp * GainReduceDeal(FC_Vars.Control.YawChInput,
                                                                                                  FC_Vars.Control.YawChOutputMax_H,
                                                                                                  FC_Vars.Control.YawChOutputMax_L,
                                                                                                  FC_Vars.FC_TXCMD.CH_Middle.Yaw,
                                                                                                  FC_Vars.FC_TXCMD.GainReduce.eYaw)/100;
        FC_Vars.Control.GyroYaw_Stabiliz.PID.Kd=FC_Vars.Control.GyroYaw_Stabiliz.PID.Kp/3.0f;
		// PID Control
		PID_Control(&FC_Vars.Control.GyroYaw_Stabiliz);	
		CHDataTem[YAW] = (signed short)(((long)FC_Vars.Control.GyroYaw_Stabiliz.OutPut <<15)/1000);
        CHDataTem[YAW]+= YawChInputTemp;
		//limit output
		CHDataTem[YAW]=LimtShort(CHDataTem[YAW],FC_Vars.Control.YawChOutputMax_H,FC_Vars.Control.YawChOutputMax_L);		
	}
    FC_Vars.Control.RollChOutput  = CHDataTem[ROLL]+FC_Vars.FC_TXCMD.CH_Middle.Roll;
    FC_Vars.Control.PitchChOutput = CHDataTem[PITCH]+FC_Vars.FC_TXCMD.CH_Middle.Pitch;	
    FC_Vars.Control.YawChOutput   = CHDataTem[YAW]+FC_Vars.FC_TXCMD.CH_Middle.Yaw;	

	//Throttle Output
	memcpy(&FC_Vars.Control.ThrottleCHOutput,&FC_Vars.Control.ThrottleChInput,sizeof(FC_Vars.Control.ThrottleChInput));	
}	



void FC_Background(void)
{
	if(SYS_SysTickMs-FC_Vars.Control.FC_SysTickMs>=2)
	{
		FC_Vars.Control.FC_SysTickMs=SYS_SysTickMs;
		GyroFC_DataDeal(&FC_Vars.Sensor);
		memcpy(&FC_Vars.Imu.Gyro,&FC_Vars.Sensor.OutGyro,sizeof(FC_Vars.Sensor.OutGyro));
		memcpy(&FC_Vars.Imu.Acc,&FC_Vars.Sensor.OutAcc,sizeof(FC_Vars.Sensor.OutAcc));		
		IMUupdate(0.001f,&FC_Vars.Imu);
		FC_Task();	 
#ifdef DEBUG
        static unsigned char i=0;
        i++;
        if(i>4) i=0;
        if(i==0)
        {
            ANO_DT_Send_Senser((short)FC_Vars.Imu.Acc.x,(short)FC_Vars.Imu.Acc.y,(short)FC_Vars.Imu.Acc.z,
                            (short)FC_Vars.Imu.Gyro.x,(short)FC_Vars.Imu.Gyro.y,(short)FC_Vars.Imu.Gyro.z);
        }
        else if(i==2)
        {   
            ANO_DT_Send_Status(FC_Vars.Imu.Roll, FC_Vars.Imu.Pitch , FC_Vars.Imu.Yaw);
        }
#endif 
	}	
}




#endif

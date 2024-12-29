#if defined(PN_INR6_FC)
#include <string.h>

#include "I2C.h"
#include "EEPROM.h"
#include "Main.h"
#include "AFHDS3 CRC library.h"

#include "GyroFC.h"

#include <stdio.h>
#include <math.h>



static BOOL GyroFC_Read(unsigned long Address,void *pData,unsigned int DataLength) 
{
	if (!I2C_Write(GYROFC_ADDRESS,&Address,1,FALSE) ||
		!I2C_Read(GYROFC_ADDRESS,pData,DataLength,TRUE))
		return FALSE;
	else
		return TRUE;
}

static BOOL GyroFC_Write(unsigned long Address,const void *pData,unsigned int DataLength)
{
	unsigned char Buffer[10];
	
	Buffer[0]=(unsigned char)Address;
	memcpy(Buffer+1,pData,DataLength);
	if (I2C_Write(GYROFC_ADDRESS,Buffer,DataLength+1,TRUE))
			return TRUE;
	return FALSE;
}

void GyroFC_Init(void)
{
	unsigned char temp;
	temp=0x00;
	GyroFC_Write(0x01,&temp,1);

	GyroFC_Read(0x0f,&temp,1);
	SYS_DelayUs(1000);
	while(temp!=0x6A)
		
	temp=0x00;
	GyroFC_Write(0x01,&temp,1);

	temp=0x7F;
	GyroFC_Write(0x10,&temp,1);
	
	temp=0x7C;
	GyroFC_Write(0x11,&temp,1);
	
	temp=0xC1;
	GyroFC_Write(0x17,&temp,1);
}

void GyroFC_DataDeal(sFC_SensorData *FC_GyroData)
{
	unsigned char GyroFC_Data[ITEM*2]; 
	static signed short SunNum,FILT_BUF[ITEM][FILTER_NUM],filter_cnt=0;
	static signed long SunTemp[ITEM],FILT_Temp[ITEM];
	//
	GyroFC_Read(0x22,&GyroFC_Data[0],12);
	//
	FC_GyroData->OriginalGyro.x = (short)(((short)GyroFC_Data[1]<<8)|GyroFC_Data[0]);
	FC_GyroData->OriginalGyro.y = (short)(((short)GyroFC_Data[3]<<8)|GyroFC_Data[2]);	
	FC_GyroData->OriginalGyro.z = (short)(((short)GyroFC_Data[5]<<8)|GyroFC_Data[4]); 	
	//
	FC_GyroData->OriginalAcc.x = (short)(((short)GyroFC_Data[7]<<8)|GyroFC_Data[6]);
	FC_GyroData->OriginalAcc.y = (short)(((short)GyroFC_Data[9]<<8)|GyroFC_Data[8]);
	FC_GyroData->OriginalAcc.z = (short)(((short)GyroFC_Data[11]<<8)|GyroFC_Data[10]);
	//
	if(FC_GyroData->CalibrationFlg==1
		&&(FC_GyroData->OriginalGyro.x - FC_GyroData->OriginalGyroPre.x<50 || FC_GyroData->OriginalGyro.x - FC_GyroData->OriginalGyroPre.x>-50)
		&&(FC_GyroData->OriginalGyro.y - FC_GyroData->OriginalGyroPre.y<50 || FC_GyroData->OriginalGyro.y - FC_GyroData->OriginalGyroPre.y>-50)
		&&(FC_GyroData->OriginalGyro.z - FC_GyroData->OriginalGyroPre.z<50 || FC_GyroData->OriginalGyro.z - FC_GyroData->OriginalGyroPre.z>-50))
	{
		SunNum++;
		SunTemp[Gx]+= FC_GyroData->OriginalGyro.x;
		SunTemp[Gy]+= FC_GyroData->OriginalGyro.y;
		SunTemp[Gz]+= FC_GyroData->OriginalGyro.z;
		SunTemp[Ax]+= FC_GyroData->OriginalAcc.x;
		SunTemp[Ay]+= FC_GyroData->OriginalAcc.y;
		SunTemp[Az]+= FC_GyroData->OriginalAcc.z;
		
		if(SunNum>=1000)
		{		
			FC_GyroData->CalibratGyro.x = (short)(SunTemp[Gx]/1000);
			FC_GyroData->CalibratGyro.y = (short)(SunTemp[Gy]/1000);
			FC_GyroData->CalibratGyro.z = (short)(SunTemp[Gz]/1000);

			FC_GyroData->CalibratAcc.x = (short)(SunTemp[Ax]/1000);
			FC_GyroData->CalibratAcc.y = (short)(SunTemp[Ay]/1000);
			FC_GyroData->CalibratAcc.z = (short)(SunTemp[Az]/1000);
           if(FC_GyroData->Installdir == LevelForward)
           {
                FC_GyroData->CalibratAcc.z -= 4096;
           }
           else if(FC_GyroData->Installdir == LeftForward)
           {
                FC_GyroData->CalibratAcc.y += 4096;
           }
           else if(FC_GyroData->Installdir == RightForward)
           {
                FC_GyroData->CalibratAcc.y -= 4096;
           }
			FC_GyroData->CalibrationFlg=0;
			FC_GyroData->CalibratDataSaveFlg=1;
		}		
	}
	else
	{
		memset(SunTemp,0x00,sizeof(SunTemp));
		SunNum=0;		
	}
	memcpy(&FC_GyroData->OriginalGyroPre,&FC_GyroData->OriginalGyro,sizeof(FC_GyroData->OriginalGyro));
	memcpy(&FC_GyroData->OriginalAccPre,&FC_GyroData->OriginalAcc,sizeof(FC_GyroData->OriginalAcc));
	
	if( ++filter_cnt > FILTER_NUM )
	{
		filter_cnt = 0;
	}	
	FILT_BUF[Gx][filter_cnt] = FC_GyroData->OriginalGyro.x - FC_GyroData->CalibratGyro.x;
	FILT_BUF[Gy][filter_cnt] = FC_GyroData->OriginalGyro.y - FC_GyroData->CalibratGyro.y;
	FILT_BUF[Gz][filter_cnt] = FC_GyroData->OriginalGyro.z - FC_GyroData->CalibratGyro.z;
	
	FILT_BUF[Ax][filter_cnt] = FC_GyroData->OriginalAcc.x - FC_GyroData->CalibratAcc.x;
	FILT_BUF[Ay][filter_cnt] = FC_GyroData->OriginalAcc.y - FC_GyroData->CalibratAcc.y;
	FILT_BUF[Az][filter_cnt] = FC_GyroData->OriginalAcc.z - FC_GyroData->CalibratAcc.z;	
	
	memset(FILT_Temp,0,sizeof(FILT_Temp));
	
	for(unsigned char i=0; i<FILTER_NUM; i++)
	{
		FILT_Temp[Gx] += FILT_BUF[Gx][i];
		FILT_Temp[Gy] += FILT_BUF[Gy][i];
		FILT_Temp[Gz] += FILT_BUF[Gz][i];
		FILT_Temp[Ax] += FILT_BUF[Ax][i];
		FILT_Temp[Ay] += FILT_BUF[Ay][i];
		FILT_Temp[Az] += FILT_BUF[Az][i];
	}
   
    FC_GyroData->OutGyro.x = -(float)(FILT_Temp[Gx]/FILTER_NUM);
    if(FC_GyroData->Installdir == LevelForward)
    {
        FC_GyroData->OutGyro.y = (float)(FILT_Temp[Gy]/FILTER_NUM);
        FC_GyroData->OutGyro.z = -(float)(FILT_Temp[Gz]/FILTER_NUM);	
    }
    else if(FC_GyroData->Installdir == LeftForward)
    {
        FC_GyroData->OutGyro.z = (float)(FILT_Temp[Gy]/FILTER_NUM);
        FC_GyroData->OutGyro.y = (float)(FILT_Temp[Gz]/FILTER_NUM);	
    }
    else if(FC_GyroData->Installdir == RightForward)
    {
        FC_GyroData->OutGyro.z = -(float)(FILT_Temp[Gy]/FILTER_NUM);
        FC_GyroData->OutGyro.y = -(float)(FILT_Temp[Gz]/FILTER_NUM);	
    }
	FC_GyroData->OutGyro.x *= 0.07f;
	FC_GyroData->OutGyro.y *= 0.07f;
	FC_GyroData->OutGyro.z *= 0.07f;
	
	FC_GyroData->OutAcc.x = (float)(FILT_Temp[Ax]/FILTER_NUM);
    if(FC_GyroData->Installdir == LevelForward)
    {
        FC_GyroData->OutAcc.y = -(float)(FILT_Temp[Ay]/FILTER_NUM);
        FC_GyroData->OutAcc.z = (float)(FILT_Temp[Az]/FILTER_NUM);
    }
    else if(FC_GyroData->Installdir == LeftForward)
    {
        FC_GyroData->OutAcc.z = -(float)(FILT_Temp[Ay]/FILTER_NUM);
        FC_GyroData->OutAcc.y = -(float)(FILT_Temp[Az]/FILTER_NUM);
    }  
    else if(FC_GyroData->Installdir == RightForward)
    {
        FC_GyroData->OutAcc.z = (float)(FILT_Temp[Ay]/FILTER_NUM);
        FC_GyroData->OutAcc.y = (float)(FILT_Temp[Az]/FILTER_NUM);
    }  
}
#endif


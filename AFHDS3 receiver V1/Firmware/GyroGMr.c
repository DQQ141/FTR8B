#if defined(PN_GMR)
#include <string.h>

#include "I2C.h"
#include "EEPROM.h"
#include "Main.h"
#include "AFHDS3 CRC library.h"

#include "GyroGMr.h"

#include <stdio.h>
#include <math.h>

sGyro_SVC_Vars  Gyro_SVC_Vars;

static void Gyro_LoadSVCCalibration(void)
{
	if (!EEPROM_Read(sizeof(sAPP_Config)+16,&Gyro_SVC_Vars.SVC_Calibration,sizeof(sGyro_SVC_Calibration)) ||
		Gyro_SVC_Vars.SVC_Calibration.Length!=sizeof(sGyro_SVC_Calibration) ||
		Gyro_SVC_Vars.SVC_Calibration.CRC32!=SYS_CalcCRC32Table(&Gyro_SVC_Vars.SVC_Calibration,sizeof(sGyro_SVC_Calibration)-4))
	{
		Gyro_SVC_Vars.SVC_Calibration.SteeringChMiddle=0;
		Gyro_SVC_Vars.SVC_Calibration.ThrottleChMiddle=0;
		Gyro_SVC_Vars.SVC_Calibration.GyroZCalibateData=0;
	}
	return;
}

static void Gyro_SaveSVCCalibration(void)
{
	Gyro_SVC_Vars.SVC_Calibration.Length=sizeof(sGyro_SVC_Calibration);
	Gyro_SVC_Vars.SVC_Calibration.CRC32=SYS_CalcCRC32Table(&Gyro_SVC_Vars.SVC_Calibration,sizeof(sGyro_SVC_Calibration)-4);
	EEPROM_Write(sizeof(sAPP_Config)+16,&Gyro_SVC_Vars.SVC_Calibration,sizeof(sGyro_SVC_Calibration));
	return;
}

static BOOL Gyro_Read(unsigned long Address,void *pData,unsigned int DataLength) 
{
	if (!I2C_Write(GYRO_ADDRESS,&Address,1,FALSE) ||
		!I2C_Read(GYRO_ADDRESS,pData,DataLength,TRUE))
		return FALSE;
	else
		return TRUE;
}

static BOOL Gyro_Write(unsigned long Address,const void *pData,unsigned int DataLength)
{
	unsigned char Buffer[10];
	
	Buffer[0]=(unsigned char)Address;
	memcpy(Buffer+1,pData,DataLength);
	if (I2C_Write(GYRO_ADDRESS,Buffer,DataLength+1,TRUE))
			return TRUE;
	return FALSE;
}



static void Gyro_SVC_Task(sGyro_SVC_Vars *pVars)
{    
//    static signed short GyroZdataUsePre=0;
    signed char  	TargetSteeringGain;
    signed short   	GyroZdataUse;	
    signed long 	OutPutHigh_MAX;
    signed long 	OutPutLow_MAX;
    signed long  SteeringSVCOut=0;
    static signed long 	SteeringSVCOutGyro=0;
    signed long		ThrottleSVCOut,ThrottleChOutputTemp,SteeringChOutputTemp;
    signed short    GyroZdataUseTmp;
    
    if(Gyro_SVC_Vars.GyroConnecedFlg==FALSE)
    {
        pVars->ThrottleChOutput=pVars->ThrottleChInput;
        pVars->SteeringChOutput=pVars->SteeringChInput;
        
        if(pVars->Gyro_CalibateFlag==TRUE)
        {
           pVars->CalibrateGYROStatus=SES_CSVCS_FAILED;
           if(APP_SignalStatus==SES_SS_SYNCHRONIZED)
				LED_SetPattern(&LED_Synchronized);       
			else
				LED_SetPattern(&LED_Synchronizing); 
             pVars->GyroCalibateCnt=0;
        }
        return;
    }
    
    if(pVars->SVC_CalibateFlag==TRUE)
    {
        pVars->SVC_Calibration.ThrottleChMiddle = pVars->ThrottleChInput; 
        pVars->SVC_Calibration.SteeringChMiddle = pVars->SteeringChInput; 
        pVars->CalibrateSVCStatus = SES_CSVCS_SUCCESSFUL;
        // Save config
		Gyro_SaveSVCCalibration();
        pVars->SVC_CalibateFlag = FALSE;
    }
    else if(pVars->Gyro_CalibateFlag==TRUE)
    {
        //
        if(pVars->GyroZdata - pVars->SVC_Calibration.GyroZCalibateData>20 ||
            pVars->SVC_Calibration.GyroZCalibateData - pVars->GyroZdata>20 )
        {
            pVars->GyroCalibateCnt=0;
            pVars->SVC_Calibration.GyroZCalibateData = pVars->GyroZdata;
        }
        pVars->CalibrateGYROStatus=SES_CSVCS_IN_PROGRESS;
        
        pVars->GyroCalibateCnt++;
        if(pVars->GyroCalibateCnt>1000)
        {
            pVars->GyroCalibateCnt=0;
            pVars->Gyro_CalibateFlag = FALSE;
			// Save config
            Gyro_SaveSVCCalibration();
            pVars->CalibrateGYROStatus = SES_CSVCS_SUCCESSFUL;
			if(APP_SignalStatus==SES_SS_SYNCHRONIZED)
				LED_SetPattern(&LED_Synchronized);       
			else
				LED_SetPattern(&LED_Synchronizing); 
        }        
    }
    else
    {            
        //MoveMode Judgement
        //Forward
        if((pVars->pSVC->ThrottleReverse==0 && pVars->ThrottleChInput > 
			(pVars->SVC_Calibration.ThrottleChMiddle+THR_BREAK_MIDDLE_BUF)) ||
           (pVars->pSVC->ThrottleReverse==1 && pVars->ThrottleChInput < 
			(pVars->SVC_Calibration.ThrottleChMiddle-THR_BREAK_MIDDLE_BUF)))
        {
            pVars->AheadEndNum=0;
            pVars->MoveMode=MODE_FORWARD;
        }
        else if(pVars->ThrottleChInput<pVars->SVC_Calibration.ThrottleChMiddle+THR_BREAK_MIDDLE_BUF && 
			pVars->ThrottleChInput>pVars->SVC_Calibration.ThrottleChMiddle-THR_BREAK_MIDDLE_BUF)
        {
            pVars->AheadEndNum++;
            if(pVars->AheadEndNum>300)
            {
                pVars->AheadEndNum=300;
            }
        }
        //Break  
        if(((pVars->pSVC->ThrottleReverse==0 && pVars->ThrottleChInput < 
			 (pVars->SVC_Calibration.ThrottleChMiddle+THR_BREAK_MIDDLE_BUF))||
            (pVars->pSVC->ThrottleReverse==1 && pVars->ThrottleChInput > 
			 (pVars->SVC_Calibration.ThrottleChMiddle-THR_BREAK_MIDDLE_BUF))) && pVars->AheadEndNum<125)//160ms
        {
            pVars->MoveMode=MODE_BRAKE;
        }
        else if(((pVars->pSVC->ThrottleReverse==0 && pVars->ThrottleChInput < 
				  (pVars->SVC_Calibration.ThrottleChMiddle+THR_BREAK_MIDDLE_BUF))||
				 (pVars->pSVC->ThrottleReverse==1 && pVars->ThrottleChInput > 
				  (pVars->SVC_Calibration.ThrottleChMiddle-THR_BREAK_MIDDLE_BUF))) && pVars->AheadEndNum>=125)//160ms
        {
           pVars->MoveMode=MODE_BACK; 
        }
  
        if(pVars->ThrottleChInput<pVars->SVC_Calibration.ThrottleChMiddle+THR_BREAK_MIDDLE_BUF && 
			pVars->ThrottleChInput>pVars->SVC_Calibration.ThrottleChMiddle-THR_BREAK_MIDDLE_BUF)
        {
            pVars->ThrottleChMiddleNUN++;
            if(pVars->ThrottleChMiddleNUN>THROTTLE_MIDDLE_MAX)
            {
                pVars->ThrottleChMiddleNUN=THROTTLE_MIDDLE_MAX;
            }        
            pVars->MoveMode=MODE_STOP;
        }
        else
        {
            pVars->ThrottleChMiddleNUN=0;
        }
        
        // turn off SVC  or Back off SVC
        if(pVars->pSVC->SVCOn==0 || pVars->MoveMode==MODE_BACK)
        {
            pVars->ThrottleChOutput=pVars->ThrottleChInput;
            pVars->SteeringChOutput=pVars->SteeringChInput;
            pVars->GyroZdataUseIntegration=0;
            return;
        }
        if(pVars->MoveMode==MODE_BRAKE)
        {
            pVars->ThrottleChOutput=pVars->ThrottleChInput;
            pVars->Break_Num++;
            if(pVars->Break_Num>BREAK_STOP_SVC_MSNUM)
            {
                pVars->Break_Num=BREAK_STOP_SVC_MSNUM;
                pVars->SteeringChOutput=pVars->SteeringChInput;
                pVars->GyroZdataUseIntegration=0;
                return;
            }
        }
        else
        {
            pVars->Break_Num=0;
        }

        //Gyro data read
        GyroZdataUse = pVars->GyroZdata - pVars->SVC_Calibration.GyroZCalibateData;         
        
        //OutPutMAX     pority 
        if(pVars->pSVC->Reserved)
        {
            OutPutHigh_MAX = 0-(unsigned short)(((unsigned long)pVars->pSVC->EndpointHigh<<14)/100);  //16384*EndpointHigh
            OutPutLow_MAX  = (unsigned short)(((unsigned long)pVars->pSVC->EndpointLow <<14)/100);  //16384*EndpointHigh 
            
            //OutPutHigh_MAX----SteeringChMiddle----SteeringChInput----OutPutLow_MAX
            if(pVars->SteeringChInput >= pVars->SVC_Calibration.SteeringChMiddle)
            {
                TargetSteeringGain = (signed char)((pVars->pSVC->SteeringGain *
                     (100-(pVars->pSVC->Priority*(pVars->SteeringChInput-pVars->SVC_Calibration.SteeringChMiddle)/(OutPutLow_MAX-pVars->SVC_Calibration.SteeringChMiddle))))/100);   
            }           
            //OutPutHigh_MAX----SteeringChInput----SteeringChMiddle----OutPutLow_MAX
            else
            {
                TargetSteeringGain = (signed char)((pVars->pSVC->SteeringGain *
                     (100-(pVars->pSVC->Priority*(pVars->SVC_Calibration.SteeringChMiddle-pVars->SteeringChInput)/(pVars->SVC_Calibration.SteeringChMiddle-OutPutHigh_MAX))))/100);                  
            }
        }            
        else
        {
            OutPutHigh_MAX = (unsigned short)(((unsigned long)pVars->pSVC->EndpointHigh<<14)/100);  //16384*EndpointHigh
            OutPutLow_MAX  = 0-(unsigned short)(((unsigned long)pVars->pSVC->EndpointLow <<14)/100);  //16384*EndpointHigh 
            //OutPutLow_MAX----SteeringChMiddle----SteeringChInput----OutPutHigh_MAX
            if(pVars->SteeringChInput >= pVars->SVC_Calibration.SteeringChMiddle)
            {
                TargetSteeringGain = (signed char)((pVars->pSVC->SteeringGain *
                     (100-(pVars->pSVC->Priority*(pVars->SteeringChInput-pVars->SVC_Calibration.SteeringChMiddle)/(OutPutHigh_MAX-pVars->SVC_Calibration.SteeringChMiddle))))/100);   
            }           
            //OutPutLow_MAX----SteeringChInput----SteeringChMiddle----OutPutHigh_MAX
            else
            {
                TargetSteeringGain = (signed char)((pVars->pSVC->SteeringGain *
                     (100-(pVars->pSVC->Priority*(pVars->SVC_Calibration.SteeringChMiddle-pVars->SteeringChInput)/(pVars->SVC_Calibration.SteeringChMiddle-OutPutLow_MAX))))/100);                  
            }
        }
        if((pVars->SteeringChInput>(pVars->SVC_Calibration.SteeringChMiddle - STEER_LOCKMODE_KEPP_BUF))  
             && (pVars->SteeringChInput< (pVars->SVC_Calibration.SteeringChMiddle + STEER_LOCKMODE_KEPP_BUF)))
        {
            pVars->SteeringChMiddleNUN++;
            if(pVars->SteeringChMiddleNUN>1000)  
            pVars->SteeringChMiddleNUN=1000;    
        }
        else
        {
            pVars->SteeringChMiddleNUN=0;
        }
        
        //LockMode
        if(pVars->pSVC->ESPMode  && pVars->SteeringChMiddleNUN>100
            && (pVars->SteeringChInput>(pVars->SVC_Calibration.SteeringChMiddle - STEER_LOCKMODE_KEPP_BUF))  
             && (pVars->SteeringChInput< (pVars->SVC_Calibration.SteeringChMiddle + STEER_LOCKMODE_KEPP_BUF)))
        {
            if(GyroZdataUse>=16 || GyroZdataUse<=-16) 
            {
                 GyroZdataUseTmp= GyroZdataUse;
            }                
            else
            {
                GyroZdataUse=0;
                GyroZdataUseTmp=0;
            }          
            if(TargetSteeringGain>0)
            {           
                if(SYS_SysTickMs-Gyro_SVC_Vars.Gyro_TaskMsPID>=4)
                {
                    Gyro_SVC_Vars.Gyro_TaskMsPID = SYS_SysTickMs;
                    
                     signed long GyroErr=0; 
                     GyroErr = GyroZdataUseTmp;
                     if(pVars->pSVC->Reserved) 
                     {
                         if((SteeringSVCOut<OutPutHigh_MAX && GyroZdataUse>0)  || 
                            (SteeringSVCOut>OutPutLow_MAX  && GyroZdataUse<0)  ||
                            (SteeringSVCOut>=OutPutHigh_MAX && SteeringSVCOut<=OutPutLow_MAX))
                            {
                                 GyroZdataUseTmp>>=4;
                                 pVars->GyroZdataUseIntegration += GyroZdataUseTmp;
                                if(pVars->GyroZdataUseIntegration<OutPutHigh_MAX)
                                {
                                    pVars->GyroZdataUseIntegration = OutPutHigh_MAX;
                                }
                                else if(pVars->GyroZdataUseIntegration>OutPutLow_MAX)
                                {
                                    pVars->GyroZdataUseIntegration = OutPutLow_MAX;
                                }
                            }                         
                     } 
                     else
                     {
                         if((SteeringSVCOut<OutPutLow_MAX && GyroZdataUse>0)  || 
                            (SteeringSVCOut>OutPutHigh_MAX  && GyroZdataUse<0)  ||
                            (SteeringSVCOut>=OutPutLow_MAX && SteeringSVCOut<=OutPutHigh_MAX))
                            {
                                 GyroZdataUseTmp>>=4;
                                 pVars->GyroZdataUseIntegration += GyroZdataUseTmp;
                                if(pVars->GyroZdataUseIntegration<OutPutLow_MAX)
                                {
                                    pVars->GyroZdataUseIntegration = OutPutLow_MAX;
                                }
                                else if(pVars->GyroZdataUseIntegration>OutPutHigh_MAX)
                                {
                                    pVars->GyroZdataUseIntegration = OutPutHigh_MAX;
                                }
                            }    
                     }                                  									
                    GyroErr += pVars->GyroZdataUseIntegration;                                          
                    static signed long GyroErrPre =0;
                    SteeringSVCOutGyro = (signed long)(16*TargetSteeringGain*GyroErr)/1000;   
                    SteeringSVCOutGyro += TargetSteeringGain*((GyroErr - GyroErrPre)<<3)/100;                  
                    GyroErrPre = GyroErr;	                
                }
                 SteeringSVCOut = SteeringSVCOutGyro;           
            }
            else
            {
                SteeringSVCOut=0;  
            }            
            //
            if(pVars->ThrottleChMiddleNUN>250)//500ms 
            {    
                if(pVars->GyroZdataUseIntegration>32)
                {
                    pVars->GyroZdataUseIntegration-=32;
                }
                else if(pVars->GyroZdataUseIntegration<-32)
                {
                    pVars->GyroZdataUseIntegration+=32;
                }
                else
                {
                    pVars->GyroZdataUseIntegration=0;
                }                   
            }                               
        }
        else//StabilityMode
        {     
            if(TargetSteeringGain>0)
            {
                if(SYS_SysTickMs-Gyro_SVC_Vars.Gyro_TaskMsPID>=4)
                {
                    Gyro_SVC_Vars.Gyro_TaskMsPID = SYS_SysTickMs;
									
                    signed long GyroErr = GyroZdataUse;                      
                    static signed long GyroErrPre =0;
                    SteeringSVCOutGyro = (signed long)(24*TargetSteeringGain*GyroErr)/1000;   
                    SteeringSVCOutGyro += TargetSteeringGain*((GyroErr - GyroErrPre)<<3)/100;                    
                    GyroErrPre = GyroErr;	                
                }
                 SteeringSVCOut = SteeringSVCOutGyro;
            }
            else
            {
                SteeringSVCOut=0; 
            }                
            pVars->GyroZdataUseIntegration=0; 
        }       

        //GyroReverse
        SteeringChOutputTemp = pVars->SteeringChInput;
        if(pVars->pSVC->GyroReverse==1)
        {   
          SteeringChOutputTemp += SteeringSVCOut;           
        }
        else
        {
           SteeringChOutputTemp -= SteeringSVCOut;
        }      
 
        //ThrottleSVCOut
        ThrottleSVCOut = ((24*pVars->pSVC->ThrottleGain)*GyroZdataUse)/1000;
        if(ThrottleSVCOut<0)
        {
            ThrottleSVCOut=0-ThrottleSVCOut;
        }
        
		//ThrottleReverse
		if(pVars->MoveMode!=MODE_BRAKE)
		{
			ThrottleChOutputTemp = pVars->ThrottleChInput ;
			if(pVars->MoveMode==MODE_FORWARD)
			{
				if(pVars->pSVC->ThrottleReverse==1)
				{
					ThrottleChOutputTemp += ThrottleSVCOut;
					if(ThrottleChOutputTemp > pVars->SVC_Calibration.ThrottleChMiddle)
					{
						ThrottleChOutputTemp = pVars->SVC_Calibration.ThrottleChMiddle;
					}
				}
				else
				{
					ThrottleChOutputTemp -= ThrottleSVCOut;
					if(ThrottleChOutputTemp < pVars->SVC_Calibration.ThrottleChMiddle)
					{
						ThrottleChOutputTemp = pVars->SVC_Calibration.ThrottleChMiddle;
					}
				}
					
			}			            
			pVars->ThrottleChOutput = (signed short)ThrottleChOutputTemp;             
		}


         // Endpoint Steering
        if(pVars->pSVC->Reserved)
        {            
            if(SteeringChOutputTemp<OutPutHigh_MAX)
            {
                SteeringChOutputTemp = OutPutHigh_MAX;  
            }        
            else if(SteeringChOutputTemp>OutPutLow_MAX)
            {
                SteeringChOutputTemp = OutPutLow_MAX;
            }                        
        }
        else
        {
            if(SteeringChOutputTemp<OutPutLow_MAX)
            {
                SteeringChOutputTemp = OutPutLow_MAX;  
            }        
            else if(SteeringChOutputTemp>OutPutHigh_MAX)
            {
                SteeringChOutputTemp = OutPutHigh_MAX;
            }                                       
        }  
        pVars->SteeringChOutput = (signed short)SteeringChOutputTemp;               
    }
}

static signed short FilterBuf[10]={0,0,0,0,0,0,0,0,0,0};
static void Gyro_DataFilter(signed short In_GyroData,signed short *pFiltedData)
{
    static unsigned char filter_cnt=0;
    signed short DataMax,DataMin;
    signed long BufSum;
    
    FilterBuf[filter_cnt]=In_GyroData;
    filter_cnt++;
    if(filter_cnt>=10)
        filter_cnt=0;
    DataMax = FilterBuf[0];
    DataMin = FilterBuf[0];
    BufSum = FilterBuf[0];
    for(unsigned char i=1;i<10;i++)
    {
        if(DataMax<FilterBuf[i])
        {
            DataMax = FilterBuf[i];
        }
        
        if(DataMin>FilterBuf[i])
        {
            DataMin = FilterBuf[i];
        }
        BufSum+=FilterBuf[i];
    }
    
    BufSum -= DataMax;
    BufSum -= DataMin;

    *pFiltedData = (signed short)(BufSum>>3);
}


static void Gyro_ReadData(signed short *pGyroZ_Data)
{
	unsigned char GyroData[3]; 
    short  GyroZDataTemp;  
    memset(GyroData,0x00,3);
    Gyro_Read(0x75,GyroData,1);
    GyroData[0] &= 0x7E;	
	if(GyroData[0] == 0x68 || GyroData[0] == 0x2E || GyroData[0] == 0x0E)//ÍÓÂÝÒÇ 6887 ºÍ 6887P
    {
        if(Gyro_SVC_Vars.GyroInitFlg == FALSE)
        {
            Gyro_Init();
        }
        Gyro_Read(0x47,&GyroData[1],2);
        GyroZDataTemp = (short)(((short)GyroData[1])<<8)|GyroData[2];
        Gyro_DataFilter(GyroZDataTemp,pGyroZ_Data);
        Gyro_SVC_Vars.GyroConnecedFlg  = TRUE;        
    }
    else
    {
        Gyro_SVC_Vars.GyroInitFlg = FALSE;
        Gyro_SVC_Vars.GyroConnecedFlg  = FALSE;
        *pGyroZ_Data = 0x7fff;
    }
}


void Gyro_Init(void)
{
	unsigned char GyroData[1];
	
    Gyro_Read(0x75,GyroData,1);
    GyroData[0] &= 0x7E;	
	if(GyroData[0] == 0x68 || GyroData[0] == 0x2E || GyroData[0] == 0x0E)//ÍÓÂÝÒÇ 6887 ºÍ 6887P
    {
        Gyro_SVC_Vars.GyroConnecedFlg  = TRUE; 
         // Device Reset
        GyroData[0] = 0x80;
        Gyro_Write(0x6B,GyroData,1);	     
        do{
            SYS_DelayUs(1000);
            Gyro_Read(0x6b,GyroData,1);
            GyroData[0] = GyroData[0] & 0x80;
        } while(GyroData[0]);    
        // temp disable
        GyroData[0] = 0x09;
        Gyro_Write(0x6b,GyroData,1);
        // sample rate 1000HZ
        GyroData[0] = 0x00; 
        Gyro_Write(0x19,GyroData,1);
        //
        GyroData[0] = 0x01;
        Gyro_Write(0x1a,GyroData,1);
        //
        GyroData[0] = 0x18;
        Gyro_Write(0x1c,GyroData,1);
        //¡À1000 dps
        GyroData[0] = 0x10;
        Gyro_Write(0x1b,GyroData,1);
        //
        GyroData[0] = 0x30;
        Gyro_Write(0x37,GyroData,1);
        //
        GyroData[0] = 0x01;
        Gyro_Write(0x38,GyroData,1);
        
        // Init paremeter
        Gyro_SVC_Vars.pSVC=&APP_Config.C.RF.SVC;
        Gyro_SVC_Vars.GyroInitFlg = TRUE;
        Gyro_SVC_Vars.CalibrationMidleRestFlg = FALSE;
		Gyro_LoadSVCCalibration();
    }
    else
    {
        Gyro_SVC_Vars.GyroConnecedFlg  = FALSE;
        Gyro_SVC_Vars.GyroInitFlg = FALSE;
    }
	return;
}


void Gyro_SVC_CalibrationBackground(sGyro_SVC_Vars *pVars, sSES_CA_CalibrateSVC *pCalibrateSVC,
            sSES_CA_CalibrateSVCResponse *pCalibrateSVCResponse)
{
    if(pCalibrateSVC->CalibrationType==SVC && pCalibrateSVC->StartCalibration==START_CATIBRETIN)
    {
        pVars->SVC_CalibateFlag=TRUE;
        pCalibrateSVCResponse->Status = SES_CSVCS_IN_PROGRESS;
    }
    else if(pCalibrateSVC->CalibrationType==GYROSCOPE && pCalibrateSVC->StartCalibration==START_CATIBRETIN)
    { 
        if(pVars->GyroConnecedFlg)  
        {
            pVars->Gyro_CalibateFlag=TRUE;
            pCalibrateSVCResponse->Status = SES_CSVCS_IN_PROGRESS;
            LED_SetPattern(&LED_SVC_Calibration);       
        } 
        else
        {
            pCalibrateSVCResponse->Status = SES_CSVCS_FAILED;
        }    
    }
    else if(pCalibrateSVC->CalibrationType==SVC && pCalibrateSVC->StartCalibration==CHECK_STATUS)
    {
       pCalibrateSVCResponse->Status = pVars->CalibrateSVCStatus;
    }
    else if(pCalibrateSVC->CalibrationType==GYROSCOPE && pCalibrateSVC->StartCalibration==CHECK_STATUS)
    {
       pCalibrateSVCResponse->Status = pVars->CalibrateGYROStatus;
    }
}


void Gyro_SVC_Background(void)
{
	if(SYS_SysTickMs-Gyro_SVC_Vars.Gyro_TaskMs>=2)
	{
		Gyro_SVC_Vars.Gyro_TaskMs=SYS_SysTickMs;		
		Gyro_ReadData(&Gyro_SVC_Vars.GyroZdata);		
 		Gyro_SVC_Task(&Gyro_SVC_Vars);
        
        if(Gyro_SVC_Vars.CalibrationMidleRestFlg==TRUE)
        {
            Gyro_SVC_Vars.CalibrationMidleRestFlg = FALSE;
            
            Gyro_SVC_Vars.SVC_Calibration.SteeringChMiddle=0;
            Gyro_SVC_Vars.SVC_Calibration.ThrottleChMiddle=0;
            Gyro_SaveSVCCalibration();
        }
	}
	return;
}





#endif

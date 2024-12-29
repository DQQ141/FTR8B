#if defined(PN_INR4_GYB) || defined(PN_GMR)
#include <string.h>

#include "I2C.h"
#include "EEPROM.h"
#include "Main.h"
#include "AFHDS3 CRC library.h"

#include "Gyro.h"
#include "PID FC.h"

#include <stdio.h>
#include <math.h>

sGyro_SVC_Vars  Gyro_SVC_Vars;
static unsigned char GYRO_ADDRESS;

static sPID GyroYawPIDBasic={
	.Kp= 2.50f,	
	.Ki= 0.00f,     
	.Kd= 0.00f,	
};

static sPID_Controller   GyroYaw=
{
	.ErrorLimitFlag = TRUE,
	.IntegrateLimitFlag = TRUE,
	.IntegrateSeparationFlag = TRUE,
	.Diff_ForwardFlag = TRUE, 
//	.Error_Max = 50000,			
    .Error_Max = 5000,	
	.IntegrateSeparationError = 300,
	.Integrate_Max = 800,			
	.OutPut_Limit = 32670,
    .Diff_IncompleteDelay=0,
};

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

static signed long EndputLimit(BOOL ReservedFlg,signed long DataIn, signed long OutPutHigh_MAX,signed long OutPutLow_MAX)
{
    if(ReservedFlg)
    {            
        if(DataIn<OutPutHigh_MAX)
        { 
            return OutPutHigh_MAX;  
        }        
        else if(DataIn>OutPutLow_MAX)
        {
            return OutPutLow_MAX;
        } 
        else  
        {
           return DataIn;
        }                
    }
    else
    {
        if(DataIn<OutPutLow_MAX)
        {
            return OutPutLow_MAX;  
        }        
        else if(DataIn>OutPutHigh_MAX)
        {
            return OutPutHigh_MAX;
        } 
        else  
        {
           return DataIn;
        }              
    } 
}


static void Gyro_SVC_Task(sGyro_SVC_Vars *pVars)
{    
    static signed short   	GyroZdataUse=0;	
    static signed long		SteeringChOutputTemp=0;
    static signed short   SteeringChoutputOld=0,SlowDownPoint=0;    
    signed char  	TargetSteeringGain;
	signed char     TargetPriority;
    signed long     SteeringChInputTemp;
    signed long 	OutPutHigh_MAX;
    signed long 	OutPutLow_MAX;

    static signed long     SteeringSVCOut=0,SteeringSVCOutTemp=0,SteeringSVCOutTemp1=0;
    signed long		ThrottleSVCOut,ThrottleChOutputTemp,DeltSteering,SlowDownProportion;
    signed short    GyroZdataUseTmp;
    static unsigned char TurnNum=0,ReturnNum=0;
    static unsigned short   ThrottleFlg=0;
    static  signed long GyroCalibateSum=0; 
    static signed short GyroZdataTemp=0;
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
        if(pVars->GyroZdata - GyroZdataTemp>30 ||GyroZdataTemp - pVars->GyroZdata>30 )
        {
            pVars->GyroCalibateCnt=0;
            GyroCalibateSum=0;
            GyroZdataTemp = pVars->GyroZdata;
        }
        pVars->CalibrateGYROStatus=SES_CSVCS_IN_PROGRESS;
        GyroCalibateSum+=pVars->GyroZdata;
        pVars->GyroCalibateCnt++;
        if(pVars->GyroCalibateCnt>=2000)
        {
            pVars->GyroCalibateCnt=0;
            pVars->Gyro_CalibateFlag = FALSE;
            GyroZdataTemp=0;
            pVars->SVC_Calibration.GyroZCalibateData=(signed short)(GyroCalibateSum/2000);
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
            if(pVars->AheadEndNum>1000)
            {
                pVars->AheadEndNum=1000;
            }
        }
        //Break  
        if(((pVars->pSVC->ThrottleReverse==0 && pVars->ThrottleChInput < 
			 (pVars->SVC_Calibration.ThrottleChMiddle+THR_BREAK_MIDDLE_BUF))||
            (pVars->pSVC->ThrottleReverse==1 && pVars->ThrottleChInput > 
			 (pVars->SVC_Calibration.ThrottleChMiddle-THR_BREAK_MIDDLE_BUF))) && pVars->AheadEndNum<800)//125  160ms
        {
            pVars->MoveMode=MODE_BRAKE;
        }
        else if(((pVars->pSVC->ThrottleReverse==0 && pVars->ThrottleChInput < 
				  (pVars->SVC_Calibration.ThrottleChMiddle+THR_BREAK_MIDDLE_BUF))||
				 (pVars->pSVC->ThrottleReverse==1 && pVars->ThrottleChInput > 
				  (pVars->SVC_Calibration.ThrottleChMiddle-THR_BREAK_MIDDLE_BUF))) && pVars->AheadEndNum>=800)//125 160ms
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
        if(pVars->pSVC->SVCOn==0)
        {
            pVars->ThrottleChOutput=pVars->ThrottleChInput;
            pVars->SteeringChOutput=pVars->SteeringChInput;
            pVars->GyroZdataUseIntegration=0;
            return;
        }

        //Gyro data read               
        GyroZdataUse = pVars->GyroZdata - pVars->SVC_Calibration.GyroZCalibateData; 
              
        //OutPutMAX     pority 
        if(pVars->pSVC->Reserved)
        {
            OutPutHigh_MAX = 0-(unsigned short)(((unsigned long)pVars->pSVC->EndpointHigh<<14)/100);  //16384*EndpointHigh
            OutPutLow_MAX  = (unsigned short)(((unsigned long)pVars->pSVC->EndpointLow <<14)/100);  //16384*EndpointHigh 
            TargetSteeringGain = (signed char)pVars->pSVC->SteeringGain; 
            //OutPutHigh_MAX----SteeringChMiddle----SteeringChInput----OutPutLow_MAX
            if(pVars->SteeringChInput >= pVars->SVC_Calibration.SteeringChMiddle)
            {
                TargetPriority = (signed char)((pVars->pSVC->SteeringGain *
                     (100-(pVars->pSVC->Priority*(pVars->SteeringChInput-pVars->SVC_Calibration.SteeringChMiddle)/(OutPutLow_MAX-pVars->SVC_Calibration.SteeringChMiddle))))/100);   
            }           
            //OutPutHigh_MAX----SteeringChInput----SteeringChMiddle----OutPutLow_MAX
            else
            {
                TargetPriority = (signed char)((pVars->pSVC->SteeringGain *
                     (100-(pVars->pSVC->Priority*(pVars->SVC_Calibration.SteeringChMiddle-pVars->SteeringChInput)/(pVars->SVC_Calibration.SteeringChMiddle-OutPutHigh_MAX))))/100);                  
            }
        }            
        else
        {
            OutPutHigh_MAX = (unsigned short)(((unsigned long)pVars->pSVC->EndpointHigh<<14)/100);  //16384*EndpointHigh
            OutPutLow_MAX  = 0-(unsigned short)(((unsigned long)pVars->pSVC->EndpointLow <<14)/100);  //16384*EndpointHigh 
			TargetSteeringGain = (signed char)pVars->pSVC->SteeringGain; 
            //OutPutLow_MAX----SteeringChMiddle----SteeringChInput----OutPutHigh_MAX
            if(pVars->SteeringChInput >= pVars->SVC_Calibration.SteeringChMiddle)
            {
                TargetPriority = (signed char)((pVars->pSVC->SteeringGain *
                     (100-(pVars->pSVC->Priority*(pVars->SteeringChInput-pVars->SVC_Calibration.SteeringChMiddle)/(OutPutHigh_MAX-pVars->SVC_Calibration.SteeringChMiddle))))/100);   
            }           
            //OutPutLow_MAX----SteeringChInput----SteeringChMiddle----OutPutHigh_MAX
            else
            {
                TargetPriority = (signed char)((pVars->pSVC->SteeringGain *
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
        
        GyroYaw.PID.Kp = (GyroYawPIDBasic.Kp*TargetPriority)/100.0f;
        GyroYaw.PID.Kd = (GyroYawPIDBasic.Kd*TargetPriority)/100.0f;
        
        //
        if(pVars->MoveMode==MODE_BACK || pVars->MoveMode== MODE_BRAKE)
        {
           ThrottleFlg=500; 
        }
        else  if(pVars->MoveMode==MODE_STOP && ThrottleFlg>=1)
        {
            ThrottleFlg--;           
        }
        else if(pVars->MoveMode==MODE_FORWARD)
        {
            ThrottleFlg=0;
        }
                   
        //LockMode
        if(pVars->pSVC->ESPMode  && pVars->SteeringChMiddleNUN>200
            && (pVars->SteeringChInput>(pVars->SVC_Calibration.SteeringChMiddle - STEER_LOCKMODE_KEPP_BUF))  
             && (pVars->SteeringChInput< (pVars->SVC_Calibration.SteeringChMiddle + STEER_LOCKMODE_KEPP_BUF))
                &&ThrottleFlg==0)
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
            //            
            if(TargetSteeringGain>0)
            {  
                if(SYS_SysTickMs-Gyro_SVC_Vars.Gyro_TaskMs1>=2)
                {
                    Gyro_SVC_Vars.Gyro_TaskMs1=SYS_SysTickMs;
                    GyroZdataUseTmp>>=5;                
                    pVars->GyroZdataUseIntegration += GyroZdataUseTmp;                                  
                    pVars->GyroZdataUseIntegration = EndputLimit(0,pVars->GyroZdataUseIntegration,(long)(16384/GyroYaw.PID.Kp),(long)(-16384/GyroYaw.PID.Kp));
                    // 
                    if(pVars->ThrottleChMiddleNUN>500)//500ms 
                    {    
                        if(pVars->GyroZdataUseIntegration>16)
                        {
                            pVars->GyroZdataUseIntegration-=16*100/((pVars->pSVC->SteeringGain>0)? pVars->pSVC->SteeringGain:1);
                        }
                        else if(pVars->GyroZdataUseIntegration<-16)
                        {
                            pVars->GyroZdataUseIntegration+=16*100/((pVars->pSVC->SteeringGain>0)? pVars->pSVC->SteeringGain:1);
                        }
                        else
                        {
                            pVars->GyroZdataUseIntegration=0;
                        }                   
                    } 	
                    if(pVars->pSVC->GyroReverse==1)
                    {
                        GyroYaw.Expect = -(float)(pVars->GyroZdataUseIntegration);
                        GyroYaw.FeedBack=(float)(GyroZdataUse);
                    }
                    else
                    {
                        GyroYaw.Expect = (float)(pVars->GyroZdataUseIntegration);
                        GyroYaw.FeedBack=-(float)(GyroZdataUse);
                    } 
                    PID_Control(&GyroYaw);										
                    SteeringSVCOut = (signed long)(((long)GyroYaw.OutPut <<15)/10000); 
                    SteeringChOutputTemp = SteeringSVCOut + pVars->SVC_Calibration.SteeringChMiddle;                    
                }                
            }
            else
            {
                SteeringChOutputTemp = pVars->SteeringChInput;
            }            
        }
        else//StabilityMode
        {     
			if(TargetSteeringGain>0)
            {									               
                GyroYaw.Expect=0;
				if(GyroZdataUse<=16 && GyroZdataUse>=-16) 
				{
					 GyroZdataUse=0;
				} 				
				//GyroReverse
				if(pVars->pSVC->GyroReverse==1)
				{
					GyroYaw.FeedBack = GyroZdataUse;
				}
				else
				{
					GyroYaw.FeedBack = -GyroZdataUse;
				}					
				PID_Control(&GyroYaw);										
				SteeringSVCOut = (signed long)(((long)GyroYaw.OutPut <<15)/10000);                            	
            }
            else
            {
                SteeringSVCOut = pVars->SteeringChInput;
            }                
            pVars->GyroZdataUseIntegration=0; 
            
            //
            if(pVars->SupportAdvancedSVC&& Gyro_SVC_Vars.RXType!=0)
            {
                SteeringChInputTemp = (long)(pVars->SteeringChInput - pVars->SVC_Calibration.SteeringChMiddle)*(100+(pVars->pAdvancedSVC->TurningGain>>1))/100;
            }
            else
            {
                SteeringChInputTemp = (long)(pVars->SteeringChInput - pVars->SVC_Calibration.SteeringChMiddle);
 
            }
            SteeringChInputTemp+=pVars->SVC_Calibration.SteeringChMiddle;
            SteeringChInputTemp=EndputLimit(pVars->pSVC->Reserved,SteeringChInputTemp,OutPutHigh_MAX,OutPutLow_MAX);  
            
            if(pVars->SupportAdvancedSVC&& Gyro_SVC_Vars.RXType!=0 &&pVars->pAdvancedSVC->ForwardSpeed !=0 && pVars->pAdvancedSVC->ReturnSpeed!=0)
            {     
                 //
                DeltSteering= SteeringChInputTemp-SteeringSVCOutTemp;
                if(DeltSteering<0)   
                {
                    DeltSteering=0-DeltSteering;
                }  
                if(DeltSteering<1)
                {
                    DeltSteering=1;
                }    
                             
                //
                if(pVars->pSVC->Reserved)
                {
                    if(SteeringSVCOutTemp>pVars->SVC_Calibration.SteeringChMiddle)
                    {
                       if(DeltSteering>OutPutLow_MAX*pVars->pAdvancedSVC->ForwardSpeed)
                       {
                          SteeringSVCOutTemp+=((SteeringSVCOut-SteeringSVCOutTemp+SteeringChInputTemp)>>4); 
                       }   
                       else
                       {
                           SlowDownProportion= DeltSteering*100/OutPutLow_MAX;
                           if(SlowDownProportion<(100-pVars->pAdvancedSVC->ReturnSpeed))
                           {
                               SlowDownProportion = 100-pVars->pAdvancedSVC->ReturnSpeed;                               
                           }
                           SlowDownProportion=SlowDownProportion>>1;
                           if(SlowDownProportion<2)
                           {
                               SlowDownProportion=2;
                           }
                           SteeringSVCOutTemp+=(((SteeringSVCOut-SteeringSVCOutTemp+SteeringChInputTemp)>>4)*SlowDownProportion)/100;
                       }
                    }
                    else
                    {
                       if(DeltSteering>(0-OutPutHigh_MAX*pVars->pAdvancedSVC->ForwardSpeed))
                       {
                          SteeringSVCOutTemp+=((SteeringSVCOut-SteeringSVCOutTemp+SteeringChInputTemp)>>4); 
                       }   
                       else
                       {
                           SlowDownProportion= DeltSteering*100/(0-OutPutHigh_MAX);
                           if(SlowDownProportion<(100-pVars->pAdvancedSVC->ReturnSpeed))
                           {
                               SlowDownProportion = 100-pVars->pAdvancedSVC->ReturnSpeed;
                           }
                           SlowDownProportion=SlowDownProportion>>1;
                           if(SlowDownProportion<5)
                           {
                               SlowDownProportion=5;
                           }
                           SteeringSVCOutTemp+=(((SteeringSVCOut-SteeringSVCOutTemp+SteeringChInputTemp)>>4)*SlowDownProportion)/100;
                       }                       
                    }  
                }
                else
                {
                    if(SteeringSVCOutTemp>pVars->SVC_Calibration.SteeringChMiddle)
                    {
                       if(DeltSteering>OutPutHigh_MAX*pVars->pAdvancedSVC->ForwardSpeed)
                       {
                          SteeringSVCOutTemp+=((SteeringSVCOut-SteeringSVCOutTemp+SteeringChInputTemp)>>4); 
                       }   
                       else
                       {
                           SlowDownProportion= DeltSteering*100/OutPutHigh_MAX;
                           if(SlowDownProportion<(100-pVars->pAdvancedSVC->ReturnSpeed))
                           {
                               SlowDownProportion = 100-pVars->pAdvancedSVC->ReturnSpeed;                               
                           }
                           SlowDownProportion=SlowDownProportion>>1;
                           if(SlowDownProportion<5)
                           {
                               SlowDownProportion=5;
                           }
                           SteeringSVCOutTemp+=(((SteeringSVCOut-SteeringSVCOutTemp+SteeringChInputTemp)>>4)*SlowDownProportion)/100;
                       }
                    }
                    else
                    {
                       if(DeltSteering>(0-OutPutLow_MAX*pVars->pAdvancedSVC->ForwardSpeed))
                       {
                          SteeringSVCOutTemp+=((SteeringSVCOut-SteeringSVCOutTemp+SteeringChInputTemp)>>4); 
                       }   
                       else
                       {
                           SlowDownProportion= DeltSteering*100/(0-OutPutLow_MAX);
                           if(SlowDownProportion<(100-pVars->pAdvancedSVC->ReturnSpeed))
                           {
                               SlowDownProportion = 100-pVars->pAdvancedSVC->ReturnSpeed;                               
                           }
                           SlowDownProportion=SlowDownProportion>>1;
                           if(SlowDownProportion<5)
                           {
                               SlowDownProportion=5;
                           }
                           SteeringSVCOutTemp+=(((SteeringSVCOut-SteeringSVCOutTemp+SteeringChInputTemp)>>4)*SlowDownProportion)/100;
                       }                       
                    }                                                   
                }

                SteeringChOutputTemp =SteeringSVCOutTemp;  
            }
            else
            {
                 // SteeringSVCOutTemp+= (((SteeringSVCOut - SteeringSVCOutTemp)>>3)*100+50)/100; //>>3这个参数可以实际跑的时候修改，改大可能会出现低频摆动，消除高频震荡，反之效果则相反                                        
                  SteeringSVCOutTemp= SteeringSVCOut;// - SteeringSVCOutTemp)>>3)*100+50)/100; //>>3这个参数可以实际跑的时候修改，改大可能会出现低频摆动，消除高频震荡，反之效果则相反 
                SteeringChOutputTemp =SteeringSVCOutTemp + SteeringChInputTemp;                
            }
        }       
        // Endpoint Steering        
        pVars->SteeringChOutput  =  (signed short)EndputLimit(pVars->pSVC->Reserved,SteeringChOutputTemp,OutPutHigh_MAX,OutPutLow_MAX);  
        
        if(pVars->SupportAdvancedSVC&& Gyro_SVC_Vars.RXType!=0 &&pVars->pSVC->ESPMode==0)
        {    
            if(pVars->SteeringChOutput - pVars->SteeringChInput>50 || pVars->SteeringChOutput - pVars->SteeringChInput<-50)
            {
                if(pVars->SteeringChOutput>pVars->SteeringChInput && SteeringChoutputOld>pVars->SteeringChInput )
                {
                    if( pVars->SteeringChOutput-SteeringChoutputOld>50)
                    {
                        TurnNum++;
                        ReturnNum=0;
                    }
                    else if( pVars->SteeringChOutput-SteeringChoutputOld<-50)
                    {
                        TurnNum=0;
                        ReturnNum++;
                    }
                }
                else if(pVars->SteeringChOutput<pVars->SteeringChInput && SteeringChoutputOld<pVars->SteeringChInput)
                {
                    if( pVars->SteeringChOutput-SteeringChoutputOld<-50)
                    {
                        TurnNum++;
                        ReturnNum=0;
                    }
                    else if( pVars->SteeringChOutput-SteeringChoutputOld>50)
                    {
                        TurnNum=0;
                        ReturnNum++;
                    }
                }           
            }
            if(TurnNum>=3)
            {
                TurnNum=3;
                pVars->SteeringType=TURN;
            }
            else if(ReturnNum>=3)
            {
                ReturnNum=3;
                pVars->SteeringType=RETURN;
            }
            //
            if(SYS_SysTickMs-Gyro_SVC_Vars.Gyro_TaskMs2>=4)
            {
                Gyro_SVC_Vars.Gyro_TaskMs2=SYS_SysTickMs;
                SteeringChoutputOld=pVars->SteeringChOutput ;
            }
        }             
        //ThrottleSVCOut
         if(pVars->MoveMode!=MODE_BRAKE )
         {
            ThrottleSVCOut = ((72*pVars->pSVC->ThrottleGain)*GyroZdataUse)/1000;
            if(ThrottleSVCOut<0)
            {
                ThrottleSVCOut=0-ThrottleSVCOut;
            }
            
            //ThrottleReverse
            if(pVars->MoveMode!=MODE_BRAKE)
            {
				ThrottleChOutputTemp = pVars->ThrottleChInput;
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
         }
         else
         {
			pVars->ThrottleChOutput = pVars->ThrottleChInput;
         }
    }
}

static signed short FilterBuf[100]={0};
static void Gyro_DataFilter(signed short In_GyroData,signed short *pFiltedData,unsigned char AdvancedSVC_Flg,unsigned char FilterNum)
{
    static unsigned char filter_cnt=0;
    signed short DataMax,DataMin;
    signed long BufSum=0;
 
    if(AdvancedSVC_Flg)
    {
        if(FilterNum<=1)
        {
          *pFiltedData=  In_GyroData;
        }
        else
        {
            FilterBuf[filter_cnt]=In_GyroData;
            filter_cnt++; 
            if(filter_cnt>=FilterNum)
                filter_cnt=0; 
            for(unsigned char i=0;i<FilterNum;i++)
            {
                BufSum+=FilterBuf[i];
            }
            *pFiltedData = (signed short)(BufSum/FilterNum);  
        }
    }
    else
    {
        FilterBuf[filter_cnt]=In_GyroData;
        filter_cnt++; 
        if(filter_cnt>=34)
            filter_cnt=0;
        DataMax = FilterBuf[0];
        DataMin = FilterBuf[0];
        BufSum = FilterBuf[0];
        for(unsigned char i=1;i<34;i++)
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
        *pFiltedData = (signed short)(BufSum>>5);        
    }  
}




static void Gyro_ReadData(signed short *pGyroZ_Data,unsigned char AdvancedSVC_Flg,unsigned char FilterNum)
{
	unsigned char GyroData[3],GyroIDRead; 
	short  GyroZDataTemp;  
	if(Gyro_SVC_Vars.GyroType==MPU6887)
	{
		memset(GyroData,0x00,3);
		Gyro_Read(0x75,GyroData,1);
		GyroIDRead = GyroData[0];//给6887P判断用
		GyroData[0] &= 0x7E;
		if((GyroData[0] == 0x68 || GyroData[0] == 0x2E) || (GyroIDRead == 0x0F))  //陀螺仪 6887 和 6887P
		{
			if(Gyro_SVC_Vars.GyroInitFlg == FALSE)
			{
				Gyro_Init();
			}
			Gyro_Read(0x47,&GyroData[1],2);
			GyroZDataTemp = (short)(((short)GyroData[1])<<8)|GyroData[2];
			Gyro_DataFilter(GyroZDataTemp,pGyroZ_Data,AdvancedSVC_Flg,FilterNum);
			Gyro_SVC_Vars.GyroConnecedFlg  = TRUE;        
		}
		else
		{
			Gyro_SVC_Vars.GyroType =None;
			Gyro_SVC_Vars.GyroInitFlg = FALSE;
			Gyro_SVC_Vars.GyroConnecedFlg  = FALSE;
			*pGyroZ_Data = 0x7fff;
		}
	}
	else  if(Gyro_SVC_Vars.GyroType==LSM6DSDTR)
	{
		memset(GyroData,0x00,3);
		Gyro_Read(0x0f,GyroData,1);
		GyroData[0] &= 0x7E;	
		if(GyroData[0] == 0x6A)
		{
			if(Gyro_SVC_Vars.GyroInitFlg == FALSE)
			{
				Gyro_Init();
			}
			Gyro_Read(0x26,&GyroData[1],2);
			GyroZDataTemp = (short)(((short)GyroData[2])<<8)|GyroData[1];
			Gyro_DataFilter(GyroZDataTemp,pGyroZ_Data,AdvancedSVC_Flg,FilterNum);
			Gyro_SVC_Vars.GyroConnecedFlg  = TRUE;        
		}
		else
		{
			Gyro_SVC_Vars.GyroType =None;
			Gyro_SVC_Vars.GyroInitFlg = FALSE;
			Gyro_SVC_Vars.GyroConnecedFlg  = FALSE;
			*pGyroZ_Data = 0x7fff;
		}
	}
	else
	{
		Gyro_Init();
	}
}


static void GyroTypeJugment(void)
{
	unsigned char GyroData[1];
	unsigned char GyroIDRead;
	
	GYRO_ADDRESS=GYRO_ADDRESS_MPU6887;
	Gyro_Read(0x75,GyroData,1);
	GyroIDRead = GyroData[0];//给6887P判断用
	GyroData[0] &= 0x7E;
	if((GyroData[0] == 0x68 || GyroData[0] == 0x2E) || (GyroIDRead == 0x0F))  //陀螺仪 6887 和 6887P
	{
		Gyro_SVC_Vars.GyroType=MPU6887;
	}
	else
	{
		GYRO_ADDRESS=GYRO_ADDRESS_LSM6DSDTR;		
		Gyro_Write(0x01,&GyroData ,1);		
		Gyro_Read(0x0f,&GyroData,1);
		if(GyroData[0]==0x6A)
		{
			Gyro_SVC_Vars.GyroType=LSM6DSDTR;
		}		
	}	
}


void Gyro_Init(void)
{	
	GyroTypeJugment();
	if(Gyro_SVC_Vars.GyroType==MPU6887)
	{
		unsigned char GyroData[1];
		
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
		//±2000 dps
		GyroData[0] = 0x18;
		Gyro_Write(0x1b,GyroData,1);
		//
		GyroData[0] = 0x30;
		Gyro_Write(0x37,GyroData,1);
		//
		GyroData[0] = 0x01;
		Gyro_Write(0x38,GyroData,1);
		
		// Init paremeter
		Gyro_SVC_Vars.pSVC=&APP_Config.C.RF.SVC;
        Gyro_SVC_Vars.pAdvancedSVC= &APP_Config.C.RF.AdvancedSVC;
        Gyro_SVC_Vars.RXType=&APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFCommon.FeedbackRatio;
		Gyro_SVC_Vars.GyroConnecedFlg  = TRUE; 
		Gyro_SVC_Vars.GyroInitFlg = TRUE;
		Gyro_SVC_Vars.CalibrationMidleRestFlg = FALSE;
		Gyro_LoadSVCCalibration();
	}
	else if(Gyro_SVC_Vars.GyroType==LSM6DSDTR)
	{
		unsigned char temp;
		temp=0x00;
		Gyro_Write(0x01,&temp,1);

		temp=0x3E;
		Gyro_Write(0x0A,&temp,1);
		
		temp=0x7F;
		Gyro_Write(0x10,&temp,1);
		
		temp=0x9C;
		Gyro_Write(0x11,&temp,1);
		
		temp=0x02;
		Gyro_Write(0x13,&temp,1);
		
        temp=0x62;
		Gyro_Write(0x15,&temp,1);
		
		temp=0xC1;
		Gyro_Write(0x17,&temp,1);		
		
		// Init paremeter
		Gyro_SVC_Vars.pSVC=&APP_Config.C.RF.SVC;
        Gyro_SVC_Vars.RXType=&APP_Config.BoundConfig.TransportBoundConfig.LinkBoundConfig.RFCommon.FeedbackRatio;
        Gyro_SVC_Vars.pAdvancedSVC= &APP_Config.C.RF.AdvancedSVC;
		Gyro_SVC_Vars.GyroConnecedFlg  = TRUE; 
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
	if(SYS_SysTickMs-Gyro_SVC_Vars.Gyro_TaskMs>=1)
	{
		Gyro_SVC_Vars.Gyro_TaskMs=SYS_SysTickMs;	
        if(Gyro_SVC_Vars.SupportAdvancedSVC)
        {
            Gyro_ReadData(&Gyro_SVC_Vars.GyroZdata,Gyro_SVC_Vars.SupportAdvancedSVC,(Gyro_SVC_Vars.pAdvancedSVC->Filter)>>1);
//            Gyro_ReadData(&Gyro_SVC_Vars.GyroZdata,Gyro_SVC_Vars.SupportAdvancedSVC,NULL);
//            Gyro_ReadData(&Gyro_SVC_Vars.GyroZdata,Gyro_SVC_Vars.SupportAdvancedSVC,6);
        }
        else
        {
            Gyro_ReadData(&Gyro_SVC_Vars.GyroZdata,NULL,NULL);
        }
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





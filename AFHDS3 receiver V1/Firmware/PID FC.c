#include "PID FC.h"

 //200---20hz
//static Butter_Parameter Control_Device_Div_LPF_Parameter={
//  {1.0f,                -1.14298050254f,    0.4128015980962f},
//  {0.06745527388907f,   0.1349105477781f,   0.06745527388907f}};

//500hz---30hz
static Butter_Parameter Control_Device_Error_LPF_Parameter={
	{1.0f, -1.4754804435926461f,  0.58691950806119031f},
	{0.027859766117136031f,    0.055719532234272062f,    0.027859766117136031f}
};

  
static float Control_Device_LPF(float curr_inputer,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
{
	/* 获取最新x(n) */
	static int LPB_Cnt=0;
	Buffer->Input_Butter[2]=curr_inputer;
	if(LPB_Cnt>=100)
	{
		/* Butterworth滤波 */
		Buffer->Output_Butter[2]=
		Parameter->b[0] * Buffer->Input_Butter[2]
		+Parameter->b[1] * Buffer->Input_Butter[1]
		+Parameter->b[2] * Buffer->Input_Butter[0]
		-Parameter->a[1] * Buffer->Output_Butter[1]
		-Parameter->a[2] * Buffer->Output_Butter[0];
	}
	else
	{
		Buffer->Output_Butter[2]=Buffer->Input_Butter[2];
		LPB_Cnt++;
	}
	/* x(n) 序列保存 */
	Buffer->Input_Butter[0]=Buffer->Input_Butter[1];
	Buffer->Input_Butter[1]=Buffer->Input_Butter[2];
	/* y(n) 序列保存 */
	Buffer->Output_Butter[0]=Buffer->Output_Butter[1];
	Buffer->Output_Butter[1]=Buffer->Output_Butter[2];

	return Buffer->Output_Butter[2];
}


void PID_Control(sPID_Controller *Control)
{	
	//Error
	Control->ErrorLast = Control->Error;
	Control->Error = Control->Expect - Control->FeedBack;
	//ErrorLPF
	if(Control->ErrorLPF_Flag)
	{
		Control->ErrorLastLPF = Control->ErrorLPF;
		Control->ErrorLPF = Control_Device_LPF(Control->Error,
												&Control->Control_Device_LPF_Buffer,
													&Control_Device_Error_LPF_Parameter);	
	}
	//Error Limit
	if(Control->ErrorLimitFlag)
	{
		if(Control->ErrorLPF_Flag)
		{
			if(Control->ErrorLPF >= Control->Error_Max)
			{
				Control->ErrorLPF = Control->Error_Max;
			}
			else if(Control->ErrorLPF <= -Control->Error_Max)
			{
				Control->ErrorLPF = -Control->Error_Max;
			}			
		}
		else
		{
			if(Control->Error >= Control->Error_Max)
			{
				Control->Error = Control->Error_Max;
			}
			else if(Control->Error <= -Control->Error_Max)
			{
				Control->Error = -Control->Error_Max;
			}		
		}
	}
	
	//Proportion
	if(Control->ErrorLPF_Flag)
	{
		Control->Proportion = Control->ErrorLPF*Control->PID.Kp;
	}
	else
	{
		Control->Proportion = Control->Error*Control->PID.Kp;
	}
	
	//Differential
	//Differential Forward
	if(Control->Diff_ForwardFlag)
	{
		Control->Diff_Error = Control->FeedBack - Control->FeedBackLast;
		Control->FeedBackLast = Control->FeedBack;
	}
	else
	{
		if(Control->ErrorLPF_Flag)
		{
			Control->Diff_Error = Control->ErrorLPF - Control->ErrorLastLPF;
		}
		else
		{
			Control->Diff_Error = Control->Error - Control->ErrorLast;
		}
	}
	//Differential Incomplete
	if(Control->Diff_IncompleteFlag)
	{
		for(signed char i=1;i<5;i++)
		{
			Control->Diff_Error_History[i] = Control->Diff_Error_History[i+1];
		}
		Control->Diff_Error_History[0] = Control_Device_LPF(Control->Diff_Error,
																&Control->Control_Device_LPF_Buffererr,
																		&Control_Device_Error_LPF_Parameter);
		
		Control->Differential = Control->Diff_Error_History[Control->Diff_IncompleteDelay]*Control->PID.Kd;
	}
	else
	{
		Control->Differential = Control->Diff_Error*Control->PID.Kd;
	}	
	
	//Integrate	
	//Integrate Separation
	if(Control->IntegrateSeparationFlag)
	{
		if(Control->Error < Control->IntegrateSeparationError || Control->Error > -Control->IntegrateSeparationError)
		{
			if(Control->ErrorLPF_Flag)
			{
				Control->Integrate += Control->ErrorLPF*Control->PID.Ki;
			}
			else
			{
				Control->Integrate += Control->Error*Control->PID.Ki;
			}
		}
	}
	else
	{
		if(Control->ErrorLPF_Flag)
		{
			Control->Integrate += Control->ErrorLPF*Control->PID.Ki;
		}
		else
		{
			Control->Integrate += Control->Error*Control->PID.Ki;
		}
	}
	//Integrate Limit
	if(Control->IntegrateLimitFlag)
	{
		if(Control->Integrate > Control->Integrate_Max)
		{
			Control->Integrate = Control->Integrate_Max;
		}
		else if(Control->Integrate < -Control->Integrate_Max)
		{
			Control->Integrate = -Control->Integrate_Max;
		}
	}
	
	//OutPut	
	Control->OutPut = Control->Proportion + Control->Integrate + Control->Differential;
	//OutPut Limit
	if(Control->OutPut > Control->OutPut_Limit)
	{
		Control->OutPut = Control->OutPut_Limit;
	}
	else if(Control->OutPut < -Control->OutPut_Limit)
	{
		Control->OutPut = -Control->OutPut_Limit;
	}
}




#ifndef _PIDFC_H
#define _PIDFC_H



typedef struct  __attribute__((packed))
{
 float Input_Butter[3];
 float Output_Butter[3];
}Butter_BufferData;


typedef struct  __attribute__((packed))
{
 const float a[3];
 const float b[3];
}Butter_Parameter;


typedef struct  __attribute__((packed))
{
    float Kp;
    float Ki;
    float Kd;
}sPID;


typedef struct  __attribute__((packed))
{
    unsigned char ErrorLimitFlag :1;           //偏差限幅标志
    unsigned char ErrorLPF_Flag :1;            //偏差限幅标志
	
    unsigned char IntegrateLimitFlag :1;       //积分限幅标志
    unsigned char IntegrateSeparationFlag :1;  //积分分离标志
	
	unsigned char Diff_ForwardFlag:1;   //微分先行
	unsigned char Diff_IncompleteFlag:1;//不完全微分
	unsigned char PAD:2;	
	unsigned char Diff_IncompleteDelay; //积分分离延时 max 4 
	sPID PID;
	
    float Expect;   //期望
    float FeedBack; //反馈值
    float FeedBackLast;//上次反馈值	
	
    float Error;    //偏差
	float ErrorLPF; //偏差低通
    float ErrorLast;//上次偏差
    float ErrorLastLPF;
    float Error_Max;//偏差限幅值
	
	float Proportion;
	
    float Integrate;    //积分值	
    float IntegrateSeparationError;//积分分离偏差值
    float Integrate_Max;//积分限幅值
	
	float Differential;
    float Diff_Error;           //微分量
	float Diff_Error_History[5];//历史微分量
    float Diff_Error_LPF;
	
    float OutPut;      //控制器总输出
    float OutPut_Limit;//输出限幅
    Butter_BufferData Control_Device_LPF_Buffererr;//控制器低通输入输出缓冲
    Butter_BufferData Control_Device_LPF_Buffer;//控制器低通输入输出缓冲
	
}sPID_Controller;



extern void PID_Control(sPID_Controller *Control);

#endif

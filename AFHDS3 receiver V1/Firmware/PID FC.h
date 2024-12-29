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
    unsigned char ErrorLimitFlag :1;           //ƫ���޷���־
    unsigned char ErrorLPF_Flag :1;            //ƫ���޷���־
	
    unsigned char IntegrateLimitFlag :1;       //�����޷���־
    unsigned char IntegrateSeparationFlag :1;  //���ַ����־
	
	unsigned char Diff_ForwardFlag:1;   //΢������
	unsigned char Diff_IncompleteFlag:1;//����ȫ΢��
	unsigned char PAD:2;	
	unsigned char Diff_IncompleteDelay; //���ַ�����ʱ max 4 
	sPID PID;
	
    float Expect;   //����
    float FeedBack; //����ֵ
    float FeedBackLast;//�ϴη���ֵ	
	
    float Error;    //ƫ��
	float ErrorLPF; //ƫ���ͨ
    float ErrorLast;//�ϴ�ƫ��
    float ErrorLastLPF;
    float Error_Max;//ƫ���޷�ֵ
	
	float Proportion;
	
    float Integrate;    //����ֵ	
    float IntegrateSeparationError;//���ַ���ƫ��ֵ
    float Integrate_Max;//�����޷�ֵ
	
	float Differential;
    float Diff_Error;           //΢����
	float Diff_Error_History[5];//��ʷ΢����
    float Diff_Error_LPF;
	
    float OutPut;      //�����������
    float OutPut_Limit;//����޷�
    Butter_BufferData Control_Device_LPF_Buffererr;//��������ͨ�����������
    Butter_BufferData Control_Device_LPF_Buffer;//��������ͨ�����������
	
}sPID_Controller;



extern void PID_Control(sPID_Controller *Control);

#endif

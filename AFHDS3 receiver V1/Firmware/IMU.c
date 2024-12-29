#include "IMU.h"

#include <stdio.h>
#include <math.h>

#ifdef DEBUG
extern void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw);
#endif

static float Kp = 1.0f;		/*��������*/
static float Ki = 0.008f;		/*��������*/
static float exInt = 0.0f;
static float eyInt = 0.0f;
static float ezInt = 0.0f;		/*��������ۼ�*/

static float q0 = 1.0f;	/*��Ԫ��*/
static float q1 = 0.0f;
static float q2 = 0.0f;
static float q3 = 0.0f;	

#define DEG2RAD		0.017453293f	/* ��ת���� ��/180 */
#define RAD2DEG		57.29578f		/* ����ת�� 180/�� */

float invSqrt(float x);	/*���ٿ�ƽ����*/

void IMUupdate(float half_T,sIMU_Data *Imu)	/*�����ں� �����˲�*/
{
	float normalise,gx,gy,gz,ax,ay,az;
	float ex, ey, ez;
	float q0s, q1s, q2s, q3s;	/*��Ԫ����ƽ��*/
	static float R11,R21;		/*����(1,1),(2,1)��*/
	static float vecxZ, vecyZ, veczZ;	/*��������ϵ�µ�Z��������*/	

	gx= Imu->Gyro.x;
	gy= Imu->Gyro.y;
	gz= Imu->Gyro.z;
   
	ax= Imu->Acc.x;
	ay= Imu->Acc.y;
	az= Imu->Acc.z;
	
	gx = gx * DEG2RAD;	/* ��ת���� */
	gy = gy * DEG2RAD;
	gz = gz * DEG2RAD;

//	ax = ax * (float)((2 * 8) / 65536.0);
//	ay = ay * (float)((2 * 8) / 65536.0);
//	az = az * (float)((2 * 8) / 65536.0);
	
	ax = ax * 0.244f;
	ay = ay * 0.244f;
	az = az * 0.244f;
	
	/* ĳһ��������ٶȲ�Ϊ0 */
	if((ax != 0.0f) || (ay != 0.0f) || (az != 0.0f))
	{
		/*��λ�����ټƲ���ֵ*/
		normalise = invSqrt(ax * ax + ay * ay + az * az);
		ax *= normalise;
		ay *= normalise;
		az *= normalise;
		
		/*���ټƶ�ȡ�ķ������������ټƷ���Ĳ�ֵ����������˼���*/
		ex = (ay * veczZ - az * vecyZ);
		ey = (az * vecxZ - ax * veczZ);
		ez = (ax * vecyZ - ay * vecxZ);
	
		/*����ۼƣ�����ֳ������*/
		exInt += Ki * ex * 2* half_T ;  
		eyInt += Ki * ey * 2* half_T ;
		ezInt += Ki * ez * 2* half_T ;
		
		/*�ò���������PI����������ƫ�����������ݶ����е�ƫ����*/
		gx += Kp * ex + exInt;
		gy += Kp * ey + eyInt;
		gz += Kp * ez + ezInt;
	}
	/* һ�׽����㷨����Ԫ���˶�ѧ���̵���ɢ����ʽ�ͻ��� */
	q0 += (-q1 *gx - q2 *gy - q3 *gz) * half_T;
	q1 += ( q0 *gx + q2 *gz - q3 *gy) * half_T;
	q2 += ( q0 *gy - q1 *gz + q3 *gx) * half_T;
	q3 += ( q0 *gz + q1 *gy - q2 *gx) * half_T;
	
	/*��λ����Ԫ��*/
	normalise = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= normalise;
	q1 *= normalise;
	q2 *= normalise;
	q3 *= normalise;
	/*��Ԫ����ƽ��*/
	q0s = q0 * q0;
	q1s = q1 * q1;
	q2s = q2 * q2;
	q3s = q3 * q3;
	
	R11 = q0s + q1s - q2s - q3s;	/*����(1,1)��*/
	R21 = 2 * (q1 * q2 + q0 * q3);	/*����(2,1)��*/

	/*��������ϵ�µ�Z��������*/
	vecxZ = 2 * (q1 * q3 - q0 * q2);/*����(3,1)��*/
	vecyZ = 2 * (q0 * q1 + q2 * q3);/*����(3,2)��*/
	veczZ = q0s - q1s - q2s + q3s;	/*����(3,3)��*/
		
	if (vecxZ>1) vecxZ=1;
	else if (vecxZ<-1) vecxZ=-1;
	
	/*����roll pitch yaw ŷ����*/
	Imu->Pitch = -asinf(vecxZ) * RAD2DEG*1.06f; 
	Imu->Roll  = atan2f(vecyZ, veczZ) * RAD2DEG;
	Imu->Yaw   = atan2f(R21, R11) * RAD2DEG;
}


float invSqrt(float x)	/*���ٿ�ƽ����*/
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}


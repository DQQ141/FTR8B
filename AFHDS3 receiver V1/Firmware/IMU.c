#include "IMU.h"

#include <stdio.h>
#include <math.h>

#ifdef DEBUG
extern void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw);
#endif

static float Kp = 1.0f;		/*比例增益*/
static float Ki = 0.008f;		/*积分增益*/
static float exInt = 0.0f;
static float eyInt = 0.0f;
static float ezInt = 0.0f;		/*积分误差累计*/

static float q0 = 1.0f;	/*四元数*/
static float q1 = 0.0f;
static float q2 = 0.0f;
static float q3 = 0.0f;	

#define DEG2RAD		0.017453293f	/* 度转弧度 π/180 */
#define RAD2DEG		57.29578f		/* 弧度转度 180/π */

float invSqrt(float x);	/*快速开平方求倒*/

void IMUupdate(float half_T,sIMU_Data *Imu)	/*数据融合 互补滤波*/
{
	float normalise,gx,gy,gz,ax,ay,az;
	float ex, ey, ez;
	float q0s, q1s, q2s, q3s;	/*四元数的平方*/
	static float R11,R21;		/*矩阵(1,1),(2,1)项*/
	static float vecxZ, vecyZ, veczZ;	/*机体坐标系下的Z方向向量*/	

	gx= Imu->Gyro.x;
	gy= Imu->Gyro.y;
	gz= Imu->Gyro.z;
   
	ax= Imu->Acc.x;
	ay= Imu->Acc.y;
	az= Imu->Acc.z;
	
	gx = gx * DEG2RAD;	/* 度转弧度 */
	gy = gy * DEG2RAD;
	gz = gz * DEG2RAD;

//	ax = ax * (float)((2 * 8) / 65536.0);
//	ay = ay * (float)((2 * 8) / 65536.0);
//	az = az * (float)((2 * 8) / 65536.0);
	
	ax = ax * 0.244f;
	ay = ay * 0.244f;
	az = az * 0.244f;
	
	/* 某一个方向加速度不为0 */
	if((ax != 0.0f) || (ay != 0.0f) || (az != 0.0f))
	{
		/*单位化加速计测量值*/
		normalise = invSqrt(ax * ax + ay * ay + az * az);
		ax *= normalise;
		ay *= normalise;
		az *= normalise;
		
		/*加速计读取的方向与重力加速计方向的差值，用向量叉乘计算*/
		ex = (ay * veczZ - az * vecyZ);
		ey = (az * vecxZ - ax * veczZ);
		ez = (ax * vecyZ - ay * vecxZ);
	
		/*误差累计，与积分常数相乘*/
		exInt += Ki * ex * 2* half_T ;  
		eyInt += Ki * ey * 2* half_T ;
		ezInt += Ki * ez * 2* half_T ;
		
		/*用叉积误差来做PI修正陀螺零偏，即抵消陀螺读数中的偏移量*/
		gx += Kp * ex + exInt;
		gy += Kp * ey + eyInt;
		gz += Kp * ez + ezInt;
	}
	/* 一阶近似算法，四元数运动学方程的离散化形式和积分 */
	q0 += (-q1 *gx - q2 *gy - q3 *gz) * half_T;
	q1 += ( q0 *gx + q2 *gz - q3 *gy) * half_T;
	q2 += ( q0 *gy - q1 *gz + q3 *gx) * half_T;
	q3 += ( q0 *gz + q1 *gy - q2 *gx) * half_T;
	
	/*单位化四元数*/
	normalise = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= normalise;
	q1 *= normalise;
	q2 *= normalise;
	q3 *= normalise;
	/*四元数的平方*/
	q0s = q0 * q0;
	q1s = q1 * q1;
	q2s = q2 * q2;
	q3s = q3 * q3;
	
	R11 = q0s + q1s - q2s - q3s;	/*矩阵(1,1)项*/
	R21 = 2 * (q1 * q2 + q0 * q3);	/*矩阵(2,1)项*/

	/*机体坐标系下的Z方向向量*/
	vecxZ = 2 * (q1 * q3 - q0 * q2);/*矩阵(3,1)项*/
	vecyZ = 2 * (q0 * q1 + q2 * q3);/*矩阵(3,2)项*/
	veczZ = q0s - q1s - q2s + q3s;	/*矩阵(3,3)项*/
		
	if (vecxZ>1) vecxZ=1;
	else if (vecxZ<-1) vecxZ=-1;
	
	/*计算roll pitch yaw 欧拉角*/
	Imu->Pitch = -asinf(vecxZ) * RAD2DEG*1.06f; 
	Imu->Roll  = atan2f(vecyZ, veczZ) * RAD2DEG;
	Imu->Yaw   = atan2f(R21, R11) * RAD2DEG;
}


float invSqrt(float x)	/*快速开平方求倒*/
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}


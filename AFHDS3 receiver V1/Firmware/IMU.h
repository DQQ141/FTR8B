#ifndef _IMU_H
#define _IMU_H

typedef struct  __attribute__((packed))
{
	float x;
	float y;
	float z;
}sImuSensor_FloatData;

typedef struct __attribute__((packed))
{
	sImuSensor_FloatData Gyro;
	sImuSensor_FloatData  Acc;
	float Roll;
	float Pitch;
	float Yaw;
}sIMU_Data;


void IMUupdate(float half_T,sIMU_Data *Imu);

#endif


#include <Arduino.h>
#include <Wire.h>

// https://github.com/bolderflight/MPU9250
#include <MPU9250.h>

#define MPU6050_ADDR         0x68 // MPU-6050 device address
#define MPU6050_SMPLRT_DIV   0x19 // MPU-6050 register address
#define MPU6050_CONFIG       0x1a
#define MPU6050_GYRO_CONFIG  0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_WHO_AM_I     0x75
#define MPU6050_PWR_MGMT_1   0x6b

MPU9250 IMU(Wire, MPU6050_ADDR);

int autoCalibrateAccel()
{
  int status;
  status = IMU.calibrateAccel();
  if (status < 0)
  {
    Serial.printf("Accel Sensor Calibration Failed.");
    return 0;
  }
  
  return 1;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(23, OUTPUT);
  
  IMU.begin();
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_8G);
  IMU.setGyroRange(MPU9250::GYRO_RANGE_500DPS);
}

void loop() {
  // put your main code here, to run repeatedly:
  float ax, ay, az, gx, gy, gz;
  IMU.readSensor();
  ax = IMU.getAccelX_mss();
  ay = IMU.getAccelY_mss();
  az = IMU.getAccelZ_mss();
  gx = IMU.getGyroX_rads();
  gy = IMU.getGyroY_rads();
  gz = IMU.getGyroZ_rads();

  Serial.printf("%.2f %.2f %.2f\n", gx, gy, gz);

  delay(10);
}

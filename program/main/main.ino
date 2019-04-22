#include <Arduino.h>
#include <Wire.h>

// https://github.com/bolderflight/MPU9250
#include <MPU9250.h>

#define MPU6050_ADDR         0x68 // MPU-6050 device address

MPU9250 IMU(Wire, MPU6050_ADDR);

// 振り向き検知の最低角速度[rad/s]
const float lookback_speed_threshold = 5.0;

// 振り向き検知の角度
const float lookback_angle_threshold = PI/2.0;

// 大きく振り返った時に2回以上検知されないようにするための
// 振り向きの最低時間間隔[ms]
const long lookback_min_interval_ms = 1000;

/**
 * 加速度センサーのキャリブレーションモード
 * 現在不使用
 */ 
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

/**
 * ジャイロ値をもとに姿勢角の更新をする
 * Z軸周り(ヨー方向,厳密には重力方向周り)の姿勢は
 * カルマンフィルタを使うことはできないので
 * ジャイロの積分をそのまま使用している。
 */ 
double updateAttitudeZ(float gz)
{
  static double angle_z=0;
  static unsigned long t0 = millis();
  unsigned long t = millis();

  angle_z += gz * (t - t0)/1000;
  t0 = t;
  return angle_z;
}

double calculateAngleDiff(double angle1, double angle2)
{
  double diff = angle1 - angle2;
  while(diff >= PI)
  {
    diff -= PI;
  }
  while(diff <= -PI)
  {
    diff += PI;
  }

  return diff;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(23, OUTPUT);
  
  IMU.begin();
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_8G);
  IMU.setGyroRange(MPU9250::GYRO_RANGE_500DPS);
}

int LookBackDetection(float gz)
{
  static unsigned long t0 = 0;
  static double last_angle_z;
  double angle_z;

  angle_z = updateAttitudeZ(gz);

  // 一定速度以下の振り向きは検知しないようにしている
  if(gz <= lookback_speed_threshold &&
     gz >= -lookback_speed_threshold)
  {
    last_angle_z = angle_z;
  }

  double diff = calculateAngleDiff(angle_z, last_angle_z);

  // 標準のabs関数だと関数マクロをしようしているからか
  // 挙動がおかしいのでこうしている
  double diff_abs = (diff >= 0 ? diff : -diff);

  if(diff_abs >= lookback_angle_threshold)
  {
    // 大きく振り向いた時に2回以上検知されないようにしている
    if (millis() - t0 > lookback_min_interval_ms)
    {
      // 振り向き検知
      Serial.println("Detected!!");
      last_angle_z = angle_z;
      t0 = millis();
      return 1;
    }
    last_angle_z = angle_z;
    t0 = millis();
  }

  return 0;
}

void onLookBack()
{
  
}

void loop() {
  // put your main code here, to run repeatedly:
  float ax, ay, az, gx, gy, gz;
  double angle_z;
  static double last_angle_z;
  static bool isDetected;
  static unsigned long t0 = 0;

  IMU.readSensor();
  gz = IMU.getGyroZ_rads();

  if(LookBackDetection(gz) != 0)
  {
    onLookBack();
  }

  delay(10);
}

#include <Arduino.h>
#include <Wire.h>

// https://github.com/bolderflight/MPU9250
#include <MPU9250.h>

#include <WiFi.h>

#include "communication.h"
#include "oled.h"

#define MPU6050_ADDR 0x68 // MPU-6050 device address

MPU9250 IMU(Wire, MPU6050_ADDR);

// 振り向き検知の最低角速度[rad/s]
const float lookback_speed_threshold = 3.0;

// 振り向きにかかった時間に応じたレベル[ms]
const unsigned long lookback_period_level1 = 250;
const unsigned long lookback_period_level2 = 0;
const unsigned long lookback_period_level3 = 0;

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

  delay(500);

  Serial.println("hello world!");
  setupCommunication();
  
  Serial.println("client setup completed");

  setupOLED();
  Serial.println("OLED setup completed");

  clearDisplay();
}

int getLookBackSpeedLevel(unsigned long t_delta)
{
  if(t_delta <= lookback_period_level3)
  {
    return 4;
  }
  else if(t_delta <= lookback_period_level2)
  {
    return 3;
  }
  else if(t_delta <= lookback_period_level1)
  {
    return 2;
  }
  
  return 1;
}

int LookBackDetection(float gz)
{
  static unsigned long t0 = 0;
  static unsigned long t_laststop = 0;
  static double last_angle_z = 0;
  double angle_z;

  angle_z = updateAttitudeZ(gz);

  // 一定速度以下の振り向きは検知しないようにしている
  if(gz <= lookback_speed_threshold &&
     gz >= -lookback_speed_threshold)
  {
    last_angle_z = angle_z;
    t_laststop = millis();
  }

  double diff = calculateAngleDiff(angle_z, last_angle_z);
  
  if(diff >= lookback_angle_threshold)
  {
    unsigned long timedelta = millis() - t_laststop;
    // 大きく振り向いた時に2回以上検知されないようにしている
    if (millis() - t0 > lookback_min_interval_ms)
    {
      
      // 右振り向き検知
      Serial.println("Right Turn Detected!!");
      Serial.print("timedelta: "); Serial.println(timedelta);
      int speed_level = getLookBackSpeedLevel(timedelta);
      last_angle_z = angle_z;

      t0 = millis();
      
      return speed_level;
    }
    last_angle_z = angle_z;
    t0 = millis();
    return 0;
  }
  else if(diff <= -lookback_angle_threshold)
  {
    unsigned long timedelta = millis() - t_laststop;
    // 大きく振り向いた時に2回以上検知されないようにしている
    if (millis() - t0 > lookback_min_interval_ms)
    {
      // 左振り向き検知
      Serial.println("Left Turn Detected!!");
      Serial.print("timedelta: "); Serial.println(timedelta);
      int speed_level = getLookBackSpeedLevel(timedelta);
      last_angle_z = angle_z;
      t0 = millis();
      return -speed_level;
    }
    last_angle_z = angle_z;
    t0 = millis();
    return 0;
  }
  return 0;
}

void onLookBack(int direction)
{
  String message = "";
  char message_payload[10];
  
  if(direction >= 1)
  {
    // 右振り向きの場合
    message += "+";
    drawHeisei();
  }
  else
  {
    // 左振り向きの場合
    clearDisplay();
  }
  message += direction;
  Serial.println(message);
  message.toCharArray(message_payload, 3);
  Serial.println(message_payload);
  sendMessage(message_payload);
}

void loop() {
  // put your main code here, to run repeatedly:
  float gz;
  int direction = 0;

  IMU.readSensor();
  gz = IMU.getGyroX_rads();
  
  direction = LookBackDetection(gz);
  if(direction != 0)
  {
    Serial.print("direction: ");
    Serial.println(direction);
    onLookBack(direction);
  }

  communication_loop();
  
  delay(10);
}

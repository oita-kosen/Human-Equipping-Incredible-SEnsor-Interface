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
const float lookback_speed_threshold = 1.0;

// 振り向きの平均速度に応じたレベル[m/s]
const float lookback_mean_speed_level1 = 4.00;
const float lookback_mean_speed_level2 = 1000;
const float lookback_mean_speed_level3 = 1000;

// 振り向き検知の角
const float lookback_angle_threshold = PI/2.0;
const float lookback_angle_turn_threshold = PI*1.5;

// 大きく振り返った時に2回以上検知されないようにするための
// 振り向きの最低時間間隔[ms]
const long lookback_min_interval_ms = 1000;

long last_lookback_time = 0;
bool is_clear_display = true;

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

  setupOLED();
  Serial.println("OLED setup completed");
  drawHeisei();

  Serial.println("hello world!");
  setupCommunication();
  
  Serial.println("client setup completed");

}

int getLookBackSpeedLevel(unsigned long t_delta, float angle)
{
  float mean_speed = angle / t_delta * 1000;

  if(mean_speed < lookback_mean_speed_level1)
  {
    return 1;
  }
  else if(mean_speed < lookback_mean_speed_level2)
  {
    return 2;
  }
  else if(mean_speed < lookback_mean_speed_level3)
  {
    return 3;
  }
  
  return 4;
}

int LookBackDetection(float gz)
{
  static unsigned long t_laststop = millis();
  static double last_angle_z = 0;
  double angle_z;

  angle_z = updateAttitudeZ(gz);

  // 一定速度以下の振り向きは検知しないようにしている
  if(gz <= lookback_speed_threshold &&
     gz >= -lookback_speed_threshold)
  {
    double diff = calculateAngleDiff(angle_z, last_angle_z);
    double diff_abs = (diff > 0 ) ? diff : -diff;

    if(diff_abs >= lookback_angle_turn_threshold)
    {
      Serial.println("One rotation Turn Detected!!");
      last_angle_z = angle_z;

      last_lookback_time = millis();
      is_clear_display = false;
      return 100;
    }
  
    if(diff <= -lookback_angle_threshold)
    {
      unsigned long timedelta = millis() - t_laststop;
      // 右振り向き検知
      Serial.println("Right Turn Detected!!");
      Serial.print("timedelta: "); Serial.println(timedelta);
      Serial.print("mean speed: "); Serial.println(diff/timedelta * 1000);
      int speed_level = getLookBackSpeedLevel(timedelta, diff_abs);
      last_angle_z = angle_z;

      last_lookback_time = millis();
      is_clear_display = false;
      return speed_level;
    }
    else if(diff >= lookback_angle_threshold)
    {
      unsigned long timedelta = millis() - t_laststop;
      // 左振り向き検知
      Serial.println("Left Turn Detected!!");
      Serial.print("timedelta: "); Serial.println(timedelta);
      Serial.print("mean speed: "); Serial.println(diff/timedelta * 1000);
      int speed_level = getLookBackSpeedLevel(timedelta, diff_abs);
      last_angle_z = angle_z;

      last_lookback_time = millis();
      is_clear_display = false;
      return -speed_level;
    }
    last_angle_z = angle_z;
    t_laststop = millis();
  }
  return 0;
}

void onLookBack(int direction)
{
  String message = "";
  char message_payload[10];

  if(direction == 100)
  {
    message += "turn";
    drawReiWa();
  }
  else{
    if(direction >= 1)
    {
      // 右振り向きの場合
      message += "+";
      message += direction;
      drawMigi();
    }
    else
    {
      // 左振り向きの場合
      message += direction;
      drawHidari();
    }
  }
  
  
  message.toCharArray(message_payload, 10);
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

  if((millis() - last_lookback_time > 2000) && !is_clear_display)
  {
    drawHeisei();
    is_clear_display = true;
  }
  communication_loop();
  
  delay(10);
}

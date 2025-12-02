#ifndef GLOBAL_H
#define GLOBAL_H

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "config.h"

// 全局对象
extern Adafruit_PWMServoDriver pwm;
extern WebServer server;
extern WebSocketsServer webSocket;

// 全局变量
extern int servoAngles[NUM_SERVOS];  // 舵机当前角度（0-270度）
extern String logBuffer;              // 日志缓冲区

#endif

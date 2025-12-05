#ifndef GLOBAL_H
#define GLOBAL_H

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <WiFiManager.h>  // WiFi配置管理库
#include "config.h"

// 全局对象
extern Adafruit_PWMServoDriver pwm;
extern Adafruit_SSD1306 display;
extern WebServer server;
extern WebSocketsServer webSocket;

// 全局变量
extern int servoAngles[NUM_SERVOS];  // 舵机当前角度（0-270度）
extern String logBuffer;              // 日志缓冲区
extern String systemIP;               // 系统IP地址
extern String systemStatus;           // 系统状态信息

#endif

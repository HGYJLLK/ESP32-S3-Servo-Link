// global.h - 全局变量声明
#ifndef GLOBAL_H
#define GLOBAL_H

#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <AccelStepper.h>

// 全局对象
extern Adafruit_PWMServoDriver pwm;
extern WebServer server;
extern WebSocketsServer webSocket;
extern AccelStepper stepperVertical;
extern AccelStepper stepperHorizontal;

// 当前角度数组 (12个舵机)
extern int currentAngles[TOTAL_SERVOS];

// 小力舵机左右值数组 (8个小力舵机: 0-3, 8-11)
extern int smallServoLeftValues[8];  // 左值(松)
extern int smallServoRightValues[8]; // 右值(紧)

// 步进电机位置
extern long verticalPosition;
extern long horizontalPosition;

// 日志缓冲区
extern String logBuffer;

#endif

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

// 当前角度数组
extern int currentAngles[TOTAL_SERVOS];

// 360度舵机中点数组 (可通过Web界面调整)
extern int servo360Centers[NUM_SERVOS_360];

// 步进电机位置
extern long verticalPosition;
extern long horizontalPosition;

// 日志缓冲区
extern String logBuffer;

#endif

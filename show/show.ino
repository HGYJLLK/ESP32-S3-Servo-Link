// show.ino - 主程序入口
// ESP32-S3展示控制系统
// 控制: 12x 180度SG90舵机 (0-3小力, 4-7大力, 8-11小力), 5x 步进电机(丝杆)

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <AccelStepper.h>

#include "config.h"
#include "web_interface.h"
#include "global.h"

// 全局对象实例化
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA9685_ADDR);
WebServer server(WEB_SERVER_PORT);
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);

// 步进电机对象 (DRIVER模式: STEP引脚, DIR引脚)
AccelStepper stepperVertical(AccelStepper::DRIVER, STEPPER_VERTICAL_STEP, STEPPER_VERTICAL_DIR);
AccelStepper stepperHorizontal(AccelStepper::DRIVER, STEPPER_HORIZONTAL_STEP, STEPPER_HORIZONTAL_DIR);

// 全局变量
int currentAngles[TOTAL_SERVOS];           // 12个舵机的当前角度
int smallServoLeftValues[8];               // 8个小力舵机的左值(松)
int smallServoRightValues[8];              // 8个小力舵机的右值(紧)
long verticalPosition = 0;
long horizontalPosition = 0;
String logBuffer = "";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32-S3 Show Control System Starting...");

  // 初始化I2C (用于PCA9685)
  Wire.begin(I2C_SDA, I2C_SCL);

  // 初始化舵机系统
  initServoSystem();

  // 初始化步进电机系统
  initStepperSystem();

  // 连接WiFi
  initWiFi();

  // 初始化Web服务器和WebSocket
  initWebServer();

  Serial.println("System initialization complete!");
  Serial.print("Access web interface at: http://");
  Serial.println(WiFi.localIP());

  addLog("系统初始化完成");
}

void loop() {
  // 处理Web服务器请求
  server.handleClient();

  // 处理WebSocket事件
  webSocket.loop();

  // 运行步进电机 (非阻塞)
  runSteppers();

  // 短暂延时避免看门狗触发
  delay(1);
}

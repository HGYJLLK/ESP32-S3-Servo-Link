/*
 * ESP32-S3 舵机控制系统
 *
 * 项目结构：
 * - esp32s3.ino: 主程序入口
 * - config.h: 配置常量
 * - global.h: 全局变量和对象声明
 * - servo_control.ino: 舵机控制逻辑
 * - web_server.ino: Web服务器和WebSocket处理
 * - web_interface.h: HTML用户界面
 */

#include "global.h"

// 全局对象实例化
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA9685_ADDRESS);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);  // 使用Wire1独立总线
WebServer server(HTTP_PORT);
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);

// 全局变量实例化
int servoAngles[NUM_SERVOS] = {DEFAULT_ANGLE, DEFAULT_ANGLE, DEFAULT_ANGLE, DEFAULT_ANGLE};
String logBuffer = "";
String systemIP = "Connecting...";
String systemStatus = "Init";

void setup() {
  // 初始化串口
  Serial0.begin(SERIAL_BAUD);
  Serial0.println("\n\n=== ESP32S3 舵机控制系统启动 ===");

  // 初始化舵机系统
  initServoSystem();

  // 连接WiFi
  initWiFi();

  // 初始化Web服务器
  initWebServer();
}

void loop() {
  // 处理Web服务器请求
  handleWebServer();

  // 可以在这里添加其他循环任务
}

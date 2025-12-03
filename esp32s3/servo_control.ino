// 舵机控制模块
#include "global.h"

// OLED完整显示更新（静态信息面板）
void updateOLEDDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // 第1行：WiFi SSID
  display.setCursor(0, 0);
  display.print("WiFi:");
  display.println(WIFI_SSID);

  // 第2行：IP地址
  display.setCursor(0, 10);
  display.print("IP:");
  display.println(systemIP);

  // 第3行：分隔线
  display.setCursor(0, 20);
  display.println("--------------------");

  // 第4行：舵机0和1
  display.setCursor(0, 30);
  display.print("S0:");
  if (servoAngles[0] < 100) display.print(" ");
  if (servoAngles[0] < 10) display.print(" ");
  display.print(servoAngles[0]);
  display.print((char)247);  // 度数符号

  display.setCursor(64, 30);
  display.print("S1:");
  if (servoAngles[1] < 100) display.print(" ");
  if (servoAngles[1] < 10) display.print(" ");
  display.print(servoAngles[1]);
  display.print((char)247);

  // 第5行：舵机2和3
  display.setCursor(0, 40);
  display.print("S2:");
  if (servoAngles[2] < 100) display.print(" ");
  if (servoAngles[2] < 10) display.print(" ");
  display.print(servoAngles[2]);
  display.print((char)247);

  display.setCursor(64, 40);
  display.print("S3:");
  if (servoAngles[3] < 100) display.print(" ");
  if (servoAngles[3] < 10) display.print(" ");
  display.print(servoAngles[3]);
  display.print((char)247);

  // 第6行：分隔线
  display.setCursor(0, 50);
  display.println("--------------------");

  // 第7行：系统状态
  display.setCursor(0, 58);
  display.print(">");
  display.println(systemStatus);

  display.display();
}

// 双语日志系统：OLED屏幕显示状态，WebSocket输出中文
void dualLog(String serialMsg, String webMsg) {
  // 1. 串口输出（调试用，发给电脑）
  Serial0.println(serialMsg);

  // 2. 更新系统状态并刷新OLED显示
  systemStatus = serialMsg;
  if (systemStatus.length() > 18) {
    systemStatus = systemStatus.substring(0, 18);  // 限制长度
  }
  updateOLEDDisplay();

  // 3. Web界面日志缓冲区（中文）
  logBuffer += webMsg + "\n";
  if (logBuffer.length() > MAX_LOG_SIZE) {
    logBuffer = logBuffer.substring(logBuffer.length() - MAX_LOG_SIZE);
  }

  // 4. 通过WebSocket广播（中文）
  webSocket.broadcastTXT(webMsg);
}

// 兼容旧版：仅输出到Web（中文）
void serialPrintln(String msg) {
  // 串口输出
  Serial0.println("[LOG]");

  // 更新系统状态并刷新OLED显示
  systemStatus = "[LOG]";
  updateOLEDDisplay();

  // 添加到日志缓冲区
  logBuffer += msg + "\n";
  if (logBuffer.length() > MAX_LOG_SIZE) {
    logBuffer = logBuffer.substring(logBuffer.length() - MAX_LOG_SIZE);
  }

  // 通过WebSocket广播
  webSocket.broadcastTXT(msg);
}

void serialPrint(String msg) {
  Serial0.print("[LOG] ");

  // 添加到日志缓冲区
  logBuffer += msg;
  if (logBuffer.length() > MAX_LOG_SIZE) {
    logBuffer = logBuffer.substring(logBuffer.length() - MAX_LOG_SIZE);
  }

  // 通过WebSocket广播
  webSocket.broadcastTXT(msg);
}

// 初始化舵机系统
void initServoSystem() {
  // 初始化I2C总线0 (Wire) - PCA9685舵机驱动
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial0.println("I2C Bus 0 (PCA9685) init OK");

  // 初始化I2C总线1 (Wire1) - OLED屏幕（独立总线）
  Wire1.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  Serial0.println("I2C Bus 1 (OLED) init OK");

  // 初始化OLED屏幕
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial0.println("ERROR: SSD1306 init failed!");
  } else {
    Serial0.println("OLED screen init OK");
    systemStatus = "Initializing...";
    updateOLEDDisplay();
  }

  // 初始化PCA9685
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);  // 舵机工作频率为50Hz
  delay(100);
  Serial0.println("PCA9685 init OK");

  // 设置舵机初始位置（中间位置135度）
  systemStatus = "Setting servos...";
  updateOLEDDisplay();

  for (int i = 0; i < NUM_SERVOS; i++) {
    setServoAngle(i, DEFAULT_ANGLE);
    delay(50);  // 稍微延迟，避免舵机同时启动
  }
  Serial0.println("Servo init OK: 135deg");

  systemStatus = "Servo Ready";
  updateOLEDDisplay();
}

// 设置舵机角度（0-270度）
void setServoAngle(uint8_t channel, int angle) {
  // 限制角度范围
  if (angle < MIN_ANGLE) angle = MIN_ANGLE;
  if (angle > MAX_ANGLE) angle = MAX_ANGLE;

  // 将角度转换为脉冲宽度
  int pulse = map(angle, MIN_ANGLE, MAX_ANGLE, SERVOMIN, SERVOMAX);

  // 设置PWM
  pwm.setPWM(channel, 0, pulse);

  // 更新角度记录
  servoAngles[channel] = angle;

  // 刷新OLED显示
  updateOLEDDisplay();
}

// 移动舵机
void moveServo(int servoNum, int direction) {
  if (servoNum < 0 || servoNum >= NUM_SERVOS) return;

  int newAngle = servoAngles[servoNum] + (direction * STEP_ANGLE);

  // 限制范围
  if (newAngle < MIN_ANGLE) newAngle = MIN_ANGLE;
  if (newAngle > MAX_ANGLE) newAngle = MAX_ANGLE;

  setServoAngle(servoNum, newAngle);

  String serialMsg = "Servo" + String(servoNum) + " " +
                     (direction > 0 ? "CW" : "CCW") +
                     " -> " + String(newAngle) + "deg";
  String webMsg = "舵机" + String(servoNum) + " " +
                  (direction > 0 ? "正转" : "反转") +
                  " -> " + String(newAngle) + "度";
  dualLog(serialMsg, webMsg);
}

// 获取舵机当前角度
int getServoAngle(int servoNum) {
  if (servoNum < 0 || servoNum >= NUM_SERVOS) return -1;
  return servoAngles[servoNum];
}

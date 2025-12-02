// 舵机控制模块
#include "global.h"

// 自定义Serial打印函数，同时发送到WebSocket
void serialPrintln(String msg) {
  Serial0.println(msg);

  // 添加到日志缓冲区
  logBuffer += msg + "\n";
  if (logBuffer.length() > MAX_LOG_SIZE) {
    logBuffer = logBuffer.substring(logBuffer.length() - MAX_LOG_SIZE);
  }

  // 通过WebSocket广播
  webSocket.broadcastTXT(msg);
}

void serialPrint(String msg) {
  Serial0.print(msg);

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
  // 初始化I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial0.println("I2C初始化完成");

  // 初始化PCA9685
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);  // 舵机工作频率为50Hz
  delay(100);
  Serial0.println("PCA9685初始化完成");

  // 设置舵机初始位置（中间位置135度）
  for (int i = 0; i < NUM_SERVOS; i++) {
    setServoAngle(i, DEFAULT_ANGLE);
  }
  Serial0.println("舵机初始化完成，位置：135度");
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
}

// 移动舵机
void moveServo(int servoNum, int direction) {
  if (servoNum < 0 || servoNum >= NUM_SERVOS) return;

  int newAngle = servoAngles[servoNum] + (direction * STEP_ANGLE);

  // 限制范围
  if (newAngle < MIN_ANGLE) newAngle = MIN_ANGLE;
  if (newAngle > MAX_ANGLE) newAngle = MAX_ANGLE;

  setServoAngle(servoNum, newAngle);

  String msg = "舵机" + String(servoNum) + " " +
               (direction > 0 ? "正转" : "反转") +
               " -> " + String(newAngle) + "度";
  serialPrintln(msg);
}

// 获取舵机当前角度
int getServoAngle(int servoNum) {
  if (servoNum < 0 || servoNum >= NUM_SERVOS) return -1;
  return servoAngles[servoNum];
}

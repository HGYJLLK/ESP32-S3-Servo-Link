// servo_control.ino - 舵机控制模块 (全部180度SG90舵机)

// 辅助函数：获取小力舵机在数组中的索引
// 通道0-3 -> 索引0-3, 通道8-11 -> 索引4-7
int getSmallServoIndex(int channel) {
  if (channel >= 0 && channel <= 3) {
    return channel;  // 通道0-3 -> 索引0-3
  } else if (channel >= 8 && channel <= 11) {
    return channel - 4;  // 通道8-11 -> 索引4-7
  }
  return -1;  // 无效通道
}

// 判断是否为章鱼抓舵机 (通道9-11)
bool isSmallServo(int channel) {
  return (channel >= 9 && channel <= 11);
}

// 判断是否为大力舵机
bool isBigServo(int channel) {
  return (channel >= 4 && channel <= 7);
}

// 初始化舵机系统
void initServoSystem() {
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);
  delay(10);

  // 初始化小力舵机的左右值 (8个小力舵机)
  for (int i = 0; i < 8; i++) {
    smallServoLeftValues[i] = DEFAULT_LEFT_VALUE;    // 默认左值0度(松)
    smallServoRightValues[i] = DEFAULT_RIGHT_VALUE;  // 默认右值180度(紧)
  }

  // 设置章鱼抓到中间位置 (135度，270度舵机中点，通道9-11)
  for (int ch = 9; ch <= 11; ch++) {
    setServoAngle270(ch, DEFAULT_CENTER_VALUE);
    currentAngles[ch] = DEFAULT_CENTER_VALUE;
  }

  // 设置大力舵机到自定义初始位置
  setServoAngle(4, DEFAULT_ANGLE_SERVO_4);
  currentAngles[4] = DEFAULT_ANGLE_SERVO_4;

  setServoAngle(5, DEFAULT_ANGLE_SERVO_5);
  currentAngles[5] = DEFAULT_ANGLE_SERVO_5;

  setServoAngle(6, DEFAULT_ANGLE_SERVO_6);
  currentAngles[6] = DEFAULT_ANGLE_SERVO_6;

  setServoAngle(7, DEFAULT_ANGLE_SERVO_7);
  currentAngles[7] = DEFAULT_ANGLE_SERVO_7;

  Serial.println("Servo system initialized - 12 servos (SG90 180-degree)");
  addLog("舵机系统初始化完成 - 12个SG90舵机");
}

// 设置舵机角度 (0-180度，用于大力舵机通道4-7)
void setServoAngle(int channel, int angle) {
  if (channel < 0 || channel >= TOTAL_SERVOS) {
    Serial.println("Invalid servo channel");
    return;
  }

  angle = constrain(angle, 0, 180);
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(channel, 0, pulse);
  currentAngles[channel] = angle;
}

// 设置舵机角度 (0-270度，用于小力舵机通道0-3, 8-11)
void setServoAngle270(int channel, int angle) {
  if (channel < 0 || channel >= TOTAL_SERVOS) {
    Serial.println("Invalid servo channel");
    return;
  }

  angle = constrain(angle, 0, 270);
  int pulse = map(angle, 0, 270, SERVOMIN, SERVOMAX);
  pwm.setPWM(channel, 0, pulse);
  currentAngles[channel] = angle;
}

// 移动大力舵机 (增量方式)
void moveBigServo(int channel, int direction) {
  if (!isBigServo(channel)) {
    Serial.println("Invalid big servo channel");
    return;
  }

  int newAngle = currentAngles[channel] + (direction * STEP_ANGLE);
  newAngle = constrain(newAngle, 0, 180);

  setServoAngle(channel, newAngle);

  String logMsg = "大力舵机" + String(channel) + ": " + String(newAngle) + "°";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 直接设置大力舵机到指定角度
void setBigServoAngle(int channel, int angle) {
  if (!isBigServo(channel)) {
    Serial.println("Invalid big servo channel");
    return;
  }

  angle = constrain(angle, 0, 180);
  setServoAngle(channel, angle);

  String logMsg = "大力舵机" + String(channel) + "设置到: " + String(angle) + "°";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 小力舵机：设置左值(松)
void setSmallServoLeft(int channel, int leftValue) {
  int idx = getSmallServoIndex(channel);
  if (idx == -1) {
    Serial.println("Invalid small servo channel");
    return;
  }

  leftValue = constrain(leftValue, 0, 270);
  smallServoLeftValues[idx] = leftValue;

  String logMsg = "章鱼抓" + String(channel) + "左值(松)设为: " + String(leftValue) + "°";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 小力舵机：设置右值(紧)
void setSmallServoRight(int channel, int rightValue) {
  int idx = getSmallServoIndex(channel);
  if (idx == -1) {
    Serial.println("Invalid small servo channel");
    return;
  }

  rightValue = constrain(rightValue, 0, 270);
  smallServoRightValues[idx] = rightValue;

  String logMsg = "章鱼抓" + String(channel) + "右值(紧)设为: " + String(rightValue) + "°";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 小力舵机：移动到左值(松)
void moveSmallServoToLeft(int channel) {
  int idx = getSmallServoIndex(channel);
  if (idx == -1) {
    Serial.println("Invalid small servo channel");
    return;
  }

  int targetAngle = smallServoLeftValues[idx];
  setServoAngle270(channel, targetAngle);

  String logMsg = "章鱼抓" + String(channel) + "松开: " + String(targetAngle) + "°";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 小力舵机：移动到右值(紧)
void moveSmallServoToRight(int channel) {
  int idx = getSmallServoIndex(channel);
  if (idx == -1) {
    Serial.println("Invalid small servo channel");
    return;
  }

  int targetAngle = smallServoRightValues[idx];
  setServoAngle270(channel, targetAngle);

  String logMsg = "章鱼抓" + String(channel) + "抓紧: " + String(targetAngle) + "°";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 小力舵机：移动到中间值(90度)
void moveSmallServoToCenter(int channel) {
  if (!isSmallServo(channel)) {
    Serial.println("Invalid small servo channel");
    return;
  }

  setServoAngle270(channel, DEFAULT_CENTER_VALUE);

  String logMsg = "章鱼抓" + String(channel) + "居中: 135°";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 获取当前状态JSON
String getServoStatusJSON() {
  String json = "{";

  // 所有舵机当前角度
  json += "\"currentAngles\":[";
  for (int i = 0; i < TOTAL_SERVOS; i++) {
    json += String(currentAngles[i]);
    if (i < TOTAL_SERVOS - 1) json += ",";
  }
  json += "],";

  // 小力舵机左值
  json += "\"leftValues\":[";
  for (int i = 0; i < 8; i++) {
    json += String(smallServoLeftValues[i]);
    if (i < 7) json += ",";
  }
  json += "],";

  // 小力舵机右值
  json += "\"rightValues\":[";
  for (int i = 0; i < 8; i++) {
    json += String(smallServoRightValues[i]);
    if (i < 7) json += ",";
  }
  json += "]";

  json += "}";
  return json;
}

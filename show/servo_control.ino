// servo_control.ino - 舵机控制模块

// 初始化舵机系统
void initServoSystem() {
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);
  delay(10);

  // 初始化360度舵机中点（仅存储，不发送PWM信号）
  servo360Centers[0] = SERVO_360_CENTER_0;
  servo360Centers[1] = SERVO_360_CENTER_1;
  servo360Centers[2] = SERVO_360_CENTER_2;
  servo360Centers[3] = SERVO_360_CENTER_3;

  // 设置所有180度舵机到初始位置（避免碰地）
  setServoAngle(4, DEFAULT_ANGLE_SERVO_4);  // 大力4: 60度
  currentAngles[4] = DEFAULT_ANGLE_SERVO_4;

  setServoAngle(5, DEFAULT_ANGLE_SERVO_5);  // 大力5: 145度
  currentAngles[5] = DEFAULT_ANGLE_SERVO_5;

  setServoAngle(6, DEFAULT_ANGLE_SERVO_6);  // 大力6: 125度
  currentAngles[6] = DEFAULT_ANGLE_SERVO_6;

  setServoAngle(7, DEFAULT_ANGLE_SERVO_7);  // 大力7: 90度
  currentAngles[7] = DEFAULT_ANGLE_SERVO_7;

  // ⚠️ 360度舵机初始化时不发送PWM信号，避免意外旋转
  // 需要用户在Web界面手动测试并设置中点后才会启用
  for (int i = 0; i < NUM_SERVOS_360; i++) {
    currentAngles[i] = servo360Centers[i];
    // 不调用 setServoAngle()，360度舵机保持断电状态
  }

  Serial.println("Servo system initialized");
  Serial.println("WARNING: 360-degree servos are DISABLED on startup");
  Serial.println("Please use web interface to test and set center points first");
  addLog("舵机系统初始化完成");
  addLog("⚠️ 360度舵机已禁用，请先在Web界面测试中点");
}

// 设置舵机角度 (0-180度)
void setServoAngle(int channel, int angle) {
  // 限制角度范围
  angle = constrain(angle, 0, 180);

  // 将角度转换为脉冲宽度
  // 0度 = SERVOMIN, 180度 = SERVOMAX
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);

  pwm.setPWM(channel, 0, pulse);
  currentAngles[channel] = angle;
}

// 移动180度舵机 (增量方式)
void moveServo180(int servoNum, int direction) {
  if (servoNum < NUM_SERVOS_360 || servoNum >= TOTAL_SERVOS) {
    Serial.println("Invalid 180-degree servo number");
    return;
  }

  int newAngle = currentAngles[servoNum] + (direction * STEP_ANGLE);
  newAngle = constrain(newAngle, 0, 180);

  setServoAngle(servoNum, newAngle);

  String logMsg = "180度舵机" + String(servoNum) + ": " + String(newAngle) + "°";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 360度舵机：设置中点
void setServo360Center(int servoNum, int centerAngle) {
  if (servoNum >= NUM_SERVOS_360) {
    Serial.println("Invalid 360-degree servo number");
    return;
  }

  centerAngle = constrain(centerAngle, 0, 180);
  servo360Centers[servoNum] = centerAngle;
  setServoAngle(servoNum, centerAngle);

  String logMsg = "360度舵机" + String(servoNum) + "中点设为: " + String(centerAngle) + "°";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 360度舵机：点动控制 (左转/右转一下然后回中点)
void pulseServo360(int servoNum, int direction) {
  if (servoNum >= NUM_SERVOS_360) {
    Serial.println("Invalid 360-degree servo number");
    return;
  }

  int center = servo360Centers[servoNum];
  int moveAngle = center + (direction * SERVO_360_MOVE_ANGLE);
  moveAngle = constrain(moveAngle, 0, 180);

  // 移动到目标角度
  setServoAngle(servoNum, moveAngle);
  delay(100); // 短暂延时让舵机转动

  // 返回中点 (停止)
  setServoAngle(servoNum, center);

  String dirStr = (direction > 0) ? "正转" : "反转";
  String logMsg = "360度舵机" + String(servoNum) + ": " + dirStr + " (中点=" + String(center) + "°)";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 360度舵机：连续旋转控制 (用于测试中点)
void rotateServo360(int servoNum, int angle) {
  if (servoNum >= NUM_SERVOS_360) {
    Serial.println("Invalid 360-degree servo number");
    return;
  }

  angle = constrain(angle, 0, 180);
  setServoAngle(servoNum, angle);

  String logMsg = "360度舵机" + String(servoNum) + "测试角度: " + String(angle) + "°";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 停止360度舵机
void stopServo360(int servoNum) {
  if (servoNum >= NUM_SERVOS_360) {
    return;
  }
  setServoAngle(servoNum, servo360Centers[servoNum]);
}

// 获取当前角度JSON
String getAnglesJSON() {
  String json = "{\"servo180\":[";
  for (int i = NUM_SERVOS_360; i < TOTAL_SERVOS; i++) {
    json += String(currentAngles[i]);
    if (i < TOTAL_SERVOS - 1) json += ",";
  }
  json += "],\"servo360\":[";
  for (int i = 0; i < NUM_SERVOS_360; i++) {
    json += String(currentAngles[i]);
    if (i < NUM_SERVOS_360 - 1) json += ",";
  }
  json += "],\"servo360Centers\":[";
  for (int i = 0; i < NUM_SERVOS_360; i++) {
    json += String(servo360Centers[i]);
    if (i < NUM_SERVOS_360 - 1) json += ",";
  }
  json += "]}";
  return json;
}

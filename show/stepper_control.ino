// stepper_control.ino - 步进电机控制模块

// 初始化步进电机系统
void initStepperSystem() {
  // 设置垂直步进电机 (4个并联)
  stepperVertical.setMaxSpeed(MAX_SPEED);
  stepperVertical.setAcceleration(ACCELERATION);
  stepperVertical.setSpeed(DEFAULT_SPEED);
  stepperVertical.setCurrentPosition(0);

  // 设置水平步进电机
  stepperHorizontal.setMaxSpeed(MAX_SPEED);
  stepperHorizontal.setAcceleration(ACCELERATION);
  stepperHorizontal.setSpeed(DEFAULT_SPEED);
  stepperHorizontal.setCurrentPosition(0);

  // 初始化使能引脚 (低电平使能)
  pinMode(STEPPER_VERTICAL_ENABLE, OUTPUT);
  pinMode(STEPPER_HORIZONTAL_ENABLE, OUTPUT);
  digitalWrite(STEPPER_VERTICAL_ENABLE, LOW);  // 使能
  digitalWrite(STEPPER_HORIZONTAL_ENABLE, LOW); // 使能

  verticalPosition = 0;
  horizontalPosition = 0;

  Serial.println("Stepper system initialized");
  addLog("步进电机系统初始化完成");
}

// 运行步进电机 (在loop中调用)
void runSteppers() {
  stepperVertical.run();
  stepperHorizontal.run();
}

// 移动垂直步进电机 (上下)
void moveVertical(long steps) {
  long targetPos = stepperVertical.currentPosition() + steps;
  stepperVertical.moveTo(targetPos);
  verticalPosition = targetPos;

  String direction = (steps > 0) ? "上升" : "下降";
  String logMsg = "垂直丝杆" + direction + ": " + String(abs(steps)) + "步";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 移动水平步进电机 (左右)
void moveHorizontal(long steps) {
  long targetPos = stepperHorizontal.currentPosition() + steps;
  stepperHorizontal.moveTo(targetPos);
  horizontalPosition = targetPos;

  String direction = (steps > 0) ? "右移" : "左移";
  String logMsg = "水平丝杆" + direction + ": " + String(abs(steps)) + "步";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 设置垂直步进电机绝对位置
void setVerticalPosition(long position) {
  stepperVertical.moveTo(position);
  verticalPosition = position;

  String logMsg = "垂直丝杆移动到: " + String(position) + "步";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 设置水平步进电机绝对位置
void setHorizontalPosition(long position) {
  stepperHorizontal.moveTo(position);
  horizontalPosition = position;

  String logMsg = "水平丝杆移动到: " + String(position) + "步";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 归零垂直步进电机
void homeVertical() {
  stepperVertical.setCurrentPosition(0);
  stepperVertical.moveTo(0);
  verticalPosition = 0;

  Serial.println("Vertical stepper homed");
  addLog("垂直丝杆已归零");
}

// 归零水平步进电机
void homeHorizontal() {
  stepperHorizontal.setCurrentPosition(0);
  stepperHorizontal.moveTo(0);
  horizontalPosition = 0;

  Serial.println("Horizontal stepper homed");
  addLog("水平丝杆已归零");
}

// 停止所有步进电机
void stopAllSteppers() {
  stepperVertical.stop();
  stepperHorizontal.stop();

  Serial.println("All steppers stopped");
  addLog("所有步进电机已停止");
}

// 使能/禁用步进电机
void enableSteppers(bool enable) {
  digitalWrite(STEPPER_VERTICAL_ENABLE, enable ? LOW : HIGH);
  digitalWrite(STEPPER_HORIZONTAL_ENABLE, enable ? LOW : HIGH);

  String logMsg = enable ? "步进电机已使能" : "步进电机已禁用";
  Serial.println(logMsg);
  addLog(logMsg);
}

// 获取步进电机状态JSON
String getStepperStatusJSON() {
  String json = "{";
  json += "\"vertical\":{\"pos\":" + String(stepperVertical.currentPosition()) +
          ",\"target\":" + String(stepperVertical.targetPosition()) +
          ",\"running\":" + String(stepperVertical.isRunning() ? "true" : "false") + "},";
  json += "\"horizontal\":{\"pos\":" + String(stepperHorizontal.currentPosition()) +
          ",\"target\":" + String(stepperHorizontal.targetPosition()) +
          ",\"running\":" + String(stepperHorizontal.isRunning() ? "true" : "false") + "}";
  json += "}";
  return json;
}

// 设置步进电机速度
void setStepperSpeed(int motor, int speed) {
  speed = constrain(speed, 100, MAX_SPEED);

  if (motor == 0) { // 垂直
    stepperVertical.setMaxSpeed(speed);
    String logMsg = "垂直丝杆速度: " + String(speed) + " 步/秒";
    addLog(logMsg);
  } else if (motor == 1) { // 水平
    stepperHorizontal.setMaxSpeed(speed);
    String logMsg = "水平丝杆速度: " + String(speed) + " 步/秒";
    addLog(logMsg);
  }
}

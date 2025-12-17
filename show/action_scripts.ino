// action_scripts.ino - 动作脚本

// 辅助函数：平滑移动舵机到目标角度
void smoothMoveServo(int channel, int targetAngle, int delayMs = 20) {
  int currentAngle = currentAngles[channel];
  int step = (targetAngle > currentAngle) ? 1 : -1;

  for (int angle = currentAngle; angle != targetAngle; angle += step) {
    setServoAngle(channel, angle);
    delay(delayMs);
  }
  setServoAngle(channel, targetAngle);
  currentAngles[channel] = targetAngle;
}

// 辅助函数：同时移动两个舵机到目标角度
void smoothMoveTwoServos(int ch1, int target1, int ch2, int target2, int delayMs = 20) {
  int current1 = currentAngles[ch1];
  int current2 = currentAngles[ch2];

  int diff1 = abs(target1 - current1);
  int diff2 = abs(target2 - current2);
  int maxSteps = max(diff1, diff2);

  for (int step = 0; step <= maxSteps; step++) {
    int angle1 = map(step, 0, maxSteps, current1, target1);
    int angle2 = map(step, 0, maxSteps, current2, target2);

    setServoAngle(ch1, angle1);
    setServoAngle(ch2, angle2);
    delay(delayMs);
  }

  currentAngles[ch1] = target1;
  currentAngles[ch2] = target2;
}

// 动作1: 垂直丝杆上10000步后再下10000步
void executeAction1() {
  addLog("动作1开始: 垂直丝杆上升");
  moveVertical(10000);

  // 等待运动完成
  while (stepperVertical.isRunning()) {
    stepperVertical.run();
    delay(1);
  }

  addLog("动作1: 垂直丝杆下降");
  moveVertical(-10000);

  // 等待运动完成
  while (stepperVertical.isRunning()) {
    stepperVertical.run();
    delay(1);
  }

  addLog("动作1完成");
}

// 动作2: 大力6摆动 (125->170->80->125)
void executeAction2() {
  addLog("动作2开始: 大力6从125度到170度");
  smoothMoveServo(6, 170, 15);
  delay(300);

  addLog("动作2: 大力6从170度到80度");
  smoothMoveServo(6, 80, 15);
  delay(300);

  addLog("动作2: 大力6从80度到125度");
  smoothMoveServo(6, 125, 15);
  delay(300);

  addLog("动作2完成");
}

// 动作3: 大力4+5联动
void executeAction3() {
  // 第一段: 4: 60->75, 5: 145->120
  addLog("动作3开始: 大力4(60->75) + 大力5(145->120)");
  smoothMoveTwoServos(4, 75, 5, 120, 15);
  delay(300);

  // 第二段: 4: 75->35, 5: 120->170
  addLog("动作3: 大力4(75->35) + 大力5(120->170)");
  smoothMoveTwoServos(4, 35, 5, 170, 15);
  delay(300);

  // 第三段: 4: 35->60, 5: 170->145
  addLog("动作3: 大力4(35->60) + 大力5(170->145)");
  smoothMoveTwoServos(4, 60, 5, 145, 15);
  delay(300);

  addLog("动作3完成");
}

// 动作4: 小力舵机左右摆动
void executeAction4() {
  int center = SERVO_360_ACTION_CENTER; // 120度中点

  addLog("动作4开始: 小力舵机中点=" + String(center) + "度");

  // 先设置所有360度舵机到中点
  for (int i = 0; i < NUM_SERVOS_360; i++) {
    setServoAngle(i, center);
  }
  delay(500);

  // 往左1秒 (中点-10度)
  addLog("动作4: 向左转动1秒");
  for (int i = 0; i < NUM_SERVOS_360; i++) {
    setServoAngle(i, center - 10);
  }
  delay(1000);

  // 往右2秒 (中点+10度)
  addLog("动作4: 向右转动2秒");
  for (int i = 0; i < NUM_SERVOS_360; i++) {
    setServoAngle(i, center + 10);
  }
  delay(2000);

  // 再往左1秒 (中点-10度)
  addLog("动作4: 向左转动1秒");
  for (int i = 0; i < NUM_SERVOS_360; i++) {
    setServoAngle(i, center - 10);
  }
  delay(1000);

  // 回到中点停止
  addLog("动作4: 回到中点停止");
  for (int i = 0; i < NUM_SERVOS_360; i++) {
    setServoAngle(i, center);
  }

  addLog("动作4完成");
}

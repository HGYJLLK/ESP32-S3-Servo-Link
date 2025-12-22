// action_scripts.ino - 动作脚本

// 辅助函数：平滑移动舵机到目标角度
// stepSize: 每次移动的角度（1=最慢最平滑, 5=快速, 10=很快）
// delayMs: 每步之间的延迟（ms）
void smoothMoveServo(int channel, int targetAngle, int stepSize = 5, int delayMs = 10) {
  int currentAngle = currentAngles[channel];

  if (currentAngle == targetAngle) return; // 已经在目标位置

  int direction = (targetAngle > currentAngle) ? 1 : -1;
  int actualStepSize = stepSize * direction;

  for (int angle = currentAngle;
       (direction > 0 && angle < targetAngle) || (direction < 0 && angle > targetAngle);
       angle += actualStepSize) {

    // 确保不会超过目标
    if ((direction > 0 && angle + actualStepSize > targetAngle) ||
        (direction < 0 && angle + actualStepSize < targetAngle)) {
      angle = targetAngle;
    }

    setServoAngle(channel, angle);
    delay(delayMs);
  }

  setServoAngle(channel, targetAngle);
  currentAngles[channel] = targetAngle;
}

// 辅助函数：同时移动两个舵机到目标角度
// stepSize: 步进大小（度），越大越快
// delayMs: 每步延迟（ms）
void smoothMoveTwoServos(int ch1, int target1, int ch2, int target2, int stepSize = 5, int delayMs = 10) {
  int current1 = currentAngles[ch1];
  int current2 = currentAngles[ch2];

  int diff1 = abs(target1 - current1);
  int diff2 = abs(target2 - current2);
  int maxSteps = max(diff1, diff2) / stepSize;

  if (maxSteps == 0) maxSteps = 1; // 至少移动一次

  for (int step = 0; step <= maxSteps; step++) {
    int angle1 = map(step, 0, maxSteps, current1, target1);
    int angle2 = map(step, 0, maxSteps, current2, target2);

    setServoAngle(ch1, angle1);
    setServoAngle(ch2, angle2);
    delay(delayMs);
  }

  setServoAngle(ch1, target1);
  setServoAngle(ch2, target2);
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
  smoothMoveServo(6, 170, 5, 10);  // 步进5度，延迟10ms
  delay(300);

  addLog("动作2: 大力6从170度到80度");
  smoothMoveServo(6, 80, 5, 10);
  delay(300);

  addLog("动作2: 大力6从80度到125度");
  smoothMoveServo(6, 125, 5, 10);
  delay(300);

  addLog("动作2完成");
}

// 动作3: 大力4+5联动
void executeAction3() {
  // 第一段: 4: 60->75, 5: 145->120
  addLog("动作3开始: 大力4(60->75) + 大力5(145->120)");
  smoothMoveTwoServos(4, 75, 5, 120, 5, 10);  // 步进5度，延迟10ms
  delay(300);

  // 第二段: 4: 75->35, 5: 120->170
  addLog("动作3: 大力4(75->35) + 大力5(120->170)");
  smoothMoveTwoServos(4, 35, 5, 170, 5, 10);
  delay(300);

  // 第三段: 4: 35->60, 5: 170->145
  addLog("动作3: 大力4(35->60) + 大力5(170->145)");
  smoothMoveTwoServos(4, 60, 5, 145, 5, 10);
  delay(300);

  addLog("动作3完成");
}

// 动作4: 小力舵机左右摆动 (8个小力舵机: 0-3, 8-11)
void executeAction4() {
  addLog("动作4开始: 小力舵机左右摆动");

  // 往左1秒 (松开)
  addLog("动作4: 向左(松开)1秒");
  for (int i = 0; i <= 3; i++) {
    moveSmallServoToLeft(i);
  }
  for (int i = 8; i <= 11; i++) {
    moveSmallServoToLeft(i);
  }
  delay(1000);

  // 往右2秒 (抓紧)
  addLog("动作4: 向右(抓紧)2秒");
  for (int i = 0; i <= 3; i++) {
    moveSmallServoToRight(i);
  }
  for (int i = 8; i <= 11; i++) {
    moveSmallServoToRight(i);
  }
  delay(2000);

  // 再往左1秒 (松开)
  addLog("动作4: 向左(松开)1秒");
  for (int i = 0; i <= 3; i++) {
    moveSmallServoToLeft(i);
  }
  for (int i = 8; i <= 11; i++) {
    moveSmallServoToLeft(i);
  }
  delay(1000);

  // 回到中点停止
  addLog("动作4: 回到中点(90度)停止");
  for (int i = 0; i <= 3; i++) {
    moveSmallServoToCenter(i);
  }
  for (int i = 8; i <= 11; i++) {
    moveSmallServoToCenter(i);
  }

  addLog("动作4完成");
}

// 动作5: 龙抓手 - 抓 (从90度初始位置到抓取姿态)
void executeAction5() {
  addLog("动作5开始: 龙抓手 - 抓");

  // 设置小力舵机到抓取姿态（快速移动：步进10度，延迟5ms）
  smoothMoveServo(0, 0, 10, 5);    // 小力0: 90 -> 0
  smoothMoveServo(1, 180, 10, 5);  // 小力1: 90 -> 180
  smoothMoveServo(2, 180, 10, 5);  // 小力2: 90 -> 180
  smoothMoveServo(3, 20, 10, 5);   // 小力3: 90 -> 20
  smoothMoveServo(8, 90, 10, 5);   // 小力8: 保持90
  smoothMoveServo(9, 20, 10, 5);   // 小力9: 90 -> 20
  smoothMoveServo(10, 90, 10, 5);  // 小力10: 保持90
  smoothMoveServo(11, 20, 10, 5);  // 小力11: 90 -> 20

  delay(300);
  addLog("动作5完成: 龙抓手已抓紧");
}

// 动作6: 龙抓手 - 张 (从90度初始位置到张开姿态)
void executeAction6() {
  addLog("动作6开始: 龙抓手 - 张");

  // 设置小力舵机到张开姿态（快速移动：步进10度，延迟5ms）
  smoothMoveServo(0, 0, 10, 5);    // 小力0: 90 -> 0
  smoothMoveServo(1, 90, 10, 5);   // 小力1: 保持90
  smoothMoveServo(2, 10, 10, 5);   // 小力2: 90 -> 10
  smoothMoveServo(3, 20, 10, 5);   // 小力3: 90 -> 20
  smoothMoveServo(8, 120, 10, 5);  // 小力8: 90 -> 120
  smoothMoveServo(9, 20, 10, 5);   // 小力9: 90 -> 20
  smoothMoveServo(10, 180, 10, 5); // 小力10: 90 -> 180
  smoothMoveServo(11, 20, 10, 5);  // 小力11: 90 -> 20

  delay(300);
  addLog("动作6完成: 龙抓手已张开");
}

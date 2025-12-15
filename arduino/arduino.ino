/*
 * Arduino 六轴机器人舵机控制程序
 * 功能：串口控制
 * 舵机类型：180度标准舵机（非270度连续旋转）
 * 硬件：Arduino Uno/Nano/Mega + 6个180度舵机
 */

#include <Servo.h>

// ========== 舵机配置 ==========
Servo myServos[6];

// 物理引脚映射（Sensor Shield V5.0 数字口）
// 注意：引脚0和1是串口，不能用于舵机！
// 建议使用引脚 2-7 或 3-8

// 如果您的舵机插在 Sensor Shield 的 0-5 口：
// 请重新插到 2-7 口（或 3-8 口），因为 0 和 1 是串口引脚
//
// 当前配置：舵机插在 Sensor Shield 的 2-7 数字口
int servoPins[] = {2, 3, 4, 5, 6, 7};  // 对应 Sensor Shield D2-D7

// 舵机逻辑编号映射（如果接线顺序不对，修改这里）
// 默认：逻辑舵机0对应物理引脚2，逻辑舵机5对应物理引脚7
int servoMapping[] = {0, 1, 2, 3, 4, 5};  // 逻辑编号到物理编号的映射

int currentAngles[] = {90, 90, 90, 90, 90, 90};  // 初始角度都是90度（中间位置）

// ========== 按钮配置 ==========
#define BUTTON_PIN A0  // 按钮连接到A0引脚（也可以用A1-A5）
int lastButtonState = HIGH;  // 上一次按钮状态（HIGH=未按下，LOW=按下）
int buttonState = HIGH;      // 当前按钮状态
unsigned long lastDebounceTime = 0;  // 上次按钮状态改变的时间
unsigned long debounceDelay = 50;    // 防抖延迟（毫秒）

// ========== 每个舵机的安全角度范围 ==========
// 格式：{最小角度, 最大角度}
int servoLimits[6][2] = {
  {0, 180},   // 舵机0: 无限制 (0-180度)
  {0, 180},   // 舵机1: 无限制 (0-180度)
  {0, 180},   // 舵机2: 无限制 (0-180度)
  {0, 180},   // 舵机3: 无限制 (0-180度)
  {0, 180},   // 舵机4: 无限制 (0-180度)
  {0, 180}    // 舵机5: 无限制 (0-180度)
};

// ========== 舵机控制函数（带安全限制）==========
void setServoAngle(int logicalIndex, int angle) {
  angle = constrain(angle, 0, 180);
  int minAngle = servoLimits[logicalIndex][0];
  int maxAngle = servoLimits[logicalIndex][1];
  angle = constrain(angle, minAngle, maxAngle);

  int physicalIndex = servoMapping[logicalIndex];
  myServos[physicalIndex].write(angle);
  currentAngles[logicalIndex] = angle;

  Serial.print(F("S"));
  Serial.print(logicalIndex);
  Serial.print(F("->"));
  Serial.print(angle);
  Serial.print(F("° ["));
  Serial.print(minAngle);
  Serial.print(F("-"));
  Serial.print(maxAngle);
  Serial.println(F("°]"));
}

// ========== 显示当前状态 ==========
void printCurrentAngles() {
  Serial.println(F("\n[Status]"));
  for (int i = 0; i < 6; i++) {
    Serial.print(F("S"));
    Serial.print(i);
    Serial.print(F(":"));
    Serial.print(currentAngles[i]);
    Serial.println(F("°"));
  }
}

// ========== 串口命令处理 ==========
void handleSerialCommands() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    // 命令格式:
    // "0 90" (舵机索引 角度)
    // "all 90" (所有舵机同一角度)
    // "status" (显示当前角度)
    // "reset" (复位到90度)

    if (command == "status") {
      printCurrentAngles();
    }
    else if (command == "map") {
      Serial.println(F("\n[Pin Map]"));
      for (int i = 0; i < 6; i++) {
        Serial.print(F("S"));
        Serial.print(i);
        Serial.print(F("->P"));
        Serial.println(servoPins[servoMapping[i]]);
      }
    }
    else if (command == "reset") {
      Serial.println(F("Resetting..."));
      for (int i = 0; i < 6; i++) {
        setServoAngle(i, 90);
        delay(50);
      }
      Serial.println(F("Done!"));
    }
    else if (command == "test") {
      // 输入 test 就执行安全范围测试
      safeRangeTest();
    }
    else if (command == "grab") {
      // 输入 grab 执行抓取动作
      grabAction();
    }
    else if (command == "sweep") {
      sweepTest();
    }
    else if (command == "demo") {
      demoSequence();
    }
    else if (command == "limits") {
      Serial.println(F("\n[Limits]"));
      Serial.println(F("S0:65-180 S1:65-180 S2:90"));
      Serial.println(F("S3:90-150 S4:30-90 S5:0-90"));
    }
    else if (command == "help" || command == "?") {
      printHelp();
    }
    else if (command.startsWith("test ")) {
      int servoIndex = command.substring(5).toInt();
      if (servoIndex >= 0 && servoIndex < 6) {
        testServo(servoIndex);
      } else {
        Serial.println("错误：舵机索引必须是 0-5");
      }
    }
    else if (command.startsWith("all ")) {
      int angle = command.substring(4).toInt();
      Serial.print(F("All->"));
      Serial.println(angle);
      for (int i = 0; i < 6; i++) {
        setServoAngle(i, angle);
        delay(50);
      }
    }
    else {
      int spaceIndex = command.indexOf(' ');
      if (spaceIndex > 0) {
        int servoIndex = command.substring(0, spaceIndex).toInt();
        int angle = command.substring(spaceIndex + 1).toInt();

        if (servoIndex >= 0 && servoIndex < 6) {
          setServoAngle(servoIndex, angle);
        } else {
          Serial.println(F("ERR:idx 0-5"));
        }
      } else if (command.length() > 0) {
        Serial.println(F("ERR:type 'help'"));
      }
    }
  }
}

// ========== 舵机测试函数（诊断用）==========
void testServo(int logicalIndex) {
  int physicalIndex = servoMapping[logicalIndex];
  Serial.print(F("\nTest S"));
  Serial.println(logicalIndex);

  myServos[physicalIndex].write(0);
  Serial.println(F("->0"));
  delay(1000);

  myServos[physicalIndex].write(90);
  Serial.println(F("->90"));
  delay(1000);

  myServos[physicalIndex].write(180);
  Serial.println(F("->180"));
  delay(1000);

  myServos[physicalIndex].write(90);
  Serial.println(F("->90"));
  currentAngles[logicalIndex] = 90;
}

// ========== 扫描测试所有舵机 ==========
void sweepTest() {
  Serial.println(F("\n[Sweep Test]"));
  for (int i = 0; i < 6; i++) {
    testServo(i);
    delay(500);
  }
  Serial.println(F("Done!"));
}

// ========== 平滑移动函数 ==========
void smoothMove(int id, int target, int spd) {
  int cur = currentAngles[id];
  int minA = servoLimits[id][0];
  int maxA = servoLimits[id][1];
  target = constrain(target, minA, maxA);

  int p = servoMapping[id];

  if (cur < target) {
    for (int i = cur; i <= target; i++) {
      myServos[p].write(i);
      delay(spd);
    }
  } else {
    for (int i = cur; i >= target; i--) {
      myServos[p].write(i);
      delay(spd);
    }
  }
  currentAngles[id] = target;
}

// ========== 机械臂舞蹈测试 ==========
void safeRangeTest() {
  Serial.println(F("\n[Wave Test]"));

  // Init
  smoothMove(1, 0, 8);    // 夹子移到0度
  smoothMove(5, 120, 8);  // 底座移到120度起始位置
  delay(500);

  // Wave Hello (招手)
  Serial.println(F("Wave"));
  for(int i=0; i<3; i++) {
    smoothMove(5, 75, 6);   // 底座移到75度
    delay(150);
    smoothMove(5, 120, 6);  // 底座移回120度
    delay(150);
  }

  delay(300);
  Serial.println(F("Done!"));
}

// ========== 抓取动作 ==========
void grabAction() {
  Serial.println(F("\n[Grab Action]"));

  // 1. 初始位置
  smoothMove(2, 180, 8);
  smoothMove(5, 90, 8);
  smoothMove(4, 90, 8);
  delay(300);

  // 2. 舵机3在120-45度来回两次
  for(int i=0; i<2; i++) {
    smoothMove(3, 120, 6);
    delay(100);
    smoothMove(3, 45, 6);
    delay(100);
  }

  // 3. 舵机3回到90度
  smoothMove(3, 90, 6);
  delay(300);

  // 4. 移动舵机5和4
  smoothMove(5, 160, 8);
  smoothMove(4, 45, 8);
  delay(300);

  // 5. 舵机2到60度
  smoothMove(2, 60, 6);
  delay(300);

  // 6. 舵机5回90度
  smoothMove(5, 90, 8);
  delay(300);

  // 7. 舵机5到160度
  smoothMove(5, 160, 8);
  delay(300);

  // 8. 舵机2到180度
  smoothMove(2, 180, 6);
  delay(300);

  // 9. 舵机5和4回中位
  smoothMove(5, 90, 8);
  smoothMove(4, 90, 8);
  delay(300);

  // 10. 舵机3在120-45度来回两次
  for(int i=0; i<2; i++) {
    smoothMove(3, 120, 6);
    delay(100);
    smoothMove(3, 45, 6);
    delay(100);
  }

  delay(300);
  Serial.println(F("Done!"));
}

// ========== 按钮检测函数 ==========
void checkButton() {
  int reading = digitalRead(BUTTON_PIN);

  // 如果按钮状态改变，重置防抖计时器
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // 如果状态稳定超过防抖延迟时间
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // 如果按钮状态确实改变了
    if (reading != buttonState) {
      buttonState = reading;

      // 如果按钮被按下（从HIGH变为LOW）
      if (buttonState == LOW) {
        Serial.println(F("\n[Button Pressed]"));
        grabAction();  // 执行抓取动作
      }
    }
  }

  lastButtonState = reading;
}

// ========== 演示动作序列 ==========
void demoSequence() {
  Serial.println(F("\n[Demo]"));

  Serial.println(F("1.Reset"));
  setServoAngle(5, 45);
  delay(500);
  setServoAngle(4, 60);
  delay(500);
  setServoAngle(3, 120);
  delay(500);
  setServoAngle(2, 90);
  delay(500);
  setServoAngle(1, 122);
  delay(1000);

  Serial.println(F("2.Base"));
  for (int a = 0; a <= 90; a += 5) {
    setServoAngle(5, a);
    delay(50);
  }
  delay(1000);

  Serial.println(F("3.Arm"));
  for (int a = 30; a <= 90; a += 5) {
    setServoAngle(4, a);
    delay(50);
  }
  delay(1000);

  Serial.println(F("4.Forearm"));
  for (int a = 90; a <= 150; a += 5) {
    setServoAngle(3, a);
    delay(50);
  }
  delay(1000);

  Serial.println(F("5.Close"));
  for (int a = 180; a >= 65; a -= 5) {
    setServoAngle(1, a);
    delay(30);
  }
  delay(1000);

  Serial.println(F("6.Open"));
  for (int a = 65; a <= 180; a += 5) {
    setServoAngle(1, a);
    delay(30);
  }
  delay(1000);

  Serial.println(F("7.Return"));
  setServoAngle(5, 45);
  delay(300);
  setServoAngle(4, 60);
  delay(300);
  setServoAngle(3, 120);
  delay(300);
  setServoAngle(1, 122);
  delay(500);

  Serial.println(F("Done!"));
}

// ========== 显示帮助信息 ==========
void printHelp() {
  Serial.println(F("\n[Commands]"));
  Serial.println(F("0 90   -Set S0 to 90deg"));
  Serial.println(F("all 90 -All to 90deg"));
  Serial.println(F("reset  -Reset all"));
  Serial.println(F("status -Show angles"));
  Serial.println(F("test   -Wave action"));
  Serial.println(F("grab   -Grab action"));
  Serial.println(F("test 3 -Test S3"));
  Serial.println(F("sweep  -Test all"));
  Serial.println(F("demo   -Demo sequence"));
  Serial.println(F("map    -Pin mapping"));
  Serial.println(F("limits -Angle limits"));
}

// ========== Arduino Setup ==========
void setup() {
  Serial.begin(115200);
  Serial.println(F("\n\n=== Robot Init ==="));

  // 初始化按钮引脚
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // 使用内部上拉电阻
  Serial.print(F("Button on Pin "));
  Serial.println(BUTTON_PIN);

  for (int i = 0; i < 6; i++) {
    myServos[i].attach(servoPins[i], 500, 2500);
    myServos[i].write(90);
    Serial.print(F("P"));
    Serial.print(servoPins[i]);
    Serial.println(F("->90"));
    delay(300);
  }

  Serial.println(F("\n=== Ready! ==="));
  printHelp();
}

// ========== Arduino Loop ==========
void loop() {
  handleSerialCommands();
  checkButton();  // 检测按钮
  delay(10);
}
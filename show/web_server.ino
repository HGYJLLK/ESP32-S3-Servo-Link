// web_server.ino - Web服务器和WebSocket处理

// 添加日志到缓冲区
void addLog(String message) {
  if (logBuffer.length() > MAX_LOG_SIZE) {
    logBuffer = logBuffer.substring(logBuffer.length() - MAX_LOG_SIZE / 2);
  }
  logBuffer += message + "\n";

  // 通过WebSocket广播
  webSocket.broadcastTXT(message);
}

// WebSocket事件处理
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_CONNECTED) {
    Serial.printf("WebSocket client #%u connected\n", num);
    webSocket.sendTXT(num, logBuffer);
  } else if (type == WStype_DISCONNECTED) {
    Serial.printf("WebSocket client #%u disconnected\n", num);
  }
}

// 初始化WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    addLog("WiFi已连接: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi connection failed!");
    addLog("WiFi连接失败");
  }
}

// 初始化Web服务器
void initWebServer() {
  // 主页
  server.on("/", []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });

  // 360度舵机 - 设置中点
  server.on("/servo360/setcenter", []() {
    if (server.hasArg("servo") && server.hasArg("center")) {
      int servo = server.arg("servo").toInt();
      int center = server.arg("center").toInt();
      setServo360Center(servo, center);
      server.send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"missing parameters\"}");
    }
  });

  // 360度舵机 - 点动
  server.on("/servo360/pulse", []() {
    if (server.hasArg("servo") && server.hasArg("dir")) {
      int servo = server.arg("servo").toInt();
      int dir = server.arg("dir").toInt();
      pulseServo360(servo, dir);
      server.send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"missing parameters\"}");
    }
  });

  // 360度舵机 - 测试旋转
  server.on("/servo360/test", []() {
    if (server.hasArg("servo") && server.hasArg("angle")) {
      int servo = server.arg("servo").toInt();
      int angle = server.arg("angle").toInt();
      rotateServo360(servo, angle);
      server.send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"missing parameters\"}");
    }
  });

  // 360度舵机 - 停止所有
  server.on("/servo360/stopall", []() {
    for (int i = 0; i < NUM_SERVOS_360; i++) {
      stopServo360(i);
    }
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // 180度舵机 - 移动
  server.on("/servo180/move", []() {
    if (server.hasArg("servo") && server.hasArg("dir")) {
      int servo = server.arg("servo").toInt();
      int dir = server.arg("dir").toInt();
      moveServo180(servo, dir);

      String json = "{\"servo\":" + String(servo) + ",\"angle\":" + String(currentAngles[servo]) + "}";
      server.send(200, "application/json", json);
    } else {
      server.send(400, "application/json", "{\"error\":\"missing parameters\"}");
    }
  });

  // 步进电机 - 垂直移动
  server.on("/stepper/vertical", []() {
    if (server.hasArg("steps")) {
      long steps = server.arg("steps").toInt();
      moveVertical(steps);
      server.send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"missing parameters\"}");
    }
  });

  // 步进电机 - 水平移动
  server.on("/stepper/horizontal", []() {
    if (server.hasArg("steps")) {
      long steps = server.arg("steps").toInt();
      moveHorizontal(steps);
      server.send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"missing parameters\"}");
    }
  });

  // 步进电机 - 归零
  server.on("/stepper/home", []() {
    if (server.hasArg("motor")) {
      int motor = server.arg("motor").toInt();
      if (motor == 0) {
        homeVertical();
      } else if (motor == 1) {
        homeHorizontal();
      }
      server.send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"missing parameters\"}");
    }
  });

  // 步进电机 - 停止
  server.on("/stepper/stop", []() {
    stopAllSteppers();
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // 获取状态
  server.on("/status", []() {
    String json = "{";
    json += "\"vertical\":" + String(stepperVertical.currentPosition()) + ",";
    json += "\"horizontal\":" + String(stepperHorizontal.currentPosition()) + ",";
    json += "\"servo180\":[";
    for (int i = NUM_SERVOS_360; i < TOTAL_SERVOS; i++) {
      json += String(currentAngles[i]);
      if (i < TOTAL_SERVOS - 1) json += ",";
    }
    json += "],\"servo360Centers\":[";
    for (int i = 0; i < NUM_SERVOS_360; i++) {
      json += String(servo360Centers[i]);
      if (i < NUM_SERVOS_360 - 1) json += ",";
    }
    json += "]}";
    server.send(200, "application/json", json);
  });

  // 动作脚本 - 动作1: 垂直丝杆上下运动
  server.on("/action/action1", []() {
    addLog("执行动作1: 垂直丝杆上下运动");
    executeAction1();
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // 动作脚本 - 动作2: 大力6摆动
  server.on("/action/action2", []() {
    addLog("执行动作2: 大力6摆动");
    executeAction2();
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // 动作脚本 - 动作3: 大力4+5联动
  server.on("/action/action3", []() {
    addLog("执行动作3: 大力4+5联动");
    executeAction3();
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // 动作脚本 - 动作4: 小力舵机左右摆动
  server.on("/action/action4", []() {
    addLog("执行动作4: 小力舵机左右摆动");
    executeAction4();
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  server.begin();
  Serial.println("HTTP server started");

  // 启动WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started");

  addLog("Web服务器已启动");
}

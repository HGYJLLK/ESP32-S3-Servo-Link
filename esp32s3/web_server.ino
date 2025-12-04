// Web服务器和WebSocket模块
#include "global.h"
#include "web_interface.h"

// 处理网页请求
void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

// 处理舵机移动请求
void handleMove() {
  if (server.hasArg("servo") && server.hasArg("dir")) {
    int servoNum = server.arg("servo").toInt();
    int direction = server.arg("dir").toInt();

    moveServo(servoNum, direction);

    String json = "{\"servo\":" + String(servoNum) +
                  ",\"angle\":" + String(servoAngles[servoNum]) + "}";
    server.send(200, "application/json", json);
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

// 获取所有舵机角度（16个通道）
void handleAngles() {
  String json = "{\"angles\":[";
  for (int i = 0; i < NUM_SERVOS; i++) {
    json += String(servoAngles[i]);
    if (i < NUM_SERVOS - 1) json += ",";
  }
  json += "]}";
  server.send(200, "application/json", json);
}

// 处理设定角度请求
void handleSet() {
  if (server.hasArg("servo") && server.hasArg("angle")) {
    int servoNum = server.arg("servo").toInt();
    int angle = server.arg("angle").toInt();

    if (servoNum >= 0 && servoNum < NUM_SERVOS && angle >= MIN_ANGLE && angle <= MAX_ANGLE) {
      setServoAngle(servoNum, angle);

      String serialMsg = "Servo" + String(servoNum) + " set to " + String(angle) + "deg";
      String webMsg = "舵机" + String(servoNum) + " 设定到 " + String(angle) + "度";
      dualLog(serialMsg, webMsg);

      String json = "{\"servo\":" + String(servoNum) +
                    ",\"angle\":" + String(servoAngles[servoNum]) + "}";
      server.send(200, "application/json", json);
    } else {
      server.send(400, "text/plain", "Invalid parameters");
    }
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

// WebSocket事件处理
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial0.printf("[%u] WS disconnected\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial0.printf("[%u] WS from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        // 发送历史日志
        webSocket.sendTXT(num, logBuffer);
      }
      break;
  }
}

// 初始化WiFi连接
void initWiFi() {
  Serial0.print("Connecting WiFi: ");
  Serial0.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial0.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial0.println("\nWiFi OK!");
    Serial0.print("IP: ");
    Serial0.println(WiFi.localIP());

    // 更新全局IP变量
    systemIP = WiFi.localIP().toString();

    dualLog("WiFi connected",
            "WiFi已连接，IP: " + systemIP);
  } else {
    Serial0.println("\nWiFi FAILED!");
    systemIP = "Failed";
    dualLog("WiFi failed!",
            "WiFi连接失败，请检查SSID和密码");
  }
}

// 初始化Web服务器
void initWebServer() {
  // 配置Web服务器路由
  server.on("/", handleRoot);
  server.on("/move", handleMove);
  server.on("/angles", handleAngles);
  server.on("/set", handleSet);
  server.begin();
  Serial0.println("HTTP server started");

  // 启动WebSocket服务器
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial0.println("WebSocket started");

  Serial0.println("\n=== SYSTEM READY ===");
  Serial0.print("Visit: http://");
  Serial0.println(WiFi.localIP());

  dualLog("Ready",
          "=== 系统初始化完成 ===");
  // Web日志显示完整URL
  logBuffer += "请在浏览器中打开: http://" + systemIP + "\n";
  webSocket.broadcastTXT("请在浏览器中打开: http://" + systemIP);
}

// Web服务器循环处理
void handleWebServer() {
  server.handleClient();
  webSocket.loop();
}

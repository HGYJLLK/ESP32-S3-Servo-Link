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

// WiFiManager配置门户回调函数（进入AP模式时触发）
void configModeCallback(WiFiManager *myWiFiManager) {
  Serial0.println("Entering config portal mode");
  Serial0.print("AP SSID: ");
  Serial0.println(myWiFiManager->getConfigPortalSSID());
  Serial0.println("AP IP: 192.168.4.1");

  // OLED显示配置模式信息
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("WiFi Config Mode");
  display.println("--------------------");

  display.setCursor(0, 20);
  display.println("Connect to WiFi:");
  display.setTextSize(1);
  display.setCursor(0, 32);
  display.println("ESP32-Servo-Setup");

  display.setCursor(0, 48);
  display.println("Then open:");
  display.setCursor(0, 56);
  display.println("192.168.4.1");

  display.display();
}

// 初始化WiFi连接（使用WiFiManager）
void initWiFi() {
  WiFiManager wifiManager;

  // 设置配置门户超时（180秒 = 3分钟）
  wifiManager.setConfigPortalTimeout(180);

  // 设置进入配置门户时的回调函数
  wifiManager.setAPCallback(configModeCallback);

  // OLED显示提示信息
  systemStatus = "WiFi init...";
  updateOLEDDisplay();

  Serial0.println("Starting WiFi configuration...");
  Serial0.println("If no saved WiFi, AP will start:");
  Serial0.println("  SSID: ESP32-Servo-Setup");
  Serial0.println("  IP: 192.168.4.1");

  // 尝试自动连接或启动配置门户
  // 热点名称: ESP32-Servo-Setup
  // 如果连接失败，会创建热点让用户配置
  if (!wifiManager.autoConnect("ESP32-Servo-Setup")) {
    Serial0.println("Failed to connect and timeout");
    systemIP = "Config Timeout";
    systemStatus = "WiFi Timeout";
    updateOLEDDisplay();

    dualLog("WiFi timeout!",
            "WiFi配置超时，3秒后重启");
    delay(3000);
    // 重启ESP32重新尝试
    ESP.restart();
  }

  // 成功连接WiFi
  Serial0.println("\nWiFi connected!");
  Serial0.print("SSID: ");
  Serial0.println(WiFi.SSID());
  Serial0.print("IP: ");
  Serial0.println(WiFi.localIP());

  // 更新全局IP变量
  systemIP = WiFi.localIP().toString();
  systemStatus = "WiFi OK";
  updateOLEDDisplay();

  dualLog("WiFi connected",
          "WiFi已连接: " + WiFi.SSID() + ", IP: " + systemIP);
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

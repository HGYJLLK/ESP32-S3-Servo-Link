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

// 获取所有舵机角度
void handleAngles() {
  String json = "{\"angles\":[" +
                String(servoAngles[0]) + "," +
                String(servoAngles[1]) + "," +
                String(servoAngles[2]) + "," +
                String(servoAngles[3]) + "]}";
  server.send(200, "application/json", json);
}

// 处理设定角度请求
void handleSet() {
  if (server.hasArg("servo") && server.hasArg("angle")) {
    int servoNum = server.arg("servo").toInt();
    int angle = server.arg("angle").toInt();

    if (servoNum >= 0 && servoNum < NUM_SERVOS && angle >= MIN_ANGLE && angle <= MAX_ANGLE) {
      setServoAngle(servoNum, angle);

      String msg = "舵机" + String(servoNum) + " 设定到 " + String(angle) + "度";
      serialPrintln(msg);

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
      Serial0.printf("[%u] 断开连接\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial0.printf("[%u] 连接来自 %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        // 发送历史日志
        webSocket.sendTXT(num, logBuffer);
      }
      break;
  }
}

// 初始化WiFi连接
void initWiFi() {
  Serial0.print("正在连接WiFi: ");
  Serial0.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial0.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial0.println("\nWiFi连接成功!");
    Serial0.print("IP地址: ");
    Serial0.println(WiFi.localIP());
    serialPrintln("WiFi已连接，IP: " + WiFi.localIP().toString());
  } else {
    Serial0.println("\nWiFi连接失败!");
    serialPrintln("WiFi连接失败，请检查SSID和密码");
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
  Serial0.println("HTTP服务器已启动");

  // 启动WebSocket服务器
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial0.println("WebSocket服务器已启动");

  Serial0.println("\n=== 系统就绪 ===");
  Serial0.print("请访问: http://");
  Serial0.println(WiFi.localIP());

  serialPrintln("=== 系统初始化完成 ===");
  serialPrintln("请在浏览器中打开: http://" + WiFi.localIP().toString());
}

// Web服务器循环处理
void handleWebServer() {
  server.handleClient();
  webSocket.loop();
}

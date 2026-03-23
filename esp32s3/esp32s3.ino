#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

// WiFi配置
const char* ssid = "GEEK2.4G";
const char* password = "34163416";

// 创建PCA9685对象，默认I2C地址为0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// 定义舵机通道（使用0,1,2,3接口）
#define SERVO_0 0
#define SERVO_1 1
#define SERVO_2 2
#define SERVO_3 3

// 270度舵机脉冲宽度设置（500us-2500us）
// PCA9685工作在50Hz，12位分辨率（4096步）
// 500us = 500/20000 * 4096 ≈ 102
// 2500us = 2500/20000 * 4096 = 512
#define SERVOMIN  102  // 对应500us（0度）
#define SERVOMAX  512  // 对应2500us（270度）

// ESP32-S3的I2C引脚
#define SDA_PIN 21
#define SCL_PIN 20

// 舵机当前角度（0-270度）
int servoAngles[4] = {135, 135, 135, 135}; // 初始位置在中间

// 每次移动的步进角度
#define STEP_ANGLE 5

// Web服务器和WebSocket
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// 存储日志信息
String logBuffer = "";
const int MAX_LOG_SIZE = 2000;

// 自定义Serial打印函数，同时发送到WebSocket
void serialPrintln(String msg) {
  Serial0.println(msg);

  // 添加到日志缓冲区
  logBuffer += msg + "\n";
  if (logBuffer.length() > MAX_LOG_SIZE) {
    logBuffer = logBuffer.substring(logBuffer.length() - MAX_LOG_SIZE);
  }

  // 通过WebSocket广播
  webSocket.broadcastTXT(msg);
}

void serialPrint(String msg) {
  Serial0.print(msg);

  // 添加到日志缓冲区
  logBuffer += msg;
  if (logBuffer.length() > MAX_LOG_SIZE) {
    logBuffer = logBuffer.substring(logBuffer.length() - MAX_LOG_SIZE);
  }

  // 通过WebSocket广播
  webSocket.broadcastTXT(msg);
}

// 设置舵机角度（0-270度）
void setServoAngle(uint8_t channel, int angle) {
  // 限制角度范围
  if (angle < 0) angle = 0;
  if (angle > 270) angle = 270;

  // 将角度转换为脉冲宽度
  int pulse = map(angle, 0, 270, SERVOMIN, SERVOMAX);

  // 设置PWM
  pwm.setPWM(channel, 0, pulse);

  // 更新角度记录
  servoAngles[channel] = angle;
}

// 移动舵机
void moveServo(int servoNum, int direction) {
  if (servoNum < 0 || servoNum > 3) return;

  int newAngle = servoAngles[servoNum] + (direction * STEP_ANGLE);

  // 限制范围
  if (newAngle < 0) newAngle = 0;
  if (newAngle > 270) newAngle = 270;

  setServoAngle(servoNum, newAngle);

  String msg = "舵机" + String(servoNum) + " " +
               (direction > 0 ? "正转" : "反转") +
               " -> " + String(newAngle) + "度";
  serialPrintln(msg);
}

// HTML网页
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32S3 舵机控制</title>
  <style>
    body { font-family: Arial; padding: 20px; background: #f0f0f0; margin: 0; }
    .container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; }
    h1 { text-align: center; color: #333; margin-bottom: 15px; }
    .main-layout { display: flex; gap: 20px; }
    .left-panel { flex: 1; min-width: 400px; }
    .right-panel { flex: 1; min-width: 400px; }
    .servo { margin: 10px 0; padding: 12px; background: #f9f9f9; border-radius: 5px; }
    .servo h3 { margin: 0 0 8px 0; font-size: 16px; }
    .controls { display: flex; justify-content: center; gap: 5px; align-items: center; flex-wrap: wrap; }
    .btn { padding: 8px 15px; border: none; border-radius: 5px; cursor: pointer; font-weight: bold; color: white; font-size: 14px; }
    .btn-left { background: #ff6b6b; }
    .btn-right { background: #4ecdc4; }
    .angle { min-width: 55px; text-align: center; font-size: 16px; font-weight: bold; }
    .log-section h3 { margin: 0 0 10px 0; }
    #log { background: #000; color: #0f0; padding: 10px; height: 500px; overflow-y: auto; font-family: monospace; font-size: 12px; border-radius: 5px; }
    .status { text-align: center; padding: 8px; margin-bottom: 15px; border-radius: 5px; font-weight: bold; }
    .connected { background: #d4edda; color: #155724; }
    .disconnected { background: #f8d7da; color: #721c24; }
    .batch-buttons { text-align: center; margin-top: 15px; padding-top: 15px; border-top: 2px solid #ddd; }
    input[type="number"] { width: 55px; padding: 5px; border: 1px solid #ccc; border-radius: 3px; }
  </style>
</head>
<body>
  <div class="container">
    <h1>ESP32S3 舵机控制</h1>
    <div id="status" class="status disconnected">断开连接</div>

    <div class="main-layout">
      <div class="left-panel">
        <h2 style="margin-top:0; font-size:18px;">舵机控制</h2>

        <div class="servo">
          <h3>舵机 0</h3>
          <div class="controls">
            <button class="btn btn-left" onclick="move(0, -1)">◀</button>
            <div class="angle" id="angle0">135°</div>
            <button class="btn btn-right" onclick="move(0, 1)">▶</button>
            <input type="number" id="input0" min="0" max="270" value="135">
            <button class="btn" style="background:#666;" onclick="setAngle(0)">设定</button>
          </div>
        </div>

        <div class="servo">
          <h3>舵机 1</h3>
          <div class="controls">
            <button class="btn btn-left" onclick="move(1, -1)">◀</button>
            <div class="angle" id="angle1">135°</div>
            <button class="btn btn-right" onclick="move(1, 1)">▶</button>
            <input type="number" id="input1" min="0" max="270" value="135">
            <button class="btn" style="background:#666;" onclick="setAngle(1)">设定</button>
          </div>
        </div>

        <div class="servo">
          <h3>舵机 2</h3>
          <div class="controls">
            <button class="btn btn-left" onclick="move(2, -1)">◀</button>
            <div class="angle" id="angle2">135°</div>
            <button class="btn btn-right" onclick="move(2, 1)">▶</button>
            <input type="number" id="input2" min="0" max="270" value="135">
            <button class="btn" style="background:#666;" onclick="setAngle(2)">设定</button>
          </div>
        </div>

        <div class="servo">
          <h3>舵机 3</h3>
          <div class="controls">
            <button class="btn btn-left" onclick="move(3, -1)">◀</button>
            <div class="angle" id="angle3">135°</div>
            <button class="btn btn-right" onclick="move(3, 1)">▶</button>
            <input type="number" id="input3" min="0" max="270" value="135">
            <button class="btn" style="background:#666;" onclick="setAngle(3)">设定</button>
          </div>
        </div>

        <div class="batch-buttons">
          <button class="btn" style="background:#ff9800; padding:10px 30px;" onclick="resetAll()">全部归零</button>
          <button class="btn" style="background:#9c27b0; padding:10px 30px; margin-left:10px;" onclick="centerAll()">全部居中</button>

          <div style="margin-top:15px;">
            <input type="number" id="allAngle" min="0" max="270" value="135" style="width:70px; padding:8px; font-size:14px;">
            <button class="btn" style="background:#2196F3; padding:10px 30px; margin-left:10px;" onclick="setAllAngle()">全部转到同角度</button>
          </div>

          <div style="margin-top:15px; padding-top:15px; border-top:1px solid #ddd;">
            <div style="font-weight:bold; margin-bottom:10px;">分别设定角度并同时执行：</div>
            <div style="display:flex; gap:10px; justify-content:center; align-items:center; flex-wrap:wrap;">
              <div>
                <label style="font-size:12px;">舵机0:</label>
                <input type="number" id="batch0" min="0" max="270" value="135" style="width:60px; padding:5px;">
              </div>
              <div>
                <label style="font-size:12px;">舵机1:</label>
                <input type="number" id="batch1" min="0" max="270" value="135" style="width:60px; padding:5px;">
              </div>
              <div>
                <label style="font-size:12px;">舵机2:</label>
                <input type="number" id="batch2" min="0" max="270" value="135" style="width:60px; padding:5px;">
              </div>
              <div>
                <label style="font-size:12px;">舵机3:</label>
                <input type="number" id="batch3" min="0" max="270" value="135" style="width:60px; padding:5px;">
              </div>
            </div>
            <button class="btn" style="background:#00bcd4; padding:10px 30px; margin-top:10px;" onclick="batchSetAll()">同时执行</button>
          </div>
        </div>
      </div>

      <div class="right-panel">
        <div class="log-section">
          <h2 style="margin-top:0; font-size:18px;">串口输出</h2>
          <div id="log"></div>
        </div>
      </div>
    </div>
  </div>

  <script>
    var ws;
    var angles = [135, 135, 135, 135];

    function initWebSocket() {
      ws = new WebSocket('ws://' + window.location.hostname + ':81');

      ws.onopen = function() {
        document.getElementById('status').textContent = '已连接';
        document.getElementById('status').className = 'status connected';
        addLog('连接成功');
      };

      ws.onclose = function() {
        document.getElementById('status').textContent = '断开连接';
        document.getElementById('status').className = 'status disconnected';
        addLog('断开连接，3秒后重连...');
        setTimeout(initWebSocket, 3000);
      };

      ws.onerror = function() {
        addLog('WebSocket 错误');
      };

      ws.onmessage = function(event) {
        addLog(event.data);
      };
    }

    function move(servo, direction) {
      fetch('/move?servo=' + servo + '&dir=' + direction)
        .then(response => response.json())
        .then(data => {
          if (data.angle !== undefined) {
            angles[servo] = data.angle;
            document.getElementById('angle' + servo).textContent = data.angle + '°';
            document.getElementById('input' + servo).value = data.angle;
          }
        });
    }

    function setAngle(servo) {
      var angle = parseInt(document.getElementById('input' + servo).value);
      if (angle < 0 || angle > 270) {
        alert('角度必须在0-270度之间');
        return;
      }
      fetch('/set?servo=' + servo + '&angle=' + angle)
        .then(response => response.json())
        .then(data => {
          if (data.angle !== undefined) {
            angles[servo] = data.angle;
            document.getElementById('angle' + servo).textContent = data.angle + '°';
          }
        });
    }

    function resetAll() {
      for (let i = 0; i < 4; i++) {
        fetch('/set?servo=' + i + '&angle=0')
          .then(response => response.json())
          .then(data => {
            if (data.angle !== undefined) {
              angles[data.servo] = data.angle;
              document.getElementById('angle' + data.servo).textContent = data.angle + '°';
              document.getElementById('input' + data.servo).value = data.angle;
            }
          });
      }
    }

    function centerAll() {
      for (let i = 0; i < 4; i++) {
        fetch('/set?servo=' + i + '&angle=135')
          .then(response => response.json())
          .then(data => {
            if (data.angle !== undefined) {
              angles[data.servo] = data.angle;
              document.getElementById('angle' + data.servo).textContent = data.angle + '°';
              document.getElementById('input' + data.servo).value = data.angle;
            }
          });
      }
    }

    function setAllAngle() {
      var angle = parseInt(document.getElementById('allAngle').value);
      if (angle < 0 || angle > 270) {
        alert('角度必须在0-270度之间');
        return;
      }
      for (let i = 0; i < 4; i++) {
        fetch('/set?servo=' + i + '&angle=' + angle)
          .then(response => response.json())
          .then(data => {
            if (data.angle !== undefined) {
              angles[data.servo] = data.angle;
              document.getElementById('angle' + data.servo).textContent = data.angle + '°';
              document.getElementById('input' + data.servo).value = data.angle;
            }
          });
      }
    }

    function batchSetAll() {
      var batchAngles = [];
      for (let i = 0; i < 4; i++) {
        var angle = parseInt(document.getElementById('batch' + i).value);
        if (angle < 0 || angle > 270) {
          alert('舵机' + i + '的角度必须在0-270度之间');
          return;
        }
        batchAngles.push(angle);
      }

      // 同时发送所有请求
      for (let i = 0; i < 4; i++) {
        fetch('/set?servo=' + i + '&angle=' + batchAngles[i])
          .then(response => response.json())
          .then(data => {
            if (data.angle !== undefined) {
              angles[data.servo] = data.angle;
              document.getElementById('angle' + data.servo).textContent = data.angle + '°';
              document.getElementById('input' + data.servo).value = data.angle;
            }
          });
      }
    }

    function addLog(message) {
      var log = document.getElementById('log');
      var timestamp = new Date().toLocaleTimeString();
      log.innerHTML += '[' + timestamp + '] ' + message + '<br>';
      log.scrollTop = log.scrollHeight;
    }

    // 初始化
    initWebSocket();

    // 获取初始角度
    fetch('/angles')
      .then(response => response.json())
      .then(data => {
        for (let i = 0; i < 4; i++) {
          angles[i] = data.angles[i];
          document.getElementById('angle' + i).textContent = data.angles[i] + '°';
          document.getElementById('input' + i).value = data.angles[i];
        }
      });
  </script>
</body>
</html>
)rawliteral";

// 处理网页请求
void handleRoot() {
  server.send(200, "text/html", index_html);
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

    if (servoNum >= 0 && servoNum <= 3 && angle >= 0 && angle <= 270) {
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

void setup() {
  // 初始化串口
  Serial0.begin(115200);
  Serial0.println("\n\n=== ESP32S3 舵机控制系统启动 ===");

  // 初始化I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial0.println("I2C初始化完成");

  // 初始化PCA9685
  pwm.begin();
  pwm.setPWMFreq(50);  // 舵机工作频率为50Hz
  delay(100);
  Serial0.println("PCA9685初始化完成");

  // 设置舵机初始位置（中间位置135度）
  for (int i = 0; i < 4; i++) {
    setServoAngle(i, 135);
  }
  Serial0.println("舵机初始化完成，位置：135度");

  // 连接WiFi
  Serial0.print("正在连接WiFi: ");
  Serial0.println(ssid);

  WiFi.begin(ssid, password);

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

  // 配置Web服务器
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

void loop() {
  // 处理Web服务器请求
  server.handleClient();

  // 处理WebSocket
  webSocket.loop();

  // 可以在这里添加其他循环任务
}
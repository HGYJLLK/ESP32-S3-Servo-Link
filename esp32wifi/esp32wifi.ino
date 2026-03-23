// ESP32-CAM WiFi Dashboard with Camera Monitoring
// Access via: http://<ESP32-IP>/

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// Configuration
#define ENABLE_CAMERA true  // Set to false to disable camera

// WiFi credentials
const char* ssid = "GEEK2.4G";
const char* password = "34163416";

// Camera pins for AI-THINKER ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// LED pins
#define LED_FLASH_GPIO     4
#define LED_RED_GPIO      33

WebServer server(80);

// Camera status
bool cameraInitialized = false;
int captureSuccessCount = 0;
int captureFailCount = 0;

// Buffer for serial messages
#define MAX_MESSAGES 50
#define MAX_MESSAGE_LENGTH 128
char serialMessages[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
int messageIndex = 0;
int messageCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\nESP32-CAM Dashboard Starting...");

  // Setup LED pins
  pinMode(LED_FLASH_GPIO, OUTPUT);
  pinMode(LED_RED_GPIO, OUTPUT);
  digitalWrite(LED_FLASH_GPIO, LOW);
  digitalWrite(LED_RED_GPIO, HIGH);  // Red on during init

  // Initialize message buffer
  for(int i = 0; i < MAX_MESSAGES; i++) {
    serialMessages[i][0] = '\0';
  }

  // Initialize camera (optional)
  #if ENABLE_CAMERA
    Serial.println("Initializing camera...");
    if (initCamera()) {
      Serial.println("Camera initialized successfully!");
      addMessage("Camera: OK");
      cameraInitialized = true;
      digitalWrite(LED_RED_GPIO, LOW);
      digitalWrite(LED_FLASH_GPIO, HIGH);  // White LED on = camera OK
    } else {
      Serial.println("Camera init failed!");
      addMessage("Camera: FAILED");
      cameraInitialized = false;
      digitalWrite(LED_RED_GPIO, HIGH);  // Red LED stays on = failed
    }
  #else
    Serial.println("Camera disabled in config");
    addMessage("Camera: DISABLED");
    digitalWrite(LED_RED_GPIO, LOW);
  #endif

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("Dashboard URL: http://");
    Serial.println(WiFi.localIP());

    addMessage("WiFi connected!");
    char ipMsg[MAX_MESSAGE_LENGTH];
    snprintf(ipMsg, MAX_MESSAGE_LENGTH, "IP: %s", WiFi.localIP().toString().c_str());
    addMessage(ipMsg);
  } else {
    Serial.println("\nWiFi connection failed!");
    addMessage("WiFi connection failed!");
    return;
  }

  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/camera", handleCamera);
  server.on("/camera/test", handleCameraTest);
  server.on("/messages", handleMessages);

  server.begin();
  Serial.println("Web server started!");
  addMessage("Web server started!");
}

void loop() {
  server.handleClient();

  // Test camera capture every 5 seconds
  static unsigned long lastCapture = 0;
  if (cameraInitialized && millis() - lastCapture > 5000) {
    lastCapture = millis();

    camera_fb_t * fb = esp_camera_fb_get();
    if (fb) {
      captureSuccessCount++;
      esp_camera_fb_return(fb);

      // Quick flash to indicate capture success
      digitalWrite(LED_FLASH_GPIO, LOW);
      delay(100);
      digitalWrite(LED_FLASH_GPIO, HIGH);
    } else {
      captureFailCount++;

      // Red blink to indicate capture failure
      digitalWrite(LED_RED_GPIO, HIGH);
      delay(200);
      digitalWrite(LED_RED_GPIO, LOW);
    }
  }
}

void addMessage(const char* message) {
  // Add timestamp
  unsigned long seconds = millis() / 1000;
  snprintf(serialMessages[messageIndex], MAX_MESSAGE_LENGTH, "[%lu] %s", seconds, message);

  messageIndex = (messageIndex + 1) % MAX_MESSAGES;
  if (messageCount < MAX_MESSAGES) {
    messageCount++;
  }
}

bool initCamera() {
  // Power cycle camera
  pinMode(PWDN_GPIO_NUM, OUTPUT);
  digitalWrite(PWDN_GPIO_NUM, HIGH);
  delay(100);
  digitalWrite(PWDN_GPIO_NUM, LOW);
  delay(100);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;  // 320x240
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_LATEST;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }

  // Configure sensor settings
  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) {
    s->set_brightness(s, 0);
    s->set_contrast(s, 0);
    s->set_saturation(s, 0);
    s->set_whitebal(s, 1);
    s->set_awb_gain(s, 1);
    s->set_exposure_ctrl(s, 1);
    s->set_gain_ctrl(s, 1);
  }

  return true;
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32-CAM Dashboard</title>
  <style>
    body { font-family: Arial; background: #2c3e50; color: white; padding: 20px; margin: 0; }
    .container { max-width: 1000px; margin: 0 auto; }
    h1 { text-align: center; margin-bottom: 30px; }
    .dashboard { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 15px; }
    .card { background: #34495e; border-radius: 10px; padding: 20px; }
    .card-title { font-size: 14px; color: #bdc3c7; margin-bottom: 10px; }
    .card-value { font-size: 32px; font-weight: bold; }
    .card-unit { font-size: 16px; color: #95a5a6; }
    .status-ok { color: #2ecc71; }
    .status-fail { color: #e74c3c; }
    .camera-card { grid-column: 1 / -1; }
    .camera-container { text-align: center; background: #2c3e50; padding: 15px; margin-top: 10px; border-radius: 8px; }
    .camera-image { max-width: 100%; border-radius: 5px; }
    .refresh-btn { background: #3498db; color: white; border: none; padding: 10px 25px; border-radius: 5px; cursor: pointer; margin-top: 10px; }
    .refresh-btn:hover { background: #2980b9; }
    .stats-row { display: flex; justify-content: space-around; margin-top: 10px; padding-top: 10px; border-top: 1px solid #7f8c8d; }
    .stat-value { font-size: 20px; font-weight: bold; }
  </style>
</head>
<body>
  <div class="container">
    <h1>ESP32-CAM Dashboard</h1>

    <div class="dashboard">
      <div class="card">
        <div class="card-title">Uptime</div>
        <div class="card-value" id="uptime">0<span class="card-unit">s</span></div>
      </div>

      <div class="card">
        <div class="card-title">Free Heap</div>
        <div class="card-value" id="heap">0<span class="card-unit">B</span></div>
      </div>

      <div class="card">
        <div class="card-title">Camera Status</div>
        <div class="card-value" id="camera-status">--</div>
        <div class="stats-row">
          <div class="stat-item">
            <div class="stat-label">Success</div>
            <div class="stat-value status-ok" id="success-count">0</div>
          </div>
          <div class="stat-item">
            <div class="stat-label">Failed</div>
            <div class="stat-value status-fail" id="fail-count">0</div>
          </div>
        </div>
        <button class="refresh-btn" onclick="testCamera()" style="margin-top:15px;width:100%;">Test Camera</button>
      </div>

      <div class="card camera-card">
        <div class="card-title">Camera Feed</div>
        <div class="camera-container">
          <img id="camera-img" class="camera-image" src="/camera" alt="Camera feed">
          <br>
          <button class="refresh-btn" onclick="refreshCamera()">Refresh Image</button>
        </div>
      </div>
    </div>
  </div>

  <script>
    function updateStatus() {
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          document.getElementById('uptime').innerHTML = data.uptime + '<span class="card-unit">s</span>';
          document.getElementById('heap').innerHTML = data.heap + '<span class="card-unit">B</span>';

          const cameraStatus = document.getElementById('camera-status');
          if (data.cameraOk) {
            cameraStatus.textContent = 'OK';
            cameraStatus.className = 'card-value status-ok';
          } else {
            cameraStatus.textContent = 'FAILED';
            cameraStatus.className = 'card-value status-fail';
          }

          document.getElementById('success-count').textContent = data.captureSuccess;
          document.getElementById('fail-count').textContent = data.captureFail;
        })
        .catch(err => console.error('Error:', err));
    }

    function refreshCamera() {
      const img = document.getElementById('camera-img');
      img.src = '/camera?t=' + new Date().getTime();
    }

    function testCamera() {
      fetch('/camera/test')
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            alert('Camera Test SUCCESS!\nImage size: ' + data.size + ' bytes\nResolution: ' + data.width + 'x' + data.height);
            refreshCamera();
          } else {
            alert('Camera Test FAILED!\nError: ' + data.error);
          }
          setTimeout(updateStatus, 500);
        })
        .catch(err => alert('Camera test error: ' + err));
    }

    // Update status every 2 seconds
    setInterval(updateStatus, 2000);
    updateStatus();

    // Auto-refresh camera every 5 seconds
    setInterval(refreshCamera, 5000);
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleStatus() {
  String json = "{";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"cameraOk\":" + String(cameraInitialized ? "true" : "false") + ",";
  json += "\"captureSuccess\":" + String(captureSuccessCount) + ",";
  json += "\"captureFail\":" + String(captureFailCount);
  json += "}";

  server.send(200, "application/json", json);
}

void handleCamera() {
  if (!cameraInitialized) {
    server.send(503, "text/plain", "Camera not initialized");
    return;
  }

  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }

  server.sendHeader("Content-Disposition", "inline; filename=camera.jpg");
  server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);

  esp_camera_fb_return(fb);
}

void handleCameraTest() {
  String json = "{";

  if (!cameraInitialized) {
    json += "\"success\":false,";
    json += "\"error\":\"Camera not initialized\"";
    json += "}";
    server.send(200, "application/json", json);
    return;
  }

  Serial.println("Web camera test triggered...");
  camera_fb_t * fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Camera test capture failed!");
    captureFailCount++;

    json += "\"success\":false,";
    json += "\"error\":\"Capture failed - check power supply\"";
    json += "}";

    // Red LED blink
    digitalWrite(LED_RED_GPIO, HIGH);
    delay(200);
    digitalWrite(LED_RED_GPIO, LOW);
  } else {
    Serial.println("Camera test capture SUCCESS!");
    Serial.print("Image size: ");
    Serial.print(fb->len);
    Serial.println(" bytes");

    captureSuccessCount++;

    json += "\"success\":true,";
    json += "\"size\":" + String(fb->len) + ",";
    json += "\"width\":" + String(fb->width) + ",";
    json += "\"height\":" + String(fb->height);
    json += "}";

    esp_camera_fb_return(fb);

    // White LED flash
    digitalWrite(LED_FLASH_GPIO, LOW);
    delay(100);
    digitalWrite(LED_FLASH_GPIO, HIGH);
  }

  server.send(200, "application/json", json);
}

void handleMessages() {
  String json = "{\"messages\":[";

  // Get messages in chronological order
  int start = (messageCount < MAX_MESSAGES) ? 0 : messageIndex;
  for (int i = 0; i < messageCount; i++) {
    int idx = (start + i) % MAX_MESSAGES;
    if (i > 0) json += ",";
    json += "\"";
    json += serialMessages[idx];
    json += "\"";
  }

  json += "],\"heap\":";
  json += ESP.getFreeHeap();
  json += "}";

  server.send(200, "application/json", json);
}

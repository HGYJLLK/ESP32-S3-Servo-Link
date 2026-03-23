// ESP32-CAM OV2640 Test - Minimal Configuration
// Works with AI-THINKER ESP32-CAM board
// Access via: http://<ESP32-IP>/

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "GEEK2.4G";
const char* password = "34163416";

// AI-THINKER ESP32-CAM Pin Configuration
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

// LED
#define LED_PIN 33

WebServer server(80);
bool cameraOk = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nESP32-CAM OV2640 Starting...");

  // LED indicator
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // ON during init

  // Camera configuration - optimized for OV2640
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

  // OV2640 optimal settings
  config.xclk_freq_hz = 20000000;  // 20MHz for OV2640
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;  // 800x600
  config.jpeg_quality = 12;  // 0-63, lower means higher quality
  config.fb_count = 1;

  // Check if PSRAM available
  if(psramFound()) {
    Serial.println("PSRAM found!");
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    Serial.println("No PSRAM - using DRAM");
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.frame_size = FRAMESIZE_VGA; // Reduce size without PSRAM
  }

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init FAILED: 0x%x\n", err);
    digitalWrite(LED_PIN, LOW); // LED off = failed
    return;
  }
  Serial.println("Camera init SUCCESS!");

  // Get sensor for OV2640 specific settings
  sensor_t * s = esp_camera_sensor_get();
  if (s->id.PID == OV2640_PID) {
    Serial.println("OV2640 detected!");

    // OV2640 specific optimizations
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
    s->set_special_effect(s, 0); // 0-6: None, Negative, Grayscale, etc
    s->set_whitebal(s, 1);       // 0=disable, 1=enable
    s->set_awb_gain(s, 1);       // 0=disable, 1=enable
    s->set_wb_mode(s, 0);        // 0-4: Auto, Sunny, Cloudy, Office, Home
    s->set_exposure_ctrl(s, 1);  // 0=disable, 1=enable
    s->set_aec2(s, 0);           // 0=disable, 1=enable
    s->set_ae_level(s, 0);       // -2 to 2
    s->set_aec_value(s, 300);    // 0 to 1200
    s->set_gain_ctrl(s, 1);      // 0=disable, 1=enable
    s->set_agc_gain(s, 0);       // 0 to 30
    s->set_gainceiling(s, (gainceiling_t)0); // 0 to 6
    s->set_bpc(s, 0);            // 0=disable, 1=enable
    s->set_wpc(s, 1);            // 0=disable, 1=enable
    s->set_raw_gma(s, 1);        // 0=disable, 1=enable
    s->set_lenc(s, 1);           // 0=disable, 1=enable
    s->set_hmirror(s, 0);        // 0=disable, 1=enable
    s->set_vflip(s, 0);          // 0=disable, 1=enable
    s->set_dcw(s, 1);            // 0=disable, 1=enable
    s->set_colorbar(s, 0);       // 0=disable, 1=enable
  }

  // Test capture
  Serial.println("Testing capture...");
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Capture test FAILED!");
    cameraOk = false;
  } else {
    Serial.printf("Capture test OK! Size: %d bytes, %dx%d\n",
                  fb->len, fb->width, fb->height);
    esp_camera_fb_return(fb);
    cameraOk = true;
    digitalWrite(LED_PIN, LOW); // LED off = ready
  }

  // Connect WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("Camera URL: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi failed!");
  }

  // Setup web server
  server.on("/", handleRoot);
  server.on("/capture", handleCapture);
  server.on("/stream", handleStream);

  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>ESP32-CAM OV2640</title>";
  html += "<style>";
  html += "body{font-family:Arial;text-align:center;margin:20px;background:#f0f0f0}";
  html += "h1{color:#333}";
  html += "img{max-width:100%;height:auto;border:3px solid #333;margin:20px 0}";
  html += "button{padding:15px 30px;margin:10px;font-size:16px;background:#4CAF50;color:white;border:none;border-radius:5px;cursor:pointer}";
  html += "button:hover{background:#45a049}";
  html += ".info{background:white;padding:15px;border-radius:5px;margin:20px auto;max-width:600px;text-align:left}";
  html += "</style></head><body>";
  html += "<h1>ESP32-CAM OV2640</h1>";

  html += "<div class='info'>";
  html += "<p><b>Status:</b> " + String(cameraOk ? "OK" : "FAILED") + "</p>";
  html += "<p><b>IP:</b> " + WiFi.localIP().toString() + "</p>";
  html += "<p><b>Heap:</b> " + String(ESP.getFreeHeap()) + " bytes</p>";
  html += "</div>";

  html += "<img id='cam' src='/capture'>";
  html += "<br>";
  html += "<button onclick='location.reload()'>Refresh</button>";
  html += "<button onclick='location.href=\"/stream\"'>Live Stream</button>";

  html += "<script>";
  html += "setInterval(function(){";
  html += "document.getElementById('cam').src='/capture?t='+Date.now();";
  html += "},2000);";
  html += "</script>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleCapture() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Capture failed");
    server.send(500, "text/plain", "Camera Error");
    return;
  }

  server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
  server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void handleStream() {
  WiFiClient client = server.client();

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (client.connected()) {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Stream capture failed");
      break;
    }

    String header = "--frame\r\n";
    header += "Content-Type: image/jpeg\r\n";
    header += "Content-Length: " + String(fb->len) + "\r\n\r\n";

    client.write(header.c_str(), header.length());
    client.write(fb->buf, fb->len);
    client.write("\r\n");

    esp_camera_fb_return(fb);

    if (!client.connected()) break;
    delay(30); // ~33 FPS
  }
}

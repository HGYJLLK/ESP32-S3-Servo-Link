// ESP32-CAM Web Server - Simple Camera Test
// Access camera via: http://<ESP32-IP>/

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

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
#define LED_FLASH_GPIO     4   // White LED (flash)
#define LED_RED_GPIO      33   // Red LED (status)

WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\nESP32-CAM Web Server Starting...");

  // Setup LED pins
  pinMode(LED_FLASH_GPIO, OUTPUT);
  pinMode(LED_RED_GPIO, OUTPUT);
  digitalWrite(LED_FLASH_GPIO, LOW);  // Flash LED off
  digitalWrite(LED_RED_GPIO, HIGH);   // Red LED on (initializing)

  Serial.println("LEDs initialized - Red LED should be ON");

  // Power cycle camera for clean initialization
  Serial.println("Initializing camera...");
  pinMode(PWDN_GPIO_NUM, OUTPUT);
  digitalWrite(PWDN_GPIO_NUM, HIGH);  // Power down
  delay(100);
  digitalWrite(PWDN_GPIO_NUM, LOW);   // Power up
  delay(100);

  // Configure camera
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
  config.xclk_freq_hz = 10000000;  // Lower frequency for OV2640 V2
  config.pixel_format = PIXFORMAT_JPEG;

  // Frame size and quality - Try different settings
  config.frame_size = FRAMESIZE_QVGA;    // 320x240 - slightly larger
  config.jpeg_quality = 10;               // Better quality
  config.fb_count = 1;                    // Single buffer
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_LATEST;  // Different grab mode

  Serial.println("Using QVGA (320x240) with PSRAM");

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);

    // Try fallback configuration with minimum settings
    Serial.println("Trying fallback configuration for OV2640 V2...");
    config.xclk_freq_hz = 8000000;       // Even lower frequency
    config.frame_size = FRAMESIZE_QQVGA; // 160x120 - minimum
    config.jpeg_quality = 20;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;

    err = esp_camera_init(&config);
    if (err != ESP_OK) {
      Serial.printf("Camera fallback also failed with error 0x%x\n", err);
      Serial.println("Check camera connection:");
      Serial.println("  - Is camera module properly connected?");
      Serial.println("  - Is camera ribbon cable inserted correctly?");
      Serial.println("  - Blue side facing PCB board");
      Serial.println("  - Try pressing RESET button after upload");
      Serial.println("  - OV2640 V2 detected but not responding");
      return;
    } else {
      Serial.println("Fallback successful with minimum resolution!");
    }
  }
  Serial.println("Camera initialized successfully!");

  // Camera init success - turn on white LED, turn off red LED
  digitalWrite(LED_RED_GPIO, LOW);     // Red LED off
  digitalWrite(LED_FLASH_GPIO, HIGH);  // White LED on (success!)
  Serial.println("SUCCESS: White LED should be ON now!");

  // Get camera sensor info
  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) {
    Serial.print("Camera sensor PID: 0x");
    Serial.println(s->id.PID, HEX);

    // Print sensor model name
    switch(s->id.PID) {
      case OV2640_PID: Serial.println("Camera: OV2640"); break;
      case OV3660_PID: Serial.println("Camera: OV3660"); break;
      case OV5640_PID: Serial.println("Camera: OV5640"); break;
      default: Serial.println("Camera: Unknown model"); break;
    }

    // Adjust camera settings
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
    s->set_hmirror(s, 0);        // 0 = disable, 1 = enable
    s->set_vflip(s, 0);          // 0 = disable, 1 = enable

    // Additional OV2640 settings
    s->set_whitebal(s, 1);       // Enable AWB
    s->set_awb_gain(s, 1);       // Enable AWB gain
    s->set_exposure_ctrl(s, 1);  // Enable AEC
    s->set_gain_ctrl(s, 1);      // Enable AGC
    s->set_lenc(s, 1);           // Enable lens correction
  }

  // WiFi disabled for testing - camera only mode
  Serial.println("WiFi disabled - testing camera only");
  Serial.println("Camera initialization complete!");

  // Try an immediate test capture
  Serial.println("\nTrying initial test capture...");
  delay(2000);  // Give camera MORE time to stabilize (2 seconds)

  camera_fb_t * test_fb = esp_camera_fb_get();
  if (!test_fb) {
    Serial.println("FAILED: Initial test capture failed!");
    Serial.println("This is likely a POWER SUPPLY issue.");
    Serial.println("Solution: Use external 5V power supply (1A minimum)");
    Serial.println("USB-TTL adapters often can't provide enough current.");

    // FAILED: Red LED blinks fast (power issue)
    for(int i = 0; i < 10; i++) {
      digitalWrite(LED_RED_GPIO, HIGH);
      delay(100);
      digitalWrite(LED_RED_GPIO, LOW);
      delay(100);
    }
    digitalWrite(LED_RED_GPIO, HIGH);  // Stay on to indicate failure
  } else {
    Serial.println("SUCCESS: Initial test capture works!");
    Serial.print("Image size: ");
    Serial.print(test_fb->len);
    Serial.println(" bytes");
    esp_camera_fb_return(test_fb);

    // SUCCESS: Both LEDs on solid
    digitalWrite(LED_FLASH_GPIO, HIGH);
    digitalWrite(LED_RED_GPIO, LOW);
  }

  Serial.println("\nLED Status Guide:");
  Serial.println("- White LED blinking every 3s = Camera working!");
  Serial.println("- Red LED blinking fast = Power supply issue");
  Serial.println("- Red LED solid = Initialization failed");

  /*
  // Uncomment this block after camera works
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("Camera URL: http://");
    Serial.println(WiFi.localIP());
    Serial.print("Stream URL: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/stream");
  } else {
    Serial.println("\nWiFi connection failed!");
    return;
  }

  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/capture", handleCapture);
  server.on("/stream", handleStream);

  server.begin();
  Serial.println("Web server started!");
  */
}

void loop() {
  // Camera test mode - capture photo every 3 seconds
  static unsigned long lastCapture = 0;

  if (millis() - lastCapture > 3000) {  // Changed to 3 seconds
    lastCapture = millis();

    Serial.println("\n--- Testing camera capture ---");
    delay(500);  // Longer delay before each capture

    camera_fb_t * fb = esp_camera_fb_get();

    if (!fb) {
      Serial.println("ERROR: Camera capture failed!");
      Serial.println("Possible causes:");
      Serial.println("  1. Insufficient power supply (most common)");
      Serial.println("  2. Camera sensor issue");
      Serial.println("  3. Try using external 5V power (1A minimum)");

      Serial.print("Free heap: ");
      Serial.println(ESP.getFreeHeap());

      // FAILED: Fast double blink on red LED
      digitalWrite(LED_FLASH_GPIO, LOW);
      digitalWrite(LED_RED_GPIO, HIGH);
      delay(150);
      digitalWrite(LED_RED_GPIO, LOW);
      delay(150);
      digitalWrite(LED_RED_GPIO, HIGH);
      delay(150);
      digitalWrite(LED_RED_GPIO, LOW);

    } else {
      Serial.println("SUCCESS: Camera capture working!");
      Serial.print("Image size: ");
      Serial.print(fb->len);
      Serial.println(" bytes");
      Serial.print("Image format: ");
      Serial.println(fb->format == PIXFORMAT_JPEG ? "JPEG" : "Other");
      Serial.print("Resolution: ");
      Serial.print(fb->width);
      Serial.print("x");
      Serial.println(fb->height);

      // SUCCESS: Single quick flash on white LED every 3 seconds
      digitalWrite(LED_FLASH_GPIO, LOW);
      delay(200);
      digitalWrite(LED_FLASH_GPIO, HIGH);

      esp_camera_fb_return(fb);
    }
  }

  // Uncomment after camera works and WiFi is enabled
  // server.handleClient();
}

// Main page with image and controls
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 20px; }";
  html += "img { max-width: 100%; height: auto; border: 2px solid #333; }";
  html += "button { padding: 15px 30px; margin: 10px; font-size: 16px; }";
  html += "</style></head><body>";
  html += "<h1>ESP32-CAM Web Server</h1>";
  html += "<p><img src='/capture' id='photo'></p>";
  html += "<p>";
  html += "<button onclick='location.reload()'>Refresh Photo</button>";
  html += "<button onclick='location.href=\"/stream\"'>Live Stream</button>";
  html += "</p>";
  html += "<p>Free Heap: " + String(ESP.getFreeHeap()) + " bytes</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Capture and return single photo
void handleCapture() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }

  server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
  server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);

  esp_camera_fb_return(fb);
}

// MJPEG streaming
void handleStream() {
  WiFiClient client = server.client();

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (client.connected()) {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
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
    delay(30); // ~30 FPS
  }
}

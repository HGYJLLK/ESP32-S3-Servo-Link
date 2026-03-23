// ESP32S I2C OLED Dashboard - Status Display
// Using SSD1306 128x64 OLED
// Wiring: VCC->3.3V, GND->GND, SDA->GPIO14, SCL->GPIO16

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

// WiFi credentials
const char* ssid = "GEEK2.4G";
const char* password = "34163416";

// Custom I2C pins - Recommended for ESP32-CAM with WiFi
#define I2C_SDA 14  // GPIO14 (cannot use SD card)
#define I2C_SCL 16  // GPIO16 (cannot use Serial2)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Temporary buffer for formatting
char tempBuffer[32];

// Dashboard update interval
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 2000; // 2 seconds

// Track WiFi connection state
bool wasConnected = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32S OLED Dashboard Starting...");
  Serial.print("I2C SDA: GPIO");
  Serial.println(I2C_SDA);
  Serial.print("I2C SCL: GPIO");
  Serial.println(I2C_SCL);

  Wire.begin(I2C_SDA, I2C_SCL);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 init failed!"));
    Serial.println(F("Check wiring:"));
    Serial.println(F("  VCC -> 3.3V"));
    Serial.println(F("  GND -> GND"));
    Serial.print(F("  SDA -> GPIO"));
    Serial.println(I2C_SDA);
    Serial.print(F("  SCL -> GPIO"));
    Serial.println(I2C_SCL);
    Serial.println(F("Try changing I2C address to 0x3D"));
    for(;;);
  }

  // Show splash screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("ESP32-CAM"));
  display.println(F("OLED Dashboard"));
  display.println(F("-------------"));
  display.println(F("Initializing..."));
  display.display();

  delay(2000);

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  //WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed!");
  }

  // Draw static dashboard layout (labels only)
  drawStaticLayout();

  Serial.println("Dashboard ready!");
  Serial.print("Initial free heap: ");
  Serial.println(ESP.getFreeHeap());
}

void loop() {
  // Update dashboard every 2 seconds
  if(millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();
    updateDashboard();

    // Debug: Print heap every 10 updates (20 seconds)
    static int updateCount = 0;
    updateCount++;
    if(updateCount % 10 == 0) {
      Serial.print("Free heap: ");
      Serial.print(ESP.getFreeHeap());
      Serial.print(" | Min free: ");
      Serial.println(ESP.getMinFreeHeap());
    }
  }

  // Handle serial commands
  if(Serial.available()) {
    int bytesRead = Serial.readBytesUntil('\n', tempBuffer, sizeof(tempBuffer) - 1);
    tempBuffer[bytesRead] = '\0';

    if (bytesRead > 0 && tempBuffer[bytesRead - 1] == '\r') {
      tempBuffer[bytesRead - 1] = '\0';
    }

    if (bytesRead > 0) {
      if (strcmp(tempBuffer, "heap") == 0) {
        Serial.print("Free heap: ");
        Serial.print(ESP.getFreeHeap());
        Serial.print(" | Min free: ");
        Serial.print(ESP.getMinFreeHeap());
        Serial.print(" | Largest block: ");
        Serial.println(ESP.getMaxAllocHeap());
      } else if (strcmp(tempBuffer, "reset") == 0) {
        Serial.println("Resetting...");
        ESP.restart();
      }
    }
  }
}

void drawStaticLayout() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Title
  display.setCursor(0, 0);
  display.println(F("== ESP32 Status =="));

  // Static labels - these never change
  display.setCursor(0, 12);
  display.print(F("Uptime:"));

  display.setCursor(0, 22);
  display.print(F("Heap:"));

  display.setCursor(0, 32);
  display.print(F("Temp:"));

  // WiFi section (labels only if connected)
  if (WiFi.status() == WL_CONNECTED) {
    display.setCursor(0, 42);
    display.print(F("IP:"));

    display.setCursor(0, 52);
    display.print(F("RSSI:"));
  } else {
    display.setCursor(0, 42);
    display.print(F("WiFi: Disconnected"));
  }

  display.display();
}

void updateDashboard() {
  // Define value area: starts at x=48 (after labels)
  const int VALUE_X = 48;
  const int VALUE_WIDTH = SCREEN_WIDTH - VALUE_X;

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Update Uptime (line 2, y=12)
  display.fillRect(VALUE_X, 12, VALUE_WIDTH, 8, SSD1306_BLACK);
  display.setCursor(VALUE_X, 12);
  unsigned long uptime = millis() / 1000;
  if (uptime < 60) {
    snprintf(tempBuffer, sizeof(tempBuffer), "%lus", uptime);
  } else if (uptime < 3600) {
    snprintf(tempBuffer, sizeof(tempBuffer), "%lum %lus", uptime/60, uptime%60);
  } else {
    snprintf(tempBuffer, sizeof(tempBuffer), "%luh %lum", uptime/3600, (uptime%3600)/60);
  }
  display.print(tempBuffer);

  // Update Heap (line 3, y=22)
  display.fillRect(VALUE_X, 22, VALUE_WIDTH, 8, SSD1306_BLACK);
  display.setCursor(VALUE_X, 22);
  snprintf(tempBuffer, sizeof(tempBuffer), "%d B", ESP.getFreeHeap());
  display.print(tempBuffer);

  // Update Temperature (line 4, y=32) - simulated
  display.fillRect(VALUE_X, 32, VALUE_WIDTH, 8, SSD1306_BLACK);
  display.setCursor(VALUE_X, 32);
  snprintf(tempBuffer, sizeof(tempBuffer), "%d C", random(20, 30));
  display.print(tempBuffer);

  // Update WiFi info
  bool currentlyConnected = (WiFi.status() == WL_CONNECTED);

  if (currentlyConnected) {
    // WiFi just connected - redraw labels if needed
    if (!wasConnected) {
      display.fillRect(0, 42, SCREEN_WIDTH, 20, SSD1306_BLACK);
      display.setCursor(0, 42);
      display.print(F("IP:"));
      display.setCursor(0, 52);
      display.print(F("RSSI:"));
      wasConnected = true;
    }

    // Update IP (line 5, y=42)
    display.fillRect(VALUE_X - 24, 42, VALUE_WIDTH + 24, 8, SSD1306_BLACK);
    display.setCursor(24, 42);
    IPAddress ip = WiFi.localIP();
    snprintf(tempBuffer, sizeof(tempBuffer), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    display.print(tempBuffer);

    // Update RSSI (line 6, y=52)
    display.fillRect(VALUE_X, 52, VALUE_WIDTH, 8, SSD1306_BLACK);
    display.setCursor(VALUE_X, 52);
    snprintf(tempBuffer, sizeof(tempBuffer), "%d dBm", WiFi.RSSI());
    display.print(tempBuffer);
  } else {
    // WiFi disconnected - redraw message if state changed
    if (wasConnected) {
      display.fillRect(0, 42, SCREEN_WIDTH, 20, SSD1306_BLACK);
      display.setCursor(0, 42);
      display.print(F("WiFi: Disconnected"));
      wasConnected = false;
    }
    // If was already disconnected, do nothing (message is already there)
  }

  display.display();
}

#ifndef CONFIG_H
#define CONFIG_H

// WiFi配置
const char* WIFI_SSID = "GEEK2.4G";
const char* WIFI_PASSWORD = "34163416";

// ESP32-S3的I2C引脚配置
// I2C总线0 (Wire) - PCA9685舵机驱动
#define SDA_PIN 21
#define SCL_PIN 20

// I2C总线1 (Wire1) - OLED屏幕（独立总线，避免冲突）
#define OLED_SDA_PIN 17
#define OLED_SCL_PIN 18

// PCA9685 I2C地址
#define PCA9685_ADDRESS 0x40

// OLED屏幕配置（0.96寸 SSD1306）
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // 复位引脚（-1表示共用Arduino复位引脚）
#define SCREEN_ADDRESS 0x3C  // 0.96寸屏常见地址，不行试试0x3D

// 舵机通道定义（PCA9685支持0-15共16个通道）
#define NUM_SERVOS 16        // 总通道数
#define QUICK_ACCESS_SERVOS 4  // 快速访问的通道数（OLED显示用）

// 270度舵机脉冲宽度设置（500us-2500us）
// PCA9685工作在50Hz，12位分辨率（4096步）
// 500us = 500/20000 * 4096 ≈ 102
// 2500us = 2500/20000 * 4096 = 512
#define SERVOMIN  102   // 对应500us（0度）
#define SERVOMAX  512   // 对应2500us（270度）
#define PWM_FREQ  50    // 舵机工作频率50Hz

// 舵机角度范围
#define MIN_ANGLE 0
#define MAX_ANGLE 270
#define DEFAULT_ANGLE 135  // 初始位置（中间位置）

// 每次移动的步进角度
#define STEP_ANGLE 5

// 服务器端口
#define HTTP_PORT 80
#define WEBSOCKET_PORT 81

// 日志缓冲区大小
#define MAX_LOG_SIZE 2000

// 串口波特率
#define SERIAL_BAUD 115200

#endif

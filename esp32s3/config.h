#ifndef CONFIG_H
#define CONFIG_H

// WiFi配置
const char* WIFI_SSID = "GEEK2.4G";
const char* WIFI_PASSWORD = "34163416";

// ESP32-S3的I2C引脚
#define SDA_PIN 21
#define SCL_PIN 20

// PCA9685 I2C地址
#define PCA9685_ADDRESS 0x40

// 舵机通道定义（使用0,1,2,3接口）
#define SERVO_0 0
#define SERVO_1 1
#define SERVO_2 2
#define SERVO_3 3
#define NUM_SERVOS 4

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

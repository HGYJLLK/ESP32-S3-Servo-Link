// config.h - 配置常量
#ifndef CONFIG_H
#define CONFIG_H

// WiFi配置 (与esp32s3项目相同)
const char* WIFI_SSID = "GEEK2.4G";
const char* WIFI_PASSWORD = "34163416";

// PCA9685配置
#define SERVOMIN  102  // 最小脉冲宽度 (500μs)
#define SERVOMAX  512  // 最大脉冲宽度 (2500μs)
#define PWM_FREQ  50   // 伺服频率 50Hz
#define PCA9685_ADDR 0x40

// I2C引脚配置
#define I2C_SDA 21
#define I2C_SCL 20

// 舵机配置（注意：0-3是小力360度舵机，4-7是大力180度舵机）
#define NUM_SERVOS_360 4     // 360度舵机数量 (通道0-3，小力舵机)
#define NUM_SERVOS_180 4     // 180度舵机数量 (通道4-7，大力舵机)
#define TOTAL_SERVOS 8       // 总舵机数量
#define SERVO_360_START 0    // 360度舵机起始通道
#define SERVO_180_START 4    // 180度舵机起始通道

// 360度舵机中点偏移量 (针对每个舵机可调整)
#define SERVO_360_CENTER_0 90  // 通道0中点角度
#define SERVO_360_CENTER_1 90  // 通道1中点角度
#define SERVO_360_CENTER_2 90  // 通道2中点角度
#define SERVO_360_CENTER_3 90  // 通道3中点角度
#define SERVO_360_MOVE_ANGLE 10  // 360度舵机移动角度

// 步进电机配置
#define NUM_STEPPERS 5

// 步进电机引脚 - 上下抬起 (4个并联，使用相同引脚)
#define STEPPER_VERTICAL_STEP 1   // 垂直步进电机步进引脚
#define STEPPER_VERTICAL_DIR 2    // 垂直步进电机方向引脚
#define STEPPER_VERTICAL_ENABLE 42  // 垂直步进电机使能引脚

// 步进电机引脚 - 左右移动
#define STEPPER_HORIZONTAL_STEP 41  // 水平步进电机步进引脚
#define STEPPER_HORIZONTAL_DIR 40   // 水平步进电机方向引脚
#define STEPPER_HORIZONTAL_ENABLE 39  // 水平步进电机使能引脚

// 步进电机参数
#define STEPS_PER_REV 200      // 每圈步数 (1.8度步进电机)
#define MICROSTEPS 16          // 细分数
#define DEFAULT_SPEED 1000     // 默认速度 (steps/s)
#define MAX_SPEED 2000         // 最大速度
#define ACCELERATION 500       // 加速度

// Web服务器配置
#define WEB_SERVER_PORT 80
#define WEBSOCKET_PORT 81
#define MAX_LOG_SIZE 2000

// 默认角度 - 开机初始位置（避免碰到地面）
#define DEFAULT_ANGLE_SERVO_4 60    // 大力4初始角度
#define DEFAULT_ANGLE_SERVO_5 145   // 大力5初始角度
#define DEFAULT_ANGLE_SERVO_6 125   // 大力6初始角度
#define DEFAULT_ANGLE_SERVO_7 90    // 大力7初始角度
#define STEP_ANGLE 5                // 舵机步进角度

// 360度舵机动作参数（用于动作4）
#define SERVO_360_ACTION_CENTER 120  // 动作4使用的中点

#endif

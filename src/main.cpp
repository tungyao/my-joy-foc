#include <Arduino.h>
// 开环电机控制实例
#include <SimpleFOC.h>
#include "pin.hpp"

// 无刷直流电机及驱动器实例
// BLDCMotor( pp number极对数 , phase resistance相电阻)
BLDCMotor motor = BLDCMotor(PP , RR, KV); 
BLDCDriver3PWM driver = BLDCDriver3PWM(M0_A, M0_B, M0_C);

// commander实例化
Commander command = Commander(Serial);
void doTarget(char* cmd) { command.scalar(&motor.target, cmd); }
void doLimitCurrent(char* cmd) { command.scalar(&motor.current_limit, cmd); }

void setup() {

  // 配置驱动器
  // 电源电压 [V]
  driver.voltage_power_supply = Vot;
  driver.init();
  // 连接电机和驱动器
  motor.linkDriver(&driver);

  // 限制电机电流 （电阻给定的话）
  motor.current_limit = 0.5;   // [Amps]
 
  // 配置开环控制
  motor.controller = MotionControlType::velocity_openloop;

  // 初始化电机
  motor.init();
  motor.initFOC();

  // 添加目标命令T
  command.add('T', doTarget, "target velocity");
  command.add('C', doLimitCurrent, "current limit");

  Serial.begin(115200);
  Serial.println("Motor ready!");
  Serial.println("Set target velocity [rad/s]");
  _delay(1000);
}

void loop() {
  motor.loopFOC();
  // 开环速度运动
  // 使用电机电压限制和电机速度限制
  motor.move();

  // 用户通信
  command.run();
}

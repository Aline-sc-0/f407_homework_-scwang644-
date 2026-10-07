#ifndef THREADS_HPP
#define THREADS_HPP

#include "tx_api.h"
#include "BMI088.hpp"
#include "M2006.hpp"

// 导出给 C 语言调用的初始化函数（必须加 extern "C"）
extern "C" void threads_init(void);

// 线程入口函数声明（ThreadX 是 C 语言，必须加 extern "C"）
extern "C" void alive_thread_entry(ULONG thread_input);
extern "C" void imu_thread_entry(ULONG thread_input);
extern "C" void control_thread_entry(ULONG thread_input);

// ================= 全局变量声明 =================
extern TX_THREAD alive_thread;
extern TX_THREAD imu_thread;
extern TX_THREAD control_thread;

extern TX_SEMAPHORE init_sem;
extern TX_SEMAPHORE imu_sem;
extern TX_SEMAPHORE control_sem;

// 供 LED 等模块读取的状态标志
extern volatile bool is_init_ok;
extern volatile bool is_imu_alive;
extern volatile bool is_control_alive;

// 全局对象声明（建议放在这里方便其他文件引用）
extern cBMI088 bmi088;
extern M2006 motor1;

#endif // THREADS_HPP
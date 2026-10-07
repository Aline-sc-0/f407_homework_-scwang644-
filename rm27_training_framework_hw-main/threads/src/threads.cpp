#include "threads.hpp"

// ================= 线程栈分配 =================
#define ALIVE_THREAD_STACK_SIZE   1024
#define IMU_THREAD_STACK_SIZE     2048
#define CONTROL_THREAD_STACK_SIZE 1024

// 定义线程控制块
TX_THREAD alive_thread;
TX_THREAD imu_thread;
TX_THREAD control_thread;

// 定义栈空间
static UCHAR alive_stack[ALIVE_THREAD_STACK_SIZE];
static UCHAR imu_stack[IMU_THREAD_STACK_SIZE];
static UCHAR control_stack[CONTROL_THREAD_STACK_SIZE];

// 定义信号量
TX_SEMAPHORE init_sem;
TX_SEMAPHORE imu_sem;
TX_SEMAPHORE control_sem;

// 定义状态变量
volatile bool is_init_ok = false;
volatile bool is_imu_alive = false;
volatile bool is_control_alive = false;

// 定义全局对象
cBMI088 bmi088;
M2006 motor1;

// ================= 初始化函数 =================
extern "C" void threads_init(void)
{
    // 1. 创建信号量 (初始计数为0)
    tx_semaphore_create(&init_sem, "Init Sem", 0);
    tx_semaphore_create(&imu_sem, "IMU Sem", 0);
    tx_semaphore_create(&control_sem, "Control Sem", 0);

    // 2. 创建线程 (参数: 句柄, 名称, 入口函数, 参数, 栈底, 栈大小, 优先级, 抢占阈值, 时间片, 自动启动)
    tx_thread_create(&alive_thread, "Alive Thread", alive_thread_entry, 0,
                     alive_stack, ALIVE_THREAD_STACK_SIZE,
                     15, 15, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&imu_thread, "IMU Thread", imu_thread_entry, 0,
                     imu_stack, IMU_THREAD_STACK_SIZE,
                     5, 5, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&control_thread, "Control Thread", control_thread_entry, 0,
                     control_stack, CONTROL_THREAD_STACK_SIZE,
                     5, 5, TX_NO_TIME_SLICE, TX_AUTO_START);
}
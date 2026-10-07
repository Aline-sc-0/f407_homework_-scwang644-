#include "threads.hpp"
#include "DJIMotorHandler.hpp"
#include "bsp_can.hpp"

extern "C" void control_thread_entry(ULONG thread_input)
{
    // 注册电机到 Handler
    DJIMotorHandler::Instance()->registerMotor(&motor1, &hcan1, 0x201);
    
    motor1.controlMode = DJIMotor::POS_MODE; // 位置环+速度环双环
    motor1.positionSet = 0.0f;

    while (1)
    {
        motor1.setOutput(); // 计算 PID
        DJIMotorHandler::Instance()->sendControlData(); // 发送 CAN 报文

        tx_semaphore_put(&control_sem); // 通知 alive 线程自己存活
        tx_thread_sleep(1); // 1kHz 频率
    }
}
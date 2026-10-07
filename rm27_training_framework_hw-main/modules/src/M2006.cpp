#include "M2006.hpp"

M2006::M2006()
{
    gearBox = GearBox_M2006;
    maxCurrent = 10000; // M2006 最大电流 10A，对应 10000 控制量
    controlMode = RELAX_MODE;
    
    // 初始化 PID 参数 (需实际调参)
    // 速度环: Kp, Ki, Kd, maxOut, maxIOut, mode
    speedPid.Tuning(5.0f, 0.05f, 0.0f); // 仅示例
    speedPid.maxOut = maxCurrent;
    speedPid.maxIOut = 3000.0f;
    
    positionPid.Tuning(10.0f, 0.0f, 0.0f);
    positionPid.maxOut = 500.0f; // 输出作为速度环的输入 (rad/s)
}

void M2006::setOutput()
{
    if (controlMode == RELAX_MODE) {
        currentSet = 0;
        speedPid.Clear();
        positionPid.Clear();
        return;
    }
    else if (controlMode == SPD_MODE) {
        speedPid.ref = speedSet;
        speedPid.fdb = motorFeedback.speedFdb;
        speedPid.UpdateResult();
        currentSet = (int16_t)speedPid.result;
    }
    else if (controlMode == POS_MODE) {
        // 位置环输出作为速度环输入 (串级PID)
        positionPid.ref = positionSet;
        positionPid.fdb = motorFeedback.positionFdb;
        positionPid.UpdateResult();
        
        speedPid.ref = positionPid.result;
        speedPid.fdb = motorFeedback.speedFdb;
        speedPid.UpdateResult();
        currentSet = (int16_t)speedPid.result;
    }

    // 最终电流限幅
    if (currentSet > maxCurrent) currentSet = maxCurrent;
    if (currentSet < -maxCurrent) currentSet = -maxCurrent;
}

M2006::MotorStateTypedef M2006::AliveCheck()
{
    if (AliveFlag == Pre_AliveFlag) {
        MotorState = MOTOR_OFFLINE;
    } else {
        MotorState = MOTOR_ONLINE;
        Pre_AliveFlag = AliveFlag;
    }
    return MotorState;
}
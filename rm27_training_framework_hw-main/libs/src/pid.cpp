#include "pid.hpp"

PID::PID(float kp, float ki, float kd, float maxOut, float maxIOut, int mode)
    : mode(mode), kp(kp), ki(ki), kd(kd), maxOut(maxOut), maxIOut(maxIOut)
{
    Clear();
}

void PID::Tuning(float tuning_kp, float tuning_ki, float tuning_kd)
{
    kp = tuning_kp;
    ki = tuning_ki;
    kd = tuning_kd;
}

void PID::UpdateResult()
{
    err[2] = err[1];
    err[1] = err[0];
    err[0] = ref - fdb;

    // TODO: 按 mode 实现位置式或增量式 PID，并限制积分和最终输出。
    // 完成前始终保持零输出。
    if (mode == PID_POSITION) // 位置式 PID (假设你的头文件中定义的是 PID_POSITION)
    {
        // 1. 计算各项
        pResult = kp * err[0];
        iResult += ki * err[0]; // 位置式积分需要累加
        dResult = kd * (err[0] - err[1]);

        // 2. 积分限幅 (防止积分饱和)
        if (iResult > maxIOut) {
            iResult = maxIOut;
        } else if (iResult < -maxIOut) {
            iResult = -maxIOut;
        }

        // 3. 计算总输出
        result = pResult + iResult + dResult;

        // 4. 输出限幅
        if (result > maxOut) {
            result = maxOut;
        } else if (result < -maxOut) {
            result = -maxOut;
        }
    }
    else if (mode == PID_DELTA) // 增量式 PID (假设你的头文件中定义的是 PID_DELTA)
    {
        // 1. 计算各项增量
        pResult = kp * (err[0] - err[1]);
        iResult = ki * err[0]; // 增量式积分不累加，只计算本次误差对应的增量
        dResult = kd * (err[0] - 2.0f * err[1] + err[2]);

        // 2. 计算本次增量并累加到总输出
        float deltaResult = pResult + iResult + dResult;
        result += deltaResult;

        // 3. 输出限幅
        if (result > maxOut) {
            result = maxOut;
        } else if (result < -maxOut) {
            result = -maxOut;
        }
    }
    else
    {
        // 未知模式，安全兜底，输出归零
        pResult = iResult = dResult = result = 0.0f;
    }
}

void PID::Clear()
{
    ref = fdb = 0.0f;
    err[0] = err[1] = err[2] = 0.0f;
    pResult = iResult = dResult = result = 0.0f;
}

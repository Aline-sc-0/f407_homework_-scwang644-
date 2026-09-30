#include "pid.hpp"
#include <cmath>

static float limit(float value, float maximum) {
    return value > maximum ? maximum : (value < -maximum ? -maximum : value);
}

PID::PID(float p, float i, float d, float out, float integral)
    : kp(p), ki(i), kd(d), maxOut(std::fabs(out)), maxIOut(std::fabs(integral)) {}

PID::PID(const PIDTuning& tuning)
    : PID(tuning.kp, tuning.ki, tuning.kd, tuning.maxOut, tuning.maxIOut) {}

void PID::UpdateResult() {
    //TODO： 1：计算误差 error = ？
float error = ref - fdb ;
    //TODO： 2：计算比例项 pResult = ？
pResult = kp*error;
    //TODO： 3：计算积分项 iResult = ？

iResult = limit(iResult + ki* error , (maxIOut) );
    //TODO： 4：计算微分项 dResult = ？
dResult = kd*(error - last_error);
    //TODO： 5：计算输出 output = ？
float output = pResult+iResult+dResult ;
    if (!std::isfinite(output)) 
    { 
        Clear(); return; 
    }
    result = limit(output, std::fabs(maxOut));
    last_error = error;
}

void PID::Clear() {
    ref = fdb = result = pResult = iResult = dResult = last_error = 0.0f;
}

void PID::Tuning(float p, float i, float d) { kp = p; ki = i; kd = d; }

void PID::Tuning(const PIDTuning& tuning) {
    kp = tuning.kp;
    ki = tuning.ki;
    kd = tuning.kd;
    maxOut = std::fabs(tuning.maxOut);
    maxIOut = std::fabs(tuning.maxIOut);
    iResult = limit(iResult, maxIOut);
    result = limit(result, maxOut);
}

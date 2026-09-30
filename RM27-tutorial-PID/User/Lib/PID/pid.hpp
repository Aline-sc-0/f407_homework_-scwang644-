#pragma once

// PID parameters are independent from controller state, so applications can
// keep and tune configurations separately from PID instances.
struct PIDTuning {
    float kp;
    float ki;
    float kd;
    float maxOut;
    float maxIOut;
};

class PID {
public:
    PID(float kp, float ki, float kd, float maxOut, float maxIOut);
    explicit PID(const PIDTuning& tuning);
    void UpdateResult();
    void Clear();
    void Tuning(float p, float i, float d);
    void Tuning(const PIDTuning& tuning);

    float kp, ki, kd;
    float maxOut, maxIOut;
    float ref = 0.0f;
    float fdb = 0.0f;
    float result = 0.0f;
    float pResult = 0.0f;
    float iResult = 0.0f;
    float dResult = 0.0f;
private:
    float last_error = 0.0f;
};

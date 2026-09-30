# 教师参考答案

学生入口为 `Core/Src/main.cpp`。

TODO 1：配置速度 PID。若启用位置模式，也要配置位置 PID；位置 PID 的输出限幅是速度目标 rad/s，速度 PID 的输出限幅是电机协议原始命令。建议先单独验证速度环，再启用位置环。

TODO 2、3：

```cpp
if (position_mode) {
    position_pid.ref = curve.Value(HAL_GetTick() - start_ms); // 目标位置 rad
    position_pid.fdb = motor.motorFeedback.positionFdb;       // 反馈位置 rad
    position_pid.UpdateResult();
    speed_pid.ref = position_pid.result; // 位置环给出速度目标 rad/s
} else {
    speed_pid.ref = curve.Value(HAL_GetTick() - start_ms); // 目标速度 rad/s
}
speed_pid.fdb = motor.motorFeedback.speedFdb;
speed_pid.UpdateResult();
```

后面已提供：

```cpp
if (!motor.AliveCheck()) { position_pid.Clear(); speed_pid.Clear(); }
motor.currentSet = speed_pid.result;
if (!motor_handler->sendControlData()) { position_pid.Clear(); speed_pid.Clear(); }
HAL_Delay(1);
```

教师选择 `M3508 motor;`、`M2006 motor;` 或 `GM6020 motor;`。编号 1 对应的注册反馈 ID 分别为 0x201、0x201、0x205。速度曲线用输出轴 rad/s；位置曲线用输出轴 rad。位置反馈首帧置零，后续用编码器差分累计并处理 8192 计数回绕。

实验顺序：恒值确认方向 → 阶跃观察响应 → 三角波观察跟随误差。不同型号和负载重新调参。PID 按每次调用计算，不要使用上一版带 dt、转子 rpm 的参数。

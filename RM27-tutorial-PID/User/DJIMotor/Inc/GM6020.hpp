//
// Created by cosmosmount on 2025/8/30.
//

#ifndef RM26_GM6020_HPP
#define RM26_GM6020_HPP

#include "DJIMotor.hpp"

/**
 * @class GM6020
 * @brief GM6020电机的控制类，继承自GMMotor类。
 *
 * 保存 GM6020 电机对应的协议参数。
 * GM6020电机通常用于精确的运动控制和高负载应用。
 */
class GM6020 final: public DJIMotor
{
public:
    /**
     * @brief 构造函数，初始化GM6020电机控制类。
     */
    GM6020();

    /**
     * @brief 构造函数，初始化GM6020电机控制类。
     */
    virtual ~GM6020() = default;

    float Offset;

    void BlockedCheck() override;

    MotorStateTypedef AliveCheck() override;
};

#endif //RM26_GM6020_HPP

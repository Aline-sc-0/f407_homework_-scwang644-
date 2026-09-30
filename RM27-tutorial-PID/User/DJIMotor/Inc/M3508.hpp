//
// Created by cosmosmount on 2025/8/30.
//

#ifndef RM26_M3508_HPP
#define RM26_M3508_HPP

#include "DJIMotor.hpp"

/**
 * @class M3508
 * @brief M3508电机的控制类，继承自Motor类。
 *
 * 保存 M3508 电机对应的协议参数。
 */
class M3508 final : public DJIMotor
{
public:
    /**
     * @brief 构造函数，初始化M3508电机控制类。
     */
    M3508();

    /**
     * @brief 构造函数，初始化M3508电机控制类。
     */
    virtual ~M3508() = default;

    void BlockedCheck() override;

    MotorStateTypedef AliveCheck() override;
};

#endif //RM26_M3508_HPP

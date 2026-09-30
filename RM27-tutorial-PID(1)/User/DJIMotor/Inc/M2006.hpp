//
// Created by cosmosmount on 2025/8/30.
//

#ifndef RM26_M2006_HPP
#define RM26_M2006_HPP

#include "DJIMotor.hpp"

/**
 * @class M2006
 * @brief M2006电机的控制类，继承自Motor类。
 *
 * 保存 M2006 电机对应的协议参数。
 */
class M2006 final : public DJIMotor
{
public:
    /**
     * @brief 构造函数，初始化M2006电机控制类。
     */
    M2006();

    /**
     * @brief 构造函数，初始化M2006电机控制类。
     */
    virtual ~M2006() = default;

    void BlockedCheck() override;

    MotorStateTypedef AliveCheck() override;
};

#endif //RM26_M2006_HPP

#include "M3508.hpp"

M3508::M3508()
{
    maxCurrent = 16384;
    // Feedback conversion in DJIMotorHandler depends on this value.
    gearBox = GearBox_M3508;
}

M3508::MotorStateTypedef M3508::AliveCheck()
{
    if (AliveFlag == Pre_AliveFlag)
    {
        MotorState = MOTOR_OFFLINE;
    }
    else
    {
        Pre_AliveFlag = AliveFlag;
        MotorState = MOTOR_ONLINE;
    }
    return MotorState;
}

void M3508::BlockedCheck()
{
}

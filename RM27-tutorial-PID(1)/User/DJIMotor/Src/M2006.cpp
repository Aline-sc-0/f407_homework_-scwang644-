#include "M2006.hpp"

M2006::M2006()
{
    maxCurrent = 10000;
    // Feedback conversion in DJIMotorHandler depends on this value.
    gearBox = GearBox_M2006;
}

M2006::MotorStateTypedef M2006::AliveCheck()
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

void M2006::BlockedCheck()
{
}

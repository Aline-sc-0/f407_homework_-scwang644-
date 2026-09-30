#include "GM6020.hpp"

GM6020::GM6020()
{
    maxCurrent = 15000;
    Offset = 0.0f;
    // GM6020 feedback is direct drive in this motor library.
    gearBox = GearBox_None;
}

GM6020::MotorStateTypedef GM6020::AliveCheck()
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

void GM6020::BlockedCheck()
{
}

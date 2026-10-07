#include "DJIMotorHandler.hpp"
#include "bsp_can.hpp"

void DJIMotorHandler::registerMotor(DJIMotor *motor, CAN_HandleTypeDef *hcan, uint16_t canId)
{
    if (motor == nullptr || hcan == nullptr) return;
    if (canId < 0x201 || canId > 0x208) return;

    // 计算总线索引 (0为CAN1, 1为CAN2)
    int bus_idx = (hcan == &hcan1) ? 0 : 1;
    // 计算槽位索引 (0x201-0x204 放在0-3，0x205-0x208 放在4-7)
    int slot_idx = (canId <= 0x204) ? (canId - 0x201) : (canId - 0x205 + 4);

    motor->canId = canId;
    motor->hcan = hcan;
    DJIMotorList[bus_idx][slot_idx] = motor;

    // 标记存在标志
    if (canId <= 0x204) {
        if (bus_idx == 0) CAN1_0x200_Exist = true; else CAN2_0x200_Exist = true;
    } else {
        if (bus_idx == 0) CAN1_0x1FF_Exist = true; else CAN2_0x1FF_Exist = true;
    }
        // TODO: 校验 CAN 句柄和 0x201~0x208 ID，再登记到对应总线与槽位。
    // TODO: 按 ID 设置对应的 0x200/0x1FF 报文存在标志。
    (void)motor;
    (void)hcan;
    (void)canId;
}

void DJIMotorHandler::sendControlData()
{
    // 1. 打包 CAN1 0x200 报文 (ID: 0x201-0x204)
    if (CAN1_0x200_Exist) {
        for (int i = 0; i < 4; i++) {
            if (DJIMotorList[0][i] != nullptr) {
                can1_send_data_0[i * 2]     = (DJIMotorList[0][i]->currentSet >> 8) & 0xFF; // 高八位
                can1_send_data_0[i * 2 + 1] = DJIMotorList[0][i]->currentSet & 0xFF;        // 低八位
            }
        }
        CAN_Transmit(&hcan1, 0x200, can1_send_data_0, 8);
    }
    
    // 2. 打包 CAN1 0x1FF 报文 (ID: 0x205-0x208)
    if (CAN1_0x1FF_Exist) {
        for (int i = 0; i < 4; i++) {
            if (DJIMotorList[0][i + 4] != nullptr) {
                can1_send_data_1[i * 2]     = (DJIMotorList[0][i + 4]->currentSet >> 8) & 0xFF;
                can1_send_data_1[i * 2 + 1] = DJIMotorList[0][i + 4]->currentSet & 0xFF;
            }
        }
        CAN_Transmit(&hcan1, 0x1FF, can1_send_data_1, 8);
    }

    // 3. 对 CAN2 同理操作 (根据实际工程板号选择总线，此处略)
    // TODO: 将各电机 currentSet 按大端序放入正确的 8 字节控制帧。
    // TODO: 仅发送包含已注册电机的报文；未完成前不发送 CAN 电流命令。
   
}

void DJIMotorHandler::updateFeedback(CAN_HandleTypeDef *hcan, uint8_t *rx_data, int index)
{
    if (rx_data == nullptr || index < 0 || index > 7) return;
    int bus_idx = (hcan == &hcan1) ? 0 : 1;
    if (DJIMotorList[bus_idx][index] != nullptr) {
        UpdateSensorData(DJIMotorList[bus_idx][index], rx_data);
    }    // TODO: 校验总线、反馈数据和索引，再找到对应电机并调用 UpdateSensorData。
    (void)hcan;
    (void)rx_data;
    (void)index;
}

void DJIMotorHandler::UpdateSensorData(DJIMotor *motor, uint8_t *can_data)
{
    if (motor == nullptr || can_data == nullptr) return;
    
    motor->motorFeedback.last_ecd = motor->motorFeedback.ecd;
    motor->motorFeedback.ecd = (uint16_t)((can_data[0] << 8) | can_data[1]);
    motor->motorFeedback.speed_rpm = (int16_t)((can_data[2] << 8) | can_data[3]);
    int16_t raw_current = (int16_t)((can_data[4] << 8) | can_data[5]);
    motor->motorFeedback.temperatureFdb = (float)can_data[6];

    // 减速比换算
    float ratio = 1.0f;
    if (motor->gearBox == GearBox_M3508) ratio = 3591.0f / 187.0f; // 约19.2
    if (motor->gearBox == GearBox_M2006) ratio = 36.0f;
    
    // 更新状态、速度等（需补充角度回绕和单位换算）
    motor->AliveFlag++;
    // TODO: 从 CAN 数据解析编码器、转速、电流和温度。
    // TODO: 处理编码器回绕，并按减速比换算输出轴位置和角速度。
    (void)motor;
    (void)can_data;
}

void DJIMotorHandler::AllMotorAliveCheck()
{
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            if (DJIMotorList[i][j] != nullptr) {
                DJIMotorList[i][j]->AliveCheck();
            }
        }
    }
    // TODO: 遍历已注册电机并调用 AliveCheck。
}

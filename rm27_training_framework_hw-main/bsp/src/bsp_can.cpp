#include "bsp_can.hpp"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

/**
 * @brief 初始化CAN滤波器配置。
 * 设置CAN硬件的滤波器，用于优化接收数据的处理。
 * 更多信息，请参考原文，链接：https://blog.csdn.net/weixin_54448108/article/details/128570593
 */

// 假设电机反馈数据结构体（参考《教程文档》14.4.3节）
typedef struct
{
    uint16_t ecd;
    int16_t speed_rpm;
    int16_t given_current;
    uint8_t temperate;
    int16_t last_ecd;
} motor_measure_t;

// 外部声明电机数据结构体（根据你的实际工程框架调整）
extern motor_measure_t motor_chassis[4];
extern motor_measure_t motor_gimbal[3];

void CAN_Init(void)
{
    CAN_FilterTypeDef can_filter_st;                   ///< 定义过滤器结构体
    can_filter_st.FilterActivation = ENABLE;           ///< ENABLE使能过滤器
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;  ///< 设置过滤器模式--标识符屏蔽位模式
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT; ///< 过滤器的位宽 32 位
    can_filter_st.FilterIdHigh = 0x0000;               ///< ID高位
    can_filter_st.FilterIdLow = 0x0000;                ///< ID低位
    can_filter_st.FilterMaskIdHigh = 0x0000;           ///< 过滤器掩码高位
    can_filter_st.FilterMaskIdLow = 0x0000;            ///< 过滤器掩码低位

    can_filter_st.FilterBank = 0;                                      ///< 过滤器组-双CAN可指定0~27
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;                 ///< 与过滤器组管理的 FIFO
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);                      ///< HAL库配置过滤器函数
    HAL_CAN_Start(&hcan1);                                             ///< 使能CAN1控制器
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); ///< 使能CAN的各种中断

    can_filter_st.SlaveStartFilterBank = 14;                           ///< 双CAN模式下规定CAN的主从模式的过滤器分配，从过滤器为14
    can_filter_st.FilterBank = 14;                                     ///< 过滤器组-双CAN可指定0~27
    HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);                      ///< HAL库配置过滤器函数
    HAL_CAN_Start(&hcan2);                                             ///< 使能CAN2控制器
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING); ///< 使能CAN的各种中断
}

HAL_StatusTypeDef CAN_Transmit(CAN_HandleTypeDef *hcan, uint32_t Id, uint8_t *msg, uint16_t len)
{
    // 1. 句柄与指针检查（避免未初始化时发送数据）
    if (hcan == NULL || msg == NULL) {
        return HAL_ERROR;
    }
    
    // 检查CAN状态是否处于就绪态（已Start）
    if (hcan->State != HAL_CAN_STATE_READY && hcan->State != HAL_CAN_STATE_LISTENING) {
        return HAL_ERROR; 
    }

    // 2. 数据长度检查（CAN标准帧数据域最大为8字节）
    if (len > 8) {
        return HAL_ERROR;
    }

    // 3. 邮箱检查
    if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {
        // 邮箱已满，直接返回忙碌
        return HAL_BUSY;
    }
    
    // 4. 构造帧头
    CAN_TxHeaderTypeDef tx_header;
    uint32_t send_mail_box;
    
    tx_header.StdId = Id;
    tx_header.IDE = CAN_ID_STD;       // 标准帧
    tx_header.RTR = CAN_RTR_DATA;     // 数据帧
    tx_header.DLC = len;              // 数据长度
    tx_header.TransmitGlobalTime = DISABLE;

    // 5. 调用HAL库发送
    return HAL_CAN_AddTxMessage(hcan, &tx_header, msg, &send_mail_box);
}

// TODO: 实现 CAN 接收回调，根据总线和标准帧 ID 分发电机反馈。
extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data) != HAL_OK) {
        return;
    }

    // 判断是CAN1还是CAN2（根据总线划分功能，或者直接根据ID判断）
    if (hcan == &hcan1) {
        // 根据标准帧ID分发电机反馈
        switch (rx_header.StdId)
        {
            // 底盘电机 M3508 (ID: 0x201 - 0x204)
            case 0x201:
            case 0x202:
            case 0x203:
            case 0x204:
            {
                uint8_t i = rx_header.StdId - 0x201;
                // 数据拼接（参考教程文档14.4.3节）
                motor_chassis[i].last_ecd = motor_chassis[i].ecd;
                motor_chassis[i].ecd = (uint16_t)((rx_data[0] << 8) | rx_data[1]);
                motor_chassis[i].speed_rpm = (int16_t)((rx_data[2] << 8) | rx_data[3]);
                motor_chassis[i].given_current = (int16_t)((rx_data[4] << 8) | rx_data[5]);
                motor_chassis[i].temperate = rx_data[6];
                break;
            }
            // 云台电机 GM6020 或拨弹电机 (ID: 0x205 - 0x208)
            case 0x205:
            case 0x206:
            case 0x207:
            case 0x208:           
            {
                uint8_t i = rx_header.StdId - 0x205;
                motor_gimbal[i].last_ecd = motor_gimbal[i].ecd;
                motor_gimbal[i].ecd = (uint16_t)((rx_data[0] << 8) | rx_data[1]);
                motor_gimbal[i].speed_rpm = (int16_t)((rx_data[2] << 8) | rx_data[3]);
                motor_gimbal[i].given_current = (int16_t)((rx_data[4] << 8) | rx_data[5]);
                motor_gimbal[i].temperate = rx_data[6];
                break;
            }
            default:
                break;
        }
    }
    // else if (hcan == &hcan2) { ... 处理挂在CAN2上的设备 }
}
extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);

    if (rx_header.StdId >= 0x201 && rx_header.StdId <= 0x208) {
        int index = (rx_header.StdId <= 0x204) ? (rx_header.StdId - 0x201) : (rx_header.StdId - 0x205 + 4);
        DJIMotorHandler::Instance()->updateFeedback(hcan, rx_data, index);
    }
}
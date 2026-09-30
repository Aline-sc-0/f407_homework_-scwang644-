#pragma once

#include "can.h"

#ifdef __cplusplus
extern "C" {
#endif
extern volatile uint32_t can1_rx0_irq_count;
extern volatile uint32_t can1_rx0_callback_count;
extern volatile uint32_t can1_rx0_frame_count;
extern volatile uint32_t can2_rx0_irq_count;
extern volatile uint32_t can2_rx0_callback_count;
extern volatile uint32_t can2_rx0_frame_count;
#ifdef __cplusplus
}
#endif

// Configure both CAN ports.
void CAN_Init(void);
void CAN_Transmit(CAN_HandleTypeDef* hcan, uint32_t id, uint8_t* data, uint16_t len);

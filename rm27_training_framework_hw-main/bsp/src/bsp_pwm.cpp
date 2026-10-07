extern "C" {
#include "tim.h"
}
#include "bsp_pwm.hpp"
void PWM_Init(void)
{
    // TODO: 根据实际使用的定时器和通道完成 PWM 初始化。
}

void PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
// 1. 判空检查，防止传入未初始化的句柄
    if (htim == NULL) return;

    // 2. 调用 HAL 库启动 PWM，并检查返回状态（参考《教程文档》4.4.3节）
    if (HAL_TIM_PWM_Start(htim, Channel) != HAL_OK) {
        // 启动失败，可以加入你的日志打印
        // printf("PWM Start Error! Channel: %lu\r\n", Channel);
    }    // TODO: 调用 HAL 启动指定定时器通道的 PWM，并检查返回状态。
}

void PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
if (htim == NULL) return;
    
    // 调用 HAL 库停止 PWM（参考《教程文档》4.4.3节）
    HAL_TIM_PWM_Stop(htim, Channel);    // TODO: 调用 HAL 停止指定定时器通道的 PWM。
}

void PWM_SetPeriod(TIM_HandleTypeDef *htim, float period_s)
{
 if (htim == NULL || period_s <= 0.0f) return;

    // 获取定时器时钟频率（注意：STM32F4 中如果 APB 预分频不为 1，定时器时钟是 APB 时钟的 2 倍）
    // 为了简便，这里假定你已经根据 clock configuration 知道了定时器的输入时钟。
    // 以 TIM1、TIM8（挂载在 APB2）和 TIM4、TIM5（挂载在 APB1）为例：
    uint32_t timer_clock = 0;
    
    // 判断是哪个总线上的定时器 (TIM1, TIM8, TIM9~TIM11 在 APB2；TIM2~TIM7, TIM12~TIM14 在 APB1)
    if (htim->Instance == TIM1 || htim->Instance == TIM8 || 
        htim->Instance == TIM9 || htim->Instance == TIM10 || htim->Instance == TIM11) {
        // APB2 定时器时钟：如果 APB2 预分频系数不为 1，则频率为 PCLK2 * 2；否则等于 PCLK2
        timer_clock = HAL_RCC_GetPCLK2Freq();
        if (HAL_RCC_GetHCLKFreq() != timer_clock) {
            timer_clock *= 2; // 此时等于 168MHz
        }
    } else {
        // APB1 定时器时钟：如果 APB1 预分频系数不为 1，则频率为 PCLK1 * 2；否则等于 PCLK1
        timer_clock = HAL_RCC_GetPCLK1Freq();
        if (HAL_RCC_GetHCLKFreq() != timer_clock) {
            timer_clock *= 2; // 此时等于 84MHz
        }
    }

    // 根据公式计算 ARR 值
    float f = 1.0f / period_s;
    uint32_t arr = (uint32_t)((float)timer_clock / ((float)(htim->Init.Prescaler + 1) * f) - 1.0f);

    // 写入 ARR 寄存器（参考《教程文档》4.4.2节）
    __HAL_TIM_SET_AUTORELOAD(htim, arr);    // TODO: 根据定时器时钟和预分频值，将秒转换为 ARR 并更新周期。
}

void PWM_SetDutyRatio(TIM_HandleTypeDef *htim, float dutyratio, uint32_t channel)
{

    if (htim == NULL || dutyratio < 0.0f || dutyratio > 1.0f) return;

    // 获取当前定时器的自动重装载值 (ARR)
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim);

    // 计算比较寄存器的值
    uint32_t compare = (uint32_t)(dutyratio * (arr + 1));

    // 写入比较寄存器（使用 HAL 宏，本质是对 CCRx 赋值）
    __HAL_TIM_SET_COMPARE(htim, channel, compare);    // TODO: 将 [0, 1] 占空比换算为比较值，写入指定通道。
}

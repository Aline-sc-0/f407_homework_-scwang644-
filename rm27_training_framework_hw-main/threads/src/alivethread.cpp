#include "threads.hpp"
#include "bsp_pwm.hpp" // 假设你的 LED 控制在这里

extern "C" void alive_thread_entry(ULONG thread_input)
{
    while (1)
    {
        if (!is_init_ok) {
            if (tx_semaphore_get(&init_sem, TX_NO_WAIT) == TX_SUCCESS) is_init_ok = true;
        }

        is_imu_alive = (tx_semaphore_get(&imu_sem, TX_NO_WAIT) == TX_SUCCESS);
        is_control_alive = (tx_semaphore_get(&control_sem, TX_NO_WAIT) == TX_SUCCESS);

        // 灯效逻辑
        if (!is_init_ok) {
            PWM_SetDutyRatio(&htim5, (HAL_GetTick() % 1000 < 500) ? 1.0f : 0.0f, TIM_CHANNEL_3); // 红灯闪烁
        } else if (!is_imu_alive || !is_control_alive) {
            PWM_SetDutyRatio(&htim5, (HAL_GetTick() % 200 < 100) ? 1.0f : 0.0f, TIM_CHANNEL_3); // 红灯快闪
        } else {
            PWM_SetDutyRatio(&htim5, 0.5f, TIM_CHANNEL_2); // 绿灯常亮
        }

        tx_thread_sleep(100);
    }
}
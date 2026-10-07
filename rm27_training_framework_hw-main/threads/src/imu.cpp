#include "threads.hpp"

extern "C" void imu_thread_entry(ULONG thread_input)
{
    bmi088.Config();
    bmi088.VerifyAccChipID();
    bmi088.VerifyGyroChipID();

    if (!bmi088.self_test.ACC_CHIP_ID_ERR && !bmi088.self_test.GYRO_CHIP_ID_ERR) {
        tx_semaphore_put(&init_sem); // 初始化成功
    }

    bmi088.Calibrate(); // 陀螺仪零漂标定

    while (1)
    {
        bmi088.ReadAccData(&bmi088.acc_data);
        bmi088.ReadGyroData(&bmi088.gyro_data);
        bmi088.ReadAccTemperature(&bmi088.acc_data.temperature);

        // TODO: 后期可以在这里加入 Mahony 姿态解算

        tx_semaphore_put(&imu_sem); // 通知 alive 线程自己存活
        tx_thread_sleep(1); // 1kHz 频率
    }
}
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DJIMotorHandler.hpp"
#include "bsp_can.hpp"
#include "M3508.hpp"
#include "M2006.hpp"
#include "GM6020.hpp"
#include "pid.hpp"
#include "ref.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//创建电机handler单例对象
DJIMotorHandler* motor_handler = DJIMotorHandler::Instance();
//TODO? 创建电机实例：选择你使用的电机
M2006 motor;  // 编号 1 的反馈 ID 为 0x201
// M3508 motor;      // 编号 1 的反馈 ID 为 0x201
// GM6020 motor; // 编号 1 的反馈 ID 为 0x205

//TODO? 创建参考曲线对象，选择你想要的曲线类型
RefCurve curve{RefShape::Triangle, -3.0f, 3.0f, 0, 3000};
// 上面依次为：曲线类型(Constant/Step/Triangle)、低值、高值、阶跃延时 ms、三角波周期 ms。

// PID 参数与控制器实例分离，便于调试时整体调整每组参数。
PIDTuning speed_pid_tuning{600.0f, 40.0f, 60.0f, 6000.0f, 600.0f};

struct debug_data_t {
    float ref;
    float fdb;
    float result;
};

debug_data_t debug_spd_data{};
debug_data_t debug_pos_data{};


// 记录程序开始时间，计算参考曲线的相对时间
uint32_t start_ms = 0;


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  /* USER CODE BEGIN 2 */

  // 注册电机
  motor_handler->registerMotor(&motor, &hcan1, 0x202);
  // 注册的是反馈报文 ID：M3508/M2006 编号 1 用 0x201；GM6020 编号 1 用 0x205。

  // PID 控制器实例
  PID speed_pid(speed_pid_tuning);


  CAN_Init();
  start_ms = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // 每轮应用参数结构体，调试器调整 tuning 后会更新到控制器。
    speed_pid.Tuning(speed_pid_tuning);

    //将PID的reference设置为参考曲线（curve）的值
    //将PID的feedback设置为电机的速度反馈
    //调用 PID 对象自己的 UpdateResult()，计算结果
    //将 PID 的结果写入电机的 currentSet，再由 Handler 统一打包发送。
    speed_pid.ref = curve.Value(HAL_GetTick() - start_ms);
    speed_pid.fdb = motor.motorFeedback.speedFdb;
    speed_pid.UpdateResult();
    motor.currentSet = speed_pid.result;
    motor_handler->sendControlData();

    //更新调试数据
    debug_spd_data.ref = speed_pid.ref;
    debug_spd_data.fdb = speed_pid.fdb;
    debug_spd_data.result = speed_pid.result;

    HAL_Delay(1);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  // Match RM26_F4: 12 MHz HSE / 6 * 168 / 2 = 168 MHz SYSCLK,
  // and PLLQ = 7 provides the 48 MHz clock domain for USB/SDIO/RNG.
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

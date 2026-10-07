#include "bsp_usart.hpp"

// ==================== 【新增：外部声明】 ====================
// 告诉 C++ 编译器，这几个变量是在 C 文件（usart.c 等）里定义的，去别的地方找
extern "C" {
    extern UART_HandleTypeDef huart1;  // 使用 CubeMX 生成的 USART1 句柄
    extern UART_HandleTypeDef huart6;  // 使用 CubeMX 生成的 USART6 句柄
    extern DMA_HandleTypeDef hdma_usart1_rx; // 中断处理函数中计算数据长度时会用到
    extern DMA_HandleTypeDef hdma_usart6_rx; 
}

// ==================== 【新增：全局变量定义】 ====================
// 为 DMA 接收分配内存空间（必须在这里真正定义，才能分配物理内存）
uint8_t usart1_rx_buf[128];  // USART1 接收缓冲区
uint8_t usart6_rx_buf[256];  // USART6 接收缓冲区，裁判系统数据帧较长，建议给大一点

void USART_Init(void)
{
  USART1_Init();
  USART6_Init();
}

void USART6_Init()
{
  // 1. 开启空闲中断，用于裁判系统或遥控器的不定长数据接收
  // 参考《教程文档》第9.4.3节对 USART3 的配置，此处替换为 USART6
  __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);

  // 2. 启动 DMA 接收
  USART_Receive(&huart6, usart6_rx_buf, sizeof(usart6_rx_buf));
    // TODO: 配置 USART6 的 DMA 接收、发送及空闲中断，并启动接收。
};

void USART1_Init()
{
  // 1. 开启空闲中断 (IDLE)，用于处理DMA接收不定长数据的关键
  // 参考《教程文档》第8.4.2节：串口空闲中断
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

  // 2. 启动 DMA 接收，并传入缓冲区
  // 如果是用于 printf 打印，通常只会用到 DMA 发送。
  // 如果需要接收 PC 端指令，则启动 DMA 接收。
  USART_Receive(&huart1, usart1_rx_buf, sizeof(usart1_rx_buf));

    // TODO: 配置 USART1 的 DMA 接收、发送及空闲中断，并启动接收。
};

void USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, enum USART_Mode mode)
{
  // 1. 参数检查（避免空指针传入）
  if (huart == NULL || pData == NULL || Size == 0) {
    return;
  }

  // 2. 根据模式选择发送方式
  switch (mode)
  {
    case USART_MODE_BLOCK:
      // 阻塞发送，超时时间设为 HAL_MAX_DELAY（参考《教程文档》第8.4.3节）
      HAL_UART_Transmit(huart, pData, Size, HAL_MAX_DELAY);
      break;

    case USART_MODE_IT:
      // 中断发送，需要开启对应的 NVIC 中断
      HAL_UART_Transmit_IT(huart, pData, Size);
      break;

    case USART_MODE_DMA:
      // DMA发送，参考《教程文档》第9.4.2节：printf函数实现过程
      HAL_UART_Transmit_DMA(huart, pData, Size);
      break;

    default:
      break;
  }
}
void USART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  if (huart == NULL || pData == NULL || Size == 0) {
    return;
  }

  // 启动DMA接收。
  // 注意：使用 DMA + 空闲中断 (IDLE) 处理不定长数据时，
  // 通常使用 HAL_UART_Receive_DMA 启动一次长接收，直到 IDLE 中断触发时再重新启动
  // 参考《教程文档》第9.4.3节中对 USART3 接收遥控器数据的处理。
  HAL_UART_Receive_DMA(huart, pData, Size);
  // TODO: 选择接收方式并启动 HAL UART 接收。
}

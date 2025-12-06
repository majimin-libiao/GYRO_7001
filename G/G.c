// 文件：FreeRTOS线程创建示例
// 作者：MCD Application Team
// 版本：V1.2.2
// 日期：25-May-2015
// 说明：FreeRTOS双线程LED控制主程序
//
// 版权所有 (c) 2015 STMicroelectronics
// 基于 MCD-ST Liberty SW License Agreement V2 许可协议

/* Includes ------------------------------------------------------------------*/
#include <stm32f1xx_hal.h>
#include <../CMSIS_RTOS/cmsis_os.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
osThreadId LEDThread1Handle, LEDThread2Handle;  // LED线程句柄

/* Private function prototypes -----------------------------------------------*/
static void LED_Thread1(void const *argument);  // LED1线程函数声明
static void LED_Thread2(void const *argument);  // LED2线程函数声明

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
	HAL_Init();  
	
	__GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_9 | GPIO_PIN_9;

	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Thread 1 definition */
	osThreadDef(LED1, LED_Thread1, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  
	 /*  Thread 2 definition */
	osThreadDef(LED2, LED_Thread2, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  
	/* Start thread 1 */
	LEDThread1Handle = osThreadCreate(osThread(LED1), NULL);
  
	/* Start thread 2 */
	LEDThread2Handle = osThreadCreate(osThread(LED2), NULL);
  
	/* Start scheduler */
	osKernelStart();

	  /* We should never get here as control is now taken by the scheduler */
	for (;;)
		;
}

void SysTick_Handler(void)
{ // SysTick中断处理函数
	HAL_IncTick();                                  // HAL时基递增
	osSystickHandler();                             // FreeRTOS系统节拍处理
}

/**
  * @brief  Toggle LED1
  * @param  thread not used
  * @retval None
  */
static void LED_Thread1(void const *argument)
{
	(void) argument;
  
	for (;;)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
		osDelay(2000);
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
		osThreadSuspend(LEDThread2Handle);
		osDelay(2000);
		
		osThreadResume(LEDThread2Handle);
	}
}

static void LED_Thread2(void const *argument)
{ // LED2控制线程
	uint32_t count;                                 // 计数变量（未使用）
	(void) argument;                                // 未使用参数
  
	for (;;)
	{ // 主循环
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);      // 翻转LED2状态
		osDelay(200);                               // 延时200ms
	}
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ // 断言失败处理函数，报告出错的文件名和行号
	// 用户可自行实现错误报告，例如：
	// printf("参数错误: 文件 %s 第 %d 行\r\n", file, line)

	while (1)
	{ // 死循环
	}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

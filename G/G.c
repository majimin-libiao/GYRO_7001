// FreeRTOS双线程LED控制示例
// 版权所有 (c) 2015 STMicroelectronics
// 基于 MCD-ST Liberty SW License Agreement V2 许可协议

#include <stm32f1xx_hal.h>
#include <../CMSIS_RTOS/cmsis_os.h>

osThreadId LEDThread1Handle, LEDThread2Handle;  // LED线程句柄

static void LED_Thread1(void const *argument);  // LED1线程函数声明
static void LED_Thread2(void const *argument);  // LED2线程函数声明

/* Private functions ---------------------------------------------------------*/

int main(void)
{ // 主程序入口
	HAL_Init();                       // 初始化HAL库
	
	// 配置系统时钟：外部振荡器8MHz，CPU时钟64MHz
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};  // 振荡器配置结构体
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};  // 时钟配置结构体
	
	// 配置外部振荡器（HSE）8MHz
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE; // 使用外部高速振荡器
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;                   // 启用外部振荡器
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;               // 启用PLL
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;       // PLL源为外部振荡器
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;               // PLL倍频因子8（8MHz × 8 = 64MHz）
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)       // 应用振荡器配置
	{ // 振荡器配置失败
		while (1)
		{ // 死循环
		}
	}
	
	// 配置系统时钟源和分频器
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;  // 系统时钟源为PLL输出
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;          // AHB不分频（HCLK = 64MHz）
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;           // APB1分频系数为2（PCLK1 = 32MHz）
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;           // APB2不分频（PCLK2 = 64MHz）
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) // 应用时钟配置
	{ // 时钟配置失败
		while (1)
		{ // 死循环
		}
	}
	
	__GPIOB_CLK_ENABLE();             // 使能GPIOB时钟
	GPIO_InitTypeDef GPIO_InitStructure;  // GPIO初始化结构体

	GPIO_InitStructure.Pin = GPIO_PIN_9; // 配置引脚9
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;     // 推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;   // 高频率
	GPIO_InitStructure.Pull = GPIO_NOPULL;             // 无上下拉
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);         // 初始化GPIO

	osThreadDef(LED1, LED_Thread1, osPriorityNormal, 0, configMINIMAL_STACK_SIZE); // LED1线程定义
	osThreadDef(LED2, LED_Thread2, osPriorityNormal, 0, configMINIMAL_STACK_SIZE); // LED2线程定义

	LEDThread1Handle = osThreadCreate(osThread(LED1), NULL); // 创建LED1线程
	LEDThread2Handle = osThreadCreate(osThread(LED2), NULL); // 创建LED2线程

	osKernelStart();                  // 启动FreeRTOS调度器

	for (;;)                          // 正常情况不会执行到这里
	{ // 死循环
	}
}

void SysTick_Handler(void)
{ // SysTick中断处理函数
	HAL_IncTick();                    // HAL时基递增
	osSystickHandler();               // FreeRTOS系统节拍处理
}

static void LED_Thread1(void const *argument)
{ // 任务1：LED50ms闪烁
	(void) argument;                  // 未使用参数
  
	for (;;)
	{ // 主循环
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);  // LED点亮
		osDelay(25);                  // 延时25ms
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET); // LED熄灭
		osDelay(25);                  // 延时25ms（总周期50ms）
	}
}

static void LED_Thread2(void const *argument)
{ // 任务2：10ms间隔休眠任务
	(void) argument;                  // 未使用参数

	for (;;)
	{ // 主循环
		osDelay(10);                  // 延时10ms
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ // 断言失败处理函数
	// 用户可添加错误报告代码
	// 例如: printf("参数错误: 文件 %s 第 %d 行\r\n", file, line)

	while (1)
	{ // 死循环
	}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

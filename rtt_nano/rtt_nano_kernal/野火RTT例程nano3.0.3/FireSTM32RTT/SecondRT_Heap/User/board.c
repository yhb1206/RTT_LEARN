/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 */
#include "board.h"
#include <rthw.h>
#include <rtthread.h>


#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 1024
/* 从内部 SRAM（即 DTCM）里面分配一部分静态内存来作为 RT-Thread 的堆空间，这里配置为 4KB */
static uint32_t rt_heap[RT_HEAP_SIZE];	// heap default size: 4K(1024*4)
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{	
    	/*根据时钟频率进行设置systick，保证适应任何系统时钟 
	 *SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
    RCC_ClocksTypeDef  RccClocks;
    uint32_t SystemClockFrequency = 0, u32Cnt = 0;
    RCC_GetClocksFreq(&RccClocks);
    SystemClockFrequency = RccClocks.HCLK_Frequency;
    u32Cnt = SystemClockFrequency/RT_TICK_PER_SECOND;
    while (SysTick_Config(u32Cnt));
    
	/*硬件 BSP 初始化统统放在这里，比如 LED，串口， LCD 等*/

    /* 初始化开发板的 LED */
    LED_GPIO_Config();

    /* 初始化开发板的串口 */
    USART_Config();

#if 0/*硬件初始化是否成功测试区域*/    
    LED1_ON;
    while (1);
#endif /* if 0. 2018-9-14 22:47:26 Yun huibin */
    
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    
#if defined(RT_USING_CONSOLE) && defined(RT_USING_DEVICE)
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

void SysTick_Handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}

/**
* @brief 重映射串口 DEBUG_USARTx 到 rt_kprintf()函数
* Note： DEBUG_USARTx 是在 bsp_usart.h 中定义的宏，默认使用串口 1
* @param str：要输出到串口的字符串
* @retval 无
*
* @attention
*
*/
 void rt_hw_console_output(const char *str)
 {
    /* 进入临界段 */
    rt_enter_critical();

    /* 直到字符串结束 */
    while (*str!='\0')
    {
        /* 换行 */
        if (*str=='\n')
        {
            USART_SendData(DEBUG_USARTx, '\r');
            while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
        }

        USART_SendData(DEBUG_USARTx, *str++);
        while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
    }

    /* 退出临界段 */
    rt_exit_critical();
}



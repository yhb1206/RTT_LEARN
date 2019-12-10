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
/* ���ڲ� SRAM���� DTCM���������һ���־�̬�ڴ�����Ϊ RT-Thread �Ķѿռ䣬��������Ϊ 4KB */
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
    	/*����ʱ��Ƶ�ʽ�������systick����֤��Ӧ�κ�ϵͳʱ�� 
	 *SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
    RCC_ClocksTypeDef  RccClocks;
    uint32_t SystemClockFrequency = 0, u32Cnt = 0;
    RCC_GetClocksFreq(&RccClocks);
    SystemClockFrequency = RccClocks.HCLK_Frequency;
    u32Cnt = SystemClockFrequency/RT_TICK_PER_SECOND;
    while (SysTick_Config(u32Cnt));
    
	/*Ӳ�� BSP ��ʼ��ͳͳ����������� LED�����ڣ� LCD ��*/

    /* ��ʼ��������� LED */
    LED_GPIO_Config();

    /* ��ʼ��������Ĵ��� */
    USART_Config();

#if 0/*Ӳ����ʼ���Ƿ�ɹ���������*/    
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
* @brief ��ӳ�䴮�� DEBUG_USARTx �� rt_kprintf()����
* Note�� DEBUG_USARTx ���� bsp_usart.h �ж���ĺ꣬Ĭ��ʹ�ô��� 1
* @param str��Ҫ��������ڵ��ַ���
* @retval ��
*
* @attention
*
*/
 void rt_hw_console_output(const char *str)
 {
    /* �����ٽ�� */
    rt_enter_critical();

    /* ֱ���ַ������� */
    while (*str!='\0')
    {
        /* ���� */
        if (*str=='\n')
        {
            USART_SendData(DEBUG_USARTx, '\r');
            while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
        }

        USART_SendData(DEBUG_USARTx, *str++);
        while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
    }

    /* �˳��ٽ�� */
    rt_exit_critical();
}



/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include <rtthread.h>
#include <board.h>
#include <drv_clock.h>

/**
 *  HAL adaptation
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    /* Return function status */
    return HAL_OK;
}

uint32_t HAL_GetTick(void)
{
    return rt_tick_get() * 1000 / RT_TICK_PER_SECOND;
}

void HAL_SuspendTick(void)
{
    return ;
}

void HAL_ResumeTick(void)
{
    return ;
}

void HAL_Delay(__IO uint32_t Delay)
{
    return ;
}

/*===================================================*/
static UART_HandleTypeDef UartHandle;

void bsp_uart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_9;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    UartHandle.Instance = USART1;
    UartHandle.Init.BaudRate   = 115200;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;

    if (HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        RT_ASSERT(0);
    }
}

void bsp_uart_send(char c)
{
    while ((__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TXE) == RESET));
    UartHandle.Instance->TDR = c;
}

void bsp_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOE_CLK_ENABLE();

    /* Configure GPIO_InitStructure */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void bsp_led_on(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
}

void bsp_led_off(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
}

/*===================================================*/

void rt_hw_console_output(const char *str)
{
    RT_ASSERT(str != RT_NULL);

    while (*str != '\0')
    {
        if (*str == '\n')
        {
            bsp_uart_send('\r');
        }
        bsp_uart_send(*str++);
    }
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
    static uint8_t heap_buf[10 * 1024];

    /* HAL_Init() function is called at the beginning of program after reset and before
     * the clock configuration.
     */
    HAL_Init();

    /* Clock Config:
     *     System Clock : 80M
     *     HCLK         : 80M
     *     PCLK1        : 80M
     *     PCLK2        : 80M
     *     SDMMC1       : 48M
     *     USART1       : PCLK2
     */
    SystemClock_Config();

    /* Configure the Systick interrupt time */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / RT_TICK_PER_SECOND);
    /* Configure the Systick */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    bsp_led_init();
    bsp_uart_init();

    rt_system_heap_init(heap_buf, heap_buf + sizeof(heap_buf) - 1);
}

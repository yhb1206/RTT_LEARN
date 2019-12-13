/* 开发板硬件相关头文件 */
#include "board.h" 

/* RT-Thread相关头文件 */
#include "rthw.h"
#include "rtthread.h"


#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 30*256
/* 从内部SRAM里面分配一部分静态内存来作为rtt的堆空间，这里配置为16KB */
static uint32_t rt_heap[RT_HEAP_SIZE];
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

void SysTick_Init(void);

/**
  * @brief  开发板硬件初始化函数
  * @param  无
  * @retval 无
  *
  * @attention
  * RTT把开发板相关的初始化函数统一放到board.c文件中实现，
  * 当然，你想把这些函数统一放到main.c文件也是可以的。
  */
void rt_hw_board_init()
{
    
    /* 初始化SysTick */
    SysTick_Init();

    /*动态内存堆初始化*/
    #if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
    #endif
    
/* =============*硬件BSP初始化统统放在这里，比如LED，串口，LCD等 =============*/
    
    /* 初始化开发板的LED */
    #ifdef BSP_USING_LED
	LED_GPIO_Config();
    #endif
	
    /* 初始化开发板的串口 */
	USART_Config();

    /* 按键初始化 */
	#ifdef BSP_USING_KEY
	Key_GPIO_Config();
	#endif

    /* 调用组件初始化函数 (use INIT_BOARD_EXPORT()) */
    #ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
    #endif
        
}

/**
  * @brief  SysTick中断服务函数
  * @param  无
  * @retval 无
  *
  * @attention
  * SysTick中断服务函数在固件库文件stm32f10x_it.c中也定义了，而现在
  * 在board.c中又定义一次，那么编译的时候会出现重复定义的错误，解决
  * 方法是可以把stm32f10x_it.c中的注释或者删除即可。
  */
void SysTick_Handler(void)
{
    /* 进入中断 */
    rt_interrupt_enter();

    /* 更新时基 */
    rt_tick_increase();

    /* 离开中断 */
    rt_interrupt_leave();
}


/**
  * @brief  启动系统滴答定时器 SysTick
  * @param  无
  * @retval 无
  */
void SysTick_Init(void)
{
	   	/* systick中断自适应
	 *SystemFrequency / 1000       1ms中断1次
	 * SystemFrequency / 100000	 10us中断1次
	 * SystemFrequency / 1000000   1us中断1次
	 */
    RCC_ClocksTypeDef  RccClocks;
    uint32_t SystemClockFrequency = 0;
    RCC_GetClocksFreq(&RccClocks);
    SystemClockFrequency = RccClocks.HCLK_Frequency;
    while (SysTick_Config(SystemClockFrequency/RT_TICK_PER_SECOND));
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






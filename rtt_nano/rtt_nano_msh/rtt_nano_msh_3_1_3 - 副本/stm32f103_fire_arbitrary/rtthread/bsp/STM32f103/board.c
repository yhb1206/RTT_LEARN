/* 开发板硬件相关头文件 */
#include "board.h" 

/* RT-Thread相关头文件 */
#include <rthw.h>
#include <rtthread.h>
#include "usart.h"



#ifdef RT_USING_FINSH 

/*finsh所用接受环形buffer区*/
extern struct rt_serial_rx_fifo* rx_fifo;

void buffer_init(void);

#endif


#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 16*256
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
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND );	
    
	/* 硬件BSP初始化统统放在这里，比如LED，串口，LCD等 */
    
/* 初始化开发板的LED */
#ifdef BSP_USING_LED
	LED_GPIO_Config();
#endif
	
/* 初始化开发板的串口 */
	USART_Config();

#ifdef RT_USING_FINSH 
    /*finsh所用中断接受回环区初始化*/
    buffer_init();
#endif

    /* 按键初始化 */
	#ifdef BSP_USING_KEY
	Key_GPIO_Config();
	#endif
	
/* 调用组件初始化函数 (use INIT_BOARD_EXPORT()) */
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



#ifndef RT_USING_FINSH 

/**
  * @brief  重映射串口DEBUG_USARTx到rt_kprintf()函数
  *   Note：DEBUG_USARTx是在bsp_usart.h中定义的宏，默认使用串口1
  * @param  str：要输出到串口的字符串
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
#endif

#ifdef RT_USING_FINSH 

/*回环缓冲区初始化*/
void buffer_init(void)
{
    rx_fifo = (struct rt_serial_rx_fifo*) rt_malloc (sizeof(struct rt_serial_rx_fifo) +
        RT_SERIAL_RB_BUFSZ);
    RT_ASSERT(rx_fifo != RT_NULL);
    rx_fifo->buffer = (rt_uint8_t*) (rx_fifo + 1);
    rt_memset(rx_fifo->buffer, 0, RT_SERIAL_RB_BUFSZ);
    rx_fifo->put_index = 0;
    rx_fifo->get_index = 0;
    rx_fifo->is_full = RT_FALSE;
   
}


/*==============3.nano finsh移植：finsh线程中断发送环形buffer数据实现部分=================*/

static int stm32_putc(USART_TypeDef *uart_device, char c)
{

    RT_ASSERT(uart_device != RT_NULL);

    USART_ClearFlag(uart_device,USART_FLAG_TC);
    uart_device->DR = c;
    while (!(uart_device->SR & USART_FLAG_TC));

    return 1;
}

int poll_tx(USART_TypeDef *uart_device, const rt_uint8_t *data, int length)
{
    int size;
    RT_ASSERT(uart_device != RT_NULL);

    size = length;
    while (length)
    {
        /*
         * to be polite with serial console add a line feed
         * to the carriage return character
         */
//        if (*data == '\n' && (serial->parent.open_flag & RT_DEVICE_FLAG_STREAM))
        if (*data == '\n')
        {
            stm32_putc(uart_device, '\r');
            
        }

        stm32_putc(uart_device, *data);

        ++ data;
        -- length;
    }

    return size - length;
}

void  rt_hw_finsh_output(const void *buffer, rt_size_t size)
{	

    poll_tx(DEBUG_USARTx, buffer, size);
        
}

#endif






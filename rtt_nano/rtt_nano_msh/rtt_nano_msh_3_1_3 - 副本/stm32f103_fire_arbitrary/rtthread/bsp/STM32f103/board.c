/* ������Ӳ�����ͷ�ļ� */
#include "board.h" 

/* RT-Thread���ͷ�ļ� */
#include <rthw.h>
#include <rtthread.h>
#include "usart.h"



#ifdef RT_USING_FINSH 

/*finsh���ý��ܻ���buffer��*/
extern struct rt_serial_rx_fifo* rx_fifo;

void buffer_init(void);

#endif


#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 16*256
/* ���ڲ�SRAM�������һ���־�̬�ڴ�����Ϊrtt�Ķѿռ䣬��������Ϊ16KB */
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
  * @brief  ������Ӳ����ʼ������
  * @param  ��
  * @retval ��
  *
  * @attention
  * RTT�ѿ�������صĳ�ʼ������ͳһ�ŵ�board.c�ļ���ʵ�֣�
  * ��Ȼ���������Щ����ͳһ�ŵ�main.c�ļ�Ҳ�ǿ��Եġ�
  */
void rt_hw_board_init()
{
    /* ��ʼ��SysTick */
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND );	
    
	/* Ӳ��BSP��ʼ��ͳͳ�����������LED�����ڣ�LCD�� */
    
/* ��ʼ���������LED */
#ifdef BSP_USING_LED
	LED_GPIO_Config();
#endif
	
/* ��ʼ��������Ĵ��� */
	USART_Config();

#ifdef RT_USING_FINSH 
    /*finsh�����жϽ��ܻػ�����ʼ��*/
    buffer_init();
#endif

    /* ������ʼ�� */
	#ifdef BSP_USING_KEY
	Key_GPIO_Config();
	#endif
	
/* ���������ʼ������ (use INIT_BOARD_EXPORT()) */
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
  * @brief  SysTick�жϷ�����
  * @param  ��
  * @retval ��
  *
  * @attention
  * SysTick�жϷ������ڹ̼����ļ�stm32f10x_it.c��Ҳ�����ˣ�������
  * ��board.c���ֶ���һ�Σ���ô�����ʱ�������ظ�����Ĵ��󣬽��
  * �����ǿ��԰�stm32f10x_it.c�е�ע�ͻ���ɾ�����ɡ�
  */
void SysTick_Handler(void)
{
    /* �����ж� */
    rt_interrupt_enter();

    /* ����ʱ�� */
    rt_tick_increase();

    /* �뿪�ж� */
    rt_interrupt_leave();
}



#ifndef RT_USING_FINSH 

/**
  * @brief  ��ӳ�䴮��DEBUG_USARTx��rt_kprintf()����
  *   Note��DEBUG_USARTx����bsp_usart.h�ж���ĺ꣬Ĭ��ʹ�ô���1
  * @param  str��Ҫ��������ڵ��ַ���
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
#endif

#ifdef RT_USING_FINSH 

/*�ػ���������ʼ��*/
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


/*==============3.nano finsh��ֲ��finsh�߳��жϷ��ͻ���buffer����ʵ�ֲ���=================*/

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






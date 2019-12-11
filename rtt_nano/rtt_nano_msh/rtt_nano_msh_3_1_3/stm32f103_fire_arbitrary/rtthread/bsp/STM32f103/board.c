/* ������Ӳ�����ͷ�ļ� */
#include "board.h" 

/* RT-Thread���ͷ�ļ� */
#include "rthw.h"
#include "rtthread.h"
#include "shell.h"



#ifdef RT_USING_FINSH 
/*
 * �ٺ٣���rtt��serial.h�еĶ����������������nano�ò���serial���ٺ٣�
 */
struct rt_serial_rx_fifo
{
    /* software fifo */
    rt_uint8_t *buffer;

    rt_uint16_t put_index, get_index;

    rt_bool_t is_full;
};

/*msh���ô��ڽ��ܻ���buffer��*/
struct rt_serial_rx_fifo *rx_fifo;

void buffer_init(void);

#endif


#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 30*256
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


    #if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
    #endif
    
	/* Ӳ��BSP��ʼ��ͳͳ�����������LED�����ڣ�LCD�� */
    
/* ��ʼ���������LED */
#ifdef BSP_USING_LED
	LED_GPIO_Config();
#endif
	
/* ��ʼ��������Ĵ��� */
	USART_Config();

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
    
#ifdef RT_USING_FINSH 
    /*finsh�����жϽ��ܻػ�����ʼ��*/
    buffer_init();
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

/*==============1.nano msh��ֲ�������жϽ���ʵ�ֲ���=================*/

static int stm32_getc(USART_TypeDef *uart_device)
{
    int ch;

    RT_ASSERT(uart_device != RT_NULL);

    ch = -1;
    if (uart_device->SR & USART_FLAG_RXNE)
    {
        ch = uart_device->DR & 0xff;
    }

    return ch;
}

/* ISR for serial interrupt */
void rt_hw_serial_isr(void)
{
    int ch = -1;
    rt_base_t level;
    rt_size_t rx_length;

    /* interrupt mode receive */
    RT_ASSERT(rx_fifo != RT_NULL);

    while (1)
    {
        ch = stm32_getc(DEBUG_USARTx);
        if (ch == -1) break;


        /* disable interrupt */
        level = rt_hw_interrupt_disable();

        rx_fifo->buffer[rx_fifo->put_index] = ch;
        rx_fifo->put_index += 1;
        if (rx_fifo->put_index >= RT_SERIAL_RB_BUFSZ) rx_fifo->put_index = 0;

        /* if the next position is read index, discard this 'read char' */
        if (rx_fifo->put_index == rx_fifo->get_index)
        {
            rx_fifo->get_index += 1;
            rx_fifo->is_full = RT_TRUE;
            if (rx_fifo->get_index >= RT_SERIAL_RB_BUFSZ) rx_fifo->get_index = 0;
        }

        /* enable interrupt */
        rt_hw_interrupt_enable(level);
    }

    /* get rx length */
    level = rt_hw_interrupt_disable();
    rx_length = (rx_fifo->put_index >= rx_fifo->get_index)? (rx_fifo->put_index - rx_fifo->get_index):
        (RT_SERIAL_RB_BUFSZ - (rx_fifo->get_index - rx_fifo->put_index));
    rt_hw_interrupt_enable(level);

    if (rx_length)
    {
        RT_ASSERT(shell != RT_NULL);
        
        /* release semaphore to let finsh thread rx data */
        rt_sem_release(&shell->rx_sem);
    }
}

/**
 * Uart common interrupt process. This need add to uart ISR.
 *
 * @param serial serial device
 */
static void uart_isr(void) 
{
    /*�������ݱ�־*/
    if(USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE) != RESET)
    {
        /*�Ƿ�����������Ҫ*/
        if(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_PE) == RESET)
        {
            rt_hw_serial_isr();
        }
        /* clear interrupt */
        USART_ClearITPendingBit(DEBUG_USARTx, USART_IT_RXNE);
    }
    
    /*�����ɶ��־�����ˣ���*/
    if (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_ORE) == SET)
    {
        USART_ReceiveData(DEBUG_USARTx);
    }
}


void DEBUG_USART_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr();

    /* leave interrupt */
    rt_interrupt_leave();
}

/*==============2.nano finsh��ֲ��finsh�߳��ж�ȡ����buffer����ʵ�ֲ���=================*/

/*
 * Serial interrupt routines
 */
static int int_finsh_rx(rt_uint8_t *data, int length)
{
    int size;

    size = length;

    RT_ASSERT(rx_fifo != RT_NULL);

    /* read from software FIFO */
    while (length)
    {
        int ch;
        rt_base_t level;

        /* disable interrupt */
        level = rt_hw_interrupt_disable();

        /* there's no data: */
        if ((rx_fifo->get_index == rx_fifo->put_index) && (rx_fifo->is_full == RT_FALSE))
        {
            /* no data, enable interrupt and break out */
            rt_hw_interrupt_enable(level);
            break;
        }

        /* otherwise there's the data: */
        ch = rx_fifo->buffer[rx_fifo->get_index];
        rx_fifo->get_index += 1;
        if (rx_fifo->get_index >= RT_SERIAL_RB_BUFSZ) rx_fifo->get_index = 0;

        if (rx_fifo->is_full == RT_TRUE)
        {
            rx_fifo->is_full = RT_FALSE;
        }

        /* enable interrupt */
        rt_hw_interrupt_enable(level);

        *data = ch & 0xff;
        data ++; length --;
    }

    return size - length;
}

char rt_hw_console_getchar(void)
{
    int ch = -1;
    
    RT_ASSERT(shell != RT_NULL);
    
    while (int_finsh_rx((void *)&ch, 1) != 1)
    {
        rt_sem_take(&shell->rx_sem, RT_WAITING_FOREVER);
    }

    return ch;
}

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


/*==============3.nano finsh��ֲ��finsh�߳���ѭ�����ͻ���buffer����ʵ�ֲ���=================*/

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






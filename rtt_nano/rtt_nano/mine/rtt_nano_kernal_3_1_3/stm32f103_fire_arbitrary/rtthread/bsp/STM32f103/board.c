/* ������Ӳ�����ͷ�ļ� */
#include "board.h" 

/* RT-Thread���ͷ�ļ� */
#include "rthw.h"
#include "rtthread.h"


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

void SysTick_Init(void);

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
    SysTick_Init();

    /*��̬�ڴ�ѳ�ʼ��*/
    #if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
    #endif
    
/* =============*Ӳ��BSP��ʼ��ͳͳ�����������LED�����ڣ�LCD�� =============*/
    
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


/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
void SysTick_Init(void)
{
	   	/* systick�ж�����Ӧ
	 *SystemFrequency / 1000       1ms�ж�1��
	 * SystemFrequency / 100000	 10us�ж�1��
	 * SystemFrequency / 1000000   1us�ж�1��
	 */
    RCC_ClocksTypeDef  RccClocks;
    uint32_t SystemClockFrequency = 0;
    RCC_GetClocksFreq(&RccClocks);
    SystemClockFrequency = RccClocks.HCLK_Frequency;
    while (SysTick_Config(SystemClockFrequency/RT_TICK_PER_SECOND));
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






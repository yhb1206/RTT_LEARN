/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "board.h"
#include "rtthread.h"

#define SOFT_DELAY Delay(0x0FFFFF);


/* �����߳̿�ջʱҪ�� RT_ALIGN_SIZE ���ֽڶ��� */
ALIGN(RT_ALIGN_SIZE)
/* �����߳�ջ */
static rt_uint8_t rt_led1_thread_stack[1024];

/* �����߳̿��ƿ� */
static struct rt_thread led1_thread;

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	

}

static void led1_thread_entry(void* parameter)
{
	while (1)
	{
        LED1_ON;
        rt_thread_delay(500); /* ������ʱ 500 �� tick */ 
        LED1_OFF;
        rt_thread_delay(500); /* ������ʱ 500 �� tick */
    }
}

/*********************************************END OF FILE**********************/

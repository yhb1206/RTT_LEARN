/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "board.h"
#include "rtthread.h"

#define SOFT_DELAY Delay(0x0FFFFF);


/* 定义线程控栈时要求 RT_ALIGN_SIZE 个字节对齐 */
ALIGN(RT_ALIGN_SIZE)
/* 定义线程栈 */
static rt_uint8_t rt_led1_thread_stack[1024];

/* 定义线程控制块 */
static struct rt_thread led1_thread;

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	

}

static void led1_thread_entry(void* parameter)
{
	while (1)
	{
        LED1_ON;
        rt_thread_delay(500); /* 挂起延时 500 个 tick */ 
        LED1_OFF;
        rt_thread_delay(500); /* 挂起延时 500 个 tick */
    }
}

/*********************************************END OF FILE**********************/

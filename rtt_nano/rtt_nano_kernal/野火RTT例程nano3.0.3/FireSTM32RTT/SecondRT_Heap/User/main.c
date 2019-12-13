/* 动态内存线程堆 */
#define SINGLE_PROCESSES  (0)
#define MULTI_PROCESSES   (1)

#define NUM_PROCESSES   MULTI_PROCESSES

/*
*************************************************************************
* 包含的头文件
*************************************************************************
*/
#include "board.h"
#include "rtthread.h"


/*
*************************************************************************
* 变量
*************************************************************************
*/
/* 定义线程控制块指针 */
static rt_thread_t led1_thread = RT_NULL;/*动态创建单线程，就是线程控制块*/
#if NUM_PROCESSES == MULTI_PROCESSES    /*多线程开关*/
static rt_thread_t led2_thread = RT_NULL;
#endif

/*
*************************************************************************
* 函数声明
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
#if NUM_PROCESSES == MULTI_PROCESSES    /*多线程开关*/
static void led2_thread_entry(void* parameter);
#endif




/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
    /*
     * 开发板硬件初始化， RTT 系统初始化已经在 main 函数之前完成，
     * 即在 component.c 文件中的 rtthread_startup()函数中完成了。
     * 所以在 main 函数中，只需要创建线程和启动线程即可。
     */
     
    led1_thread = /* 线程控制块指针 */
    rt_thread_create(  "led1", /* 线程名字 */    
                        led1_thread_entry, /* 线程入口函数 */
                        RT_NULL, /* 线程入口函数参数 */
                        512, /* 线程栈大小 */
                        3, /* 线程的优先级 */
                        20); /* 线程时间片 */
    
	/* 启动线程，开启调度 */
    if (led1_thread != RT_NULL)
    {
        rt_thread_startup(led1_thread);
    }
	else
	{
        return -1;
	}
	
#if NUM_PROCESSES == MULTI_PROCESSES    /*多线程开关*/
    led2_thread = /* 线程控制块指针 */
    rt_thread_create(  "led2", /* 线程名字 */    
                        led2_thread_entry, /* 线程入口函数 */
                        RT_NULL, /* 线程入口函数参数 */
                        512, /* 线程栈大小 */
                        4, /* 线程的优先级 */
                        20); /* 线程时间片 */
    
	/* 启动线程，开启调度 */
    if (led2_thread != RT_NULL)
    {
        rt_thread_startup(led2_thread);
    }
	else
	{
        return -1;
	}
	#endif
}

static void led1_thread_entry(void* parameter)
{
	while (1)
	{
        LED1_ON;
		rt_kprintf("led1_thread running,LED1_ON\r\n");
        rt_thread_delay(500); /* 挂起延时 500 个 tick */
		
        LED1_OFF;
		rt_kprintf("led1_thread running,LED1_OFF\r\n");
        rt_thread_delay(500); /* 挂起延时 500 个 tick */
    }
}

#if NUM_PROCESSES == MULTI_PROCESSES    /*多线程开关*/
static void led2_thread_entry(void* parameter)
{
	while (1)
	{
        LED2_ON;
		rt_kprintf("led2_thread running,LED2_ON\r\n");
        rt_thread_delay(500); /* 挂起延时 500 个 tick */ 
		
        LED2_OFF;
		rt_kprintf("led2_thread running,LED2_OFF\r\n");
        rt_thread_delay(500); /* 挂起延时 500 个 tick */
    }
}
#endif
/*********************************************END OF FILE**********************/

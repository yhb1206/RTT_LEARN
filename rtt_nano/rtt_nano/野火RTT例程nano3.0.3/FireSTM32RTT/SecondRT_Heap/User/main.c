/* ��̬�ڴ��̶߳� */
#define SINGLE_PROCESSES  (0)
#define MULTI_PROCESSES   (1)

#define NUM_PROCESSES   MULTI_PROCESSES

/*
*************************************************************************
* ������ͷ�ļ�
*************************************************************************
*/
#include "board.h"
#include "rtthread.h"


/*
*************************************************************************
* ����
*************************************************************************
*/
/* �����߳̿��ƿ�ָ�� */
static rt_thread_t led1_thread = RT_NULL;/*��̬�������̣߳������߳̿��ƿ�*/
#if NUM_PROCESSES == MULTI_PROCESSES    /*���߳̿���*/
static rt_thread_t led2_thread = RT_NULL;
#endif

/*
*************************************************************************
* ��������
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
#if NUM_PROCESSES == MULTI_PROCESSES    /*���߳̿���*/
static void led2_thread_entry(void* parameter);
#endif




/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
    /*
     * ������Ӳ����ʼ���� RTT ϵͳ��ʼ���Ѿ��� main ����֮ǰ��ɣ�
     * ���� component.c �ļ��е� rtthread_startup()����������ˡ�
     * ������ main �����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
     */
     
    led1_thread = /* �߳̿��ƿ�ָ�� */
    rt_thread_create(  "led1", /* �߳����� */    
                        led1_thread_entry, /* �߳���ں��� */
                        RT_NULL, /* �߳���ں������� */
                        512, /* �߳�ջ��С */
                        3, /* �̵߳����ȼ� */
                        20); /* �߳�ʱ��Ƭ */
    
	/* �����̣߳��������� */
    if (led1_thread != RT_NULL)
    {
        rt_thread_startup(led1_thread);
    }
	else
	{
        return -1;
	}
	
#if NUM_PROCESSES == MULTI_PROCESSES    /*���߳̿���*/
    led2_thread = /* �߳̿��ƿ�ָ�� */
    rt_thread_create(  "led2", /* �߳����� */    
                        led2_thread_entry, /* �߳���ں��� */
                        RT_NULL, /* �߳���ں������� */
                        512, /* �߳�ջ��С */
                        4, /* �̵߳����ȼ� */
                        20); /* �߳�ʱ��Ƭ */
    
	/* �����̣߳��������� */
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
        rt_thread_delay(500); /* ������ʱ 500 �� tick */
		
        LED1_OFF;
		rt_kprintf("led1_thread running,LED1_OFF\r\n");
        rt_thread_delay(500); /* ������ʱ 500 �� tick */
    }
}

#if NUM_PROCESSES == MULTI_PROCESSES    /*���߳̿���*/
static void led2_thread_entry(void* parameter)
{
	while (1)
	{
        LED2_ON;
		rt_kprintf("led2_thread running,LED2_ON\r\n");
        rt_thread_delay(500); /* ������ʱ 500 �� tick */ 
		
        LED2_OFF;
		rt_kprintf("led2_thread running,LED2_OFF\r\n");
        rt_thread_delay(500); /* ������ʱ 500 �� tick */
    }
}
#endif
/*********************************************END OF FILE**********************/

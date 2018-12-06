/* ��̬�ڴ��߳�ջ */
#include "board.h"
#include "rtthread.h"

/* �����߳̿��ƿ� */
static struct rt_thread led1_thread;

/* �����߳̿�ջʱҪ�� RT_ALIGN_SIZE ���ֽڶ��� */
ALIGN(RT_ALIGN_SIZE)

/* �����߳�ջ */
static rt_uint8_t rt_led1_thread_stack[1024];


/*************************************************************************
* ��������
*************************************************************************
*/
static void led1_thread_entry(void* parameter);


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
    rt_thread_init(&led1_thread, /* �߳̿��ƿ� */ 
                    "led1", /* �߳����� */ 
                    led1_thread_entry, /* �߳���ں��� */ 
                    RT_NULL, /* �߳���ں������� */ 
                    &rt_led1_thread_stack[0], /* �߳�ջ��ʼ��ַ */ 
                    sizeof(rt_led1_thread_stack), /* �߳�ջ��С */ 
                    3, /* �̵߳����ȼ� */ 
                    20); /* �߳�ʱ��Ƭ */
    
    rt_thread_startup(&led1_thread); /* �����̣߳��������� */

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

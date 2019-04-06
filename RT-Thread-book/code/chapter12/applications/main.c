/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-16     armink       first implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/* using RED LED in RGB */
#define LED_PIN              PIN_LED_R

int main(void)
{
    unsigned int count = 1;
    /* set LED pin mode to output */
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);

    while (count > 0)
    {
        /* led on */
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(500);

        /* led off */
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(500);

        count++;
    }

    return 0;
}

/* msh命令示例 */
void hello_rtt(void)
{
    rt_kprintf("hello RT-Thread!\n");
}
MSH_CMD_EXPORT(hello_rtt , say hello to RT-Thread);

/* 带参数的msh命令示例 */
static void atcmd(int argc, char **argv)
{
    if (argc < 2)
    {
        rt_kprintf("Please input 'atcmd <server|client>' \n");
        return;
    }

    if (!rt_strcmp(argv[1], "server"))
    {
        rt_kprintf("AT server!\n");
    }
    else if (!rt_strcmp(argv[1], "client"))
    {
        rt_kprintf("AT client!\n");
    }
    else
    {
        rt_kprintf("Please input 'atcmd <server|client>' \n");
    }
}
MSH_CMD_EXPORT(atcmd, atcmd sample: atcmd <server|client>);



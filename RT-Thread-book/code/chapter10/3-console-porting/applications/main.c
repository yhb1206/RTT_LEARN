/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-25     DQL      first implementation
 */

#include <rtthread.h>
#include <stdio.h>
#include <board.h>

int main(void)
{
    while (1)
    {
        bsp_led_on();
        rt_kprintf("led on\n");
        rt_thread_delay(RT_TICK_PER_SECOND);

        bsp_led_off();
        rt_kprintf("led off\n");
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
}

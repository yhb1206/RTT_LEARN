/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <stm32l4xx.h>

void rt_hw_board_init(void);

void bsp_led_init(void);
void bsp_led_on(void);
void bsp_led_off(void);

void bsp_uart_init(void);
void bsp_uart_send(char c);

#endif

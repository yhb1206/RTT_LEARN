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
#include <dfs_fs.h>

int main(void)
{
    /* mount the file system from spi flash */
    if (dfs_mount("w25q128", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("Filesystem initialized!\n");
    }
    else
    {
        rt_kprintf("Failed to initialize filesystem!\n");
    }

    return 0;
}

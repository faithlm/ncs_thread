/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include "kernel.h"
#include <sys/printk.h>

#define MY_TEST_STACK_SIZE 500
#define MY_TEST_PRIORITY 5

#define SLEEP_TIME_MS   1000
//动态创建一个线程，通过在main里面调用 thread_dyamic_create 来创建
K_THREAD_STACK_DEFINE(my_test2_dyamic_thread_stack_area, MY_TEST_STACK_SIZE);
//这个 k_thread 目前看到的只有 k_thread_join 用到，其他的api用来识别线程的都是通过 k_tid
static struct k_thread my_test2_dyamic_thread; 
void my_test2_thread_entry_point(void *arg1,void *arg2,void *arg3)
{
	uint8_t cnt = 0;
	printk("This is test2 thread,create by dyamic!\r\n");
	while(true)
	{
		printk("Test2 thread run!\r\n");
		cnt++;
		if(5 == cnt)
		{
			printk("Test2 thread Finish\r\n");
			return ;
		}
		k_msleep(SLEEP_TIME_MS);
	}
}
void thread_dyamic_create()
{	
	k_tid_t my_test2_dyamic_thread_id = k_thread_create(&my_test2_dyamic_thread,my_test2_dyamic_thread_stack_area, 
									K_THREAD_STACK_SIZEOF(my_test2_dyamic_thread_stack_area),
									my_test2_thread_entry_point,
									NULL,NULL,NULL,MY_TEST_PRIORITY,0,K_NO_WAIT);
	printk("test2 thread tid = %d\r\n",(int)my_test2_dyamic_thread_id);
}

//静态创建1个线程，使用 K_THREAD_DEFINE 进行创建，名字为my_test1_static_thread_id
void my_test1_thread_entry_point(void)
{
	printk("This is test1 thread,create by static,it wait the test2 thread finish!\r\n");
	//k_thread_join 用于等待一个线程结束，结束以后再继续运行，这里等待 my_test2_dyamic_thread 这个线程结束，然后开始运行
	k_thread_join(&my_test2_dyamic_thread,K_FOREVER );
	while(true)
	{
		printk("Test1 thread run!\r\n");
		k_msleep(SLEEP_TIME_MS);
	}
}
//K_THREAD_DEFINE 参数从前往后依次是：线程名字，也叫作thread_id,栈空间大小，入口函数，三个入口函数参数，线程调度优先级，线程选项，启动延时时间
K_THREAD_DEFINE(my_test1_static_thread_id, MY_TEST_STACK_SIZE,my_test1_thread_entry_point, NULL, NULL, NULL, MY_TEST_PRIORITY, 0, 0);

void main(void)
{
	thread_dyamic_create();
	printk("Thread create test");
}
	

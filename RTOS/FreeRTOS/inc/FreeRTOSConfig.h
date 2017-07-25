/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 *=================================== 中文注解 =======================================
 *
 * 作者：秉火 Fire
 * 论坛：www.firebbs.cn
 * 淘宝：www.fire-stm32.taobao.com
 * 
 * FreeRTOSConfig.h这个头文件用来配置FreeRTOS的功能，但是并不全面，剩下的其他功能可以
 * 在FreeRTOS.h这个头文件配置，比如使能互斥信号量，使能递归信号量，使能事件标志组这些
 * 内核对象的功能时，是在FreeRTOS.h这个头文件里面配置的。但是，为了方便配置，我们可以
 * 统一在FreeRTOSConfig.h配置，当需要用到什么功能就把对应的宏在FreeRTOSConfig.h中使能。
 *====================================================================================
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/


#define configCPU_CLOCK_HZ			( ( unsigned long ) 180000000 )	/* 系统时钟，单位为HZ */
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )         /* SysTick中断周期，单位为HZ，1000HZ即1ms中断一次 */

/*================================================================================================================================================================
 ********************************************************************* FreeRTOS功能配置开始 **********************************************************************
 * 
 * 这些宏都可以在FreeRTOS.h这个头文件里面开启，但是这个头文件里面的宏非常多，找起来很麻烦，为了方便，我们可以统一在FreeRTOSConfig.h这个头文件里面配置。
 * 目前只包含了常用的配置，剩下的其他功能等需要用的时候再配置即可。
 *
 *===============================================================================================================================================================*/
#define configUSE_PREEMPTION		1                               /* 使能抢占式调度，否则用合作式调度，默认我们都是用抢占式 */
#define configUSE_IDLE_HOOK			0                               /* 空闲任务钩子函数，如果配置为1，这个钩子函数需要用户自己编写 */
#define configUSE_TICK_HOOK			0                               /* 时基任务钩子函数，如果配置为1，这个钩子函数需要用户自己编写*/

#define configMAX_PRIORITIES		( 5 )                           /* 任务能使用最大优先级个数，数字越大优先级越高，范围为：0~configMAX_PRIORITIES-1
                                                                   最低的0由系统分配给空闲任务，每个任务的优先级可以相同 */
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 75 * 1024 ) )    /* 堆空间大小，内核在创建各种对象时需要用到，单位为字节 */
#define configMAX_TASK_NAME_LEN		( 32 )                        /* 任务名称的长度，即字符串的长度 */
#define configUSE_TRACE_FACILITY	1
#define configUSE_16_BIT_TICKS		0                             /* SysTick Counter的宽度，0表示32bit，1表示16bit，STM32用的是32bit */
#define configIDLE_SHOULD_YIELD		1                             /* 上下文切换强制使能，即当前任务执行完毕之后还有剩余的时间片，可以强制自己放弃
                                                                   剩余的时间片，然后执行上下文切换去执行其他的任务*/
/* 
 * 合作式调度配置 
 * 合作式调度是为性能低，资源少的处理器设计的，非常非常少用到，在STM32这种高性能，资源多的处理器上是不会使用的。
 * 合作式调度在FreeRTOS-V9.0.0版本之后不会被删除，但是也不会更新，就是这么留着。
 */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* 递归互斥信号量配置 */
#define configUSE_RECURSIVE_MUTEXES 0

/* 互斥信号量配置 */
#define configUSE_MUTEXES 1

/* 计数信号量配置 */
#define configUSE_COUNTING_SEMAPHORES 1

/* 软件定时器配置 */
#define configUSE_TIMERS				1
#define configTIMER_TASK_PRIORITY		( 2 )
#define configTIMER_QUEUE_LENGTH		10
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )

/* 任务通知配置
 * 即任务信号量和任务消息 ，是任务的自有的属性
 * 在任务的控制块TCB里面有一个32bit的变量来传递消息
 */
#define configUSE_TASK_NOTIFICATIONS 1

/* 多个内核对象配置 */
#define configUSE_QUEUE_SETS 0

/*
 * 1：使能函数
 * 0：失能函数
 */
#define INCLUDE_vTaskPrioritySet		      1
#define INCLUDE_uxTaskPriorityGet		      1
#define INCLUDE_vTaskDelete				        1
#define INCLUDE_vTaskCleanUpResources	    1
#define INCLUDE_vTaskSuspend			        1
#define INCLUDE_vTaskDelayUntil			      1
#define INCLUDE_vTaskDelay				        1  /* vTaskDelay 为阻塞延时函数，常用，单位为ms */


extern volatile unsigned int uiTimerCount;

#define portGET_RUN_TIME_COUNTER_VALUE() uiTimerCount
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() (uiTimerCount = 0)

#define configUSE_STATS_FORMATTING_FUNCTIONS 1
#define configGENERATE_RUN_TIME_STATS        1


/*================================================================================================================================================================
 ********************************************************************* FreeRTOS功能配置结束 **********************************************************************
 *===============================================================================================================================================================*/
 
 
/*======================================== STM32使用了多少bit来表达优先级 ========================================*/
/*
 * Cortex-M内核使用8bit来配置优先级，但是STM32只使用了高4bit，数值越小，优先级越高。
 * 在往寄存器里面写数值配置的时候，是按照8bit来写的，所以真正写的时候需要经过转换，公式为：
 * ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff)，其中的priority就是我们配置的真正的优先级
 */
#ifdef __NVIC_PRIO_BITS  /* __NVIC_PRIO_BITS 已经在stm32f4xx.h里面定义为4 */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4
#endif

/*============================================== SysTick中断优先级配置 ============================================*/
/*
 * 在往寄存器里面写数值配置的时候，是按照8bit来写的，所以真正写的时候需要经过转换，公式为：
 * ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff)，其中的priority就是我们配置的真正的优先级。经过这个公式之后得到的是
 * 下面的这个宏：configKERNEL_INTERRUPT_PRIORITY
 * SysTick的优先级我们一般配置为最低，即0xf 。这样可以提高系统的实时响应能力，即其他的外部中断可以及时的得到响应。
 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0xf
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/*===========================================可屏蔽的中断优先级配置====================================================*/
/*
 * 用于配置STM32的特殊寄存器basepri寄存器的值，用于屏蔽中断，当大于basepri值的优先级的中断将被全部屏蔽。basepri只有4bit有效，
 * 默认只为0，即全部中断都没有被屏蔽。configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY配置为：5，意思就是中断优先级大于5的中断都被屏蔽。
 * 当把配置好的优先级写到寄存器的时候，是按照8bit来写的，所以真正写的时候需要经过转换，公式为：
 * ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff)，其中的priority就是我们配置的真正的优先级。经过这个公式之后得到的是下面的这个宏：
 * configMAX_SYSCALL_INTERRUPT_PRIORITY
 *
 * 在FreeRTOS中，关中断是通过配置basepri寄存器来实现的，关掉的中断由配置的basepri的值决定，小于basepri值的
 * 中断FreeRTOS是关不掉的，这样做的好处是可以系统设计者可以人为的控制那些非常重要的中断不能被关闭，在紧要的关头必须被响应。
 * 而在UCOS中，关中断是通过控制PRIMASK来实现的，PRIMASK是一个单1的二进制位，写1则除能除了NMI和硬 fault的所有中断。当UCOS关闭
 * 中断之后，即使是你在系统中设计的非常紧急的中断来了都不能马上响应，这加大了中断延迟的时间，如果是性命攸关的场合，那后果估计挺严重。
 * 相比UCOS的关中断的设计，FreeRTOS的设计则显得人性化很多。
 *
 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	0x1
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/*
 * 断言配置，一般调试的时候用，发布的时候不用
 */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }	
	

/*=================================== SVC，PendSV 和 SysTick 中断服务函数的配置 ========================================*/
/*
 * 在移植FreeRTOS的时候，需要用到STM32的三个中断，分别是SVC,PendSV和SysTick，这三个中断在向量表
 * 里面的名字分别是：SVC_Handler，PendSV_Handler和SysTick_Handler（在启动文件:startup_stm32f10x_hd.s中）
 * 
 * 而在port.c里面写的这三个中断的服务函数的名称跟向量表里面的名称不一样，为了中断响应之后能正确的执行port.c
 * 里面写好的中断服务函数，我们需要统一向量表和中断服务函数的名字。
 *
 * 为了实现这个目的，可以有两种方法：
 * 1：把启动文件里面的向量表里面的名字改成跟port.c里面的中断服务函数名一样。
 * 2：把port.c里面的中断服务函数名改成跟启动文件里面的中断向量表里面的名字一样。
 *
 * 这里为了保持启动文件的完整性，我们统一修改port.c里面的中断函数名，即添加下面三个宏定义即可。
 * 如果你在stm32f10x_it.c这里面实现了这三个中断服务函数的定义的话，那么为了避免跟port.c里面的重复定义，应该
 * 把stm32f4xx_it.c里面的注释掉。
 */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */


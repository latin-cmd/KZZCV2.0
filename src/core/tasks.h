#ifndef __TASKS_H
#define __TASKS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS资源句柄 */
extern QueueHandle_t uart_rx_queue;  // uart接收中断队列
extern QueueHandle_t temp_queue;     // 内部温度传感器队列

/* START_TASK 任务配置 */
#define START_TASK_PRIO     1           /* 任务优先级 */
#define START_STK_SIZE      128         /* 任务堆栈大小 */
extern TaskHandle_t StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);    /* 任务函数 */

/* LV_DEMO_TASK 任务配置 */
#define LV_DEMO_TASK_PRIO   3           /* 任务优先级 */
#define LV_DEMO_STK_SIZE    4096        /* 任务堆栈大小 */
extern TaskHandle_t LV_DEMOTask_Handler; /* 任务句柄 */
void lv_demo_task(void *pvParameters);   /* 任务函数 */

/* LED_TASK 任务配置 */
#define LED_TASK_PRIO       4           /* 任务优先级 */
#define LED_STK_SIZE        128         /* 任务堆栈大小 */
extern TaskHandle_t LEDTask_Handler;     /* 任务句柄 */
void led_task(void *pvParameters);       /* 任务函数 */

/* ADC3_TEMP_TASK 任务配置 */
extern TaskHandle_t ADC3Task_Handler;    /* 任务句柄 */
void ADC3_task(void *pvParameters);      /* 任务函数 */

#endif /* __TASKS_H */ 
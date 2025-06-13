#include "main.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_benchmark.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* 任务相关 */
TaskHandle_t start_task_handle;
TaskHandle_t lv_demo_task_handle;
TaskHandle_t led_task_handle;
TaskHandle_t adc3_temp_task_handle;

/* 队列相关 */
QueueHandle_t uart_rx_queue;
QueueHandle_t temp_queue;

/* 函数实现 */
void start_task(void *pvParameters)
{
    // 创建其他任务
    xTaskCreate(lv_demo_task, "lv_demo_task", LV_DEMO_TASK_STK_SIZE, NULL, LV_DEMO_TASK_PRIO, &lv_demo_task_handle);
    xTaskCreate(led_task, "led_task", LED_TASK_STK_SIZE, NULL, LED_TASK_PRIO, &led_task_handle);
    xTaskCreate(adc3_temp_task, "adc3_temp_task", ADC3_TEMP_TASK_STK_SIZE, NULL, ADC3_TEMP_TASK_PRIO, &adc3_temp_task_handle);

    // 删除启动任务
    vTaskDelete(start_task_handle);
}

void lv_demo_task(void *pvParameters)
{
    // 初始化LVGL
    lvgl_init();

    // 创建主界面
    create_main_ui();

    while (1)
    {
        // 处理LVGL任务
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void led_task(void *pvParameters)
{
    while (1)
    {
        // LED闪烁
        LED_Toggle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void adc3_temp_task(void *pvParameters)
{
    float temperature;
    while (1)
    {
        // 获取温度值
        temperature = adc_get_temperature();

        // 发送温度值到队列
        xQueueSend(temp_queue, &temperature, 0);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    // 初始化系统时钟
    SystemClock_Config();

    // 初始化外设
    lcd_init();
    touch_init();
    adc_init();
    usb_init();

    // 创建队列
    uart_rx_queue = xQueueCreate(10, sizeof(uint8_t));
    temp_queue = xQueueCreate(10, sizeof(float));

    // 创建启动任务
    xTaskCreate(start_task, "start_task", START_TASK_STK_SIZE, NULL, START_TASK_PRIO, &start_task_handle);

    // 启动调度器
    vTaskStartScheduler();

    while (1)
    {
        // 系统空闲
    }
}

void SystemClock_Config(void)
{
    // 配置系统时钟
    // 这里需要根据具体的硬件平台来实现
} 
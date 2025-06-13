#include "uart_ui.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_benchmark.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* 串口界面控件 */
lv_obj_t *uart_tabview;        // uart选项卡
lv_obj_t *uart_btnm;           // 串口助手界面按钮矩阵
lv_obj_t *uart_bg;             // UART界面
lv_obj_t *uart_init_btn;       // 初始化按钮
lv_obj_t *uart_date_roller;    // 数据位选择器
lv_obj_t *baundrate_roller;    // 波特率选择器
lv_obj_t *uart_stop_roller;    // 停止位选择器
lv_obj_t *uart_parity_roller;  // 校验位选择器
lv_obj_t *uart_mode_roller;    // 模式选择器
lv_obj_t *uart_it_flag_roller; // 中断标志选择器
lv_obj_t *uart_it_pr_roller;   // 中断优先级选择器
lv_obj_t *uart_it_enable_switch; // 中断使能开关
lv_obj_t *uart_enable_switch;  // 串口使能开关

/* 串口配置选项 */
const char *baudrate_options = "1200\n2400\n4800\n9600\n19200\n38400\n115200";
const char *uart_date_options = "7\n8\n9";
const char *uart_parity_options = "Odd Parity\nEven Parity\nNo Parity";
const char *uart_stop_options = "0.5\n1\n1.5\n2";
const char *uart_mode_options = "TX\nRX\nTX/RX";
const char *uart_it_flag_options = "TX\nRX\nTX/RX";
const char *uart_it_pr_options = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15";
const char *uart_btnm_map[] = { "0","1","2","3","4","5","6","7","8","9","\n","A","B","C","D","E","F","Del","\n","a","b","c","d","e","f",",","__","" };

/* 串口操作按钮 */
lv_obj_t *uart_get_clean_btn;  // 消息接收区域清除按钮
lv_obj_t *uart_send_clean_btn; // 消息发送区域清除按钮
lv_obj_t *uart_send_btn;       // 消息发送按钮
lv_obj_t *uart_off_switch;     // 串口开关
lv_obj_t *uart_send2_label;    // 串口发送区域文本框
lv_obj_t *uart_get_label;      // 串口接收区域文本框

/* 函数实现 */
void uart_btn_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (obj == uart_init_btn)
        {
            // 初始化串口
        }
        else if (obj == uart_get_clean_btn)
        {
            // 清除接收区域
            lv_label_set_text(uart_get_label, "");
        }
        else if (obj == uart_send_clean_btn)
        {
            // 清除发送区域
            lv_label_set_text(uart_send2_label, "");
        }
        else if (obj == uart_send_btn)
        {
            // 发送数据
            const char *text = lv_label_get_text(uart_send2_label);
            // TODO: 实现串口发送
        }
    }
}

void USART1_IRQHandler(void)
{
    // 串口中断处理
    uint8_t data;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        data = USART_ReceiveData(USART1);
        // 将接收到的数据发送到队列
        xQueueSendFromISR(uart_rx_queue, &data, NULL);
    }
}

void create_uart_ui(void)
{
    // 创建串口界面背景
    uart_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(uart_bg, 480, 320);
    lv_obj_set_pos(uart_bg, 0, 0);
    lv_obj_set_style_bg_color(uart_bg, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(uart_bg, 0, 0);

    // 创建串口配置区域
    lv_obj_t *config_area = lv_obj_create(uart_bg);
    lv_obj_set_size(config_area, 480, 100);
    lv_obj_set_pos(config_area, 0, 0);
    lv_obj_set_style_bg_color(config_area, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(config_area, 0, 0);

    // 创建波特率选择器
    baundrate_roller = lv_roller_create(config_area);
    lv_roller_set_options(baundrate_roller, baudrate_options, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(baundrate_roller, 100, 80);
    lv_obj_set_pos(baundrate_roller, 10, 10);
    lv_obj_set_style_bg_color(baundrate_roller, lv_color_hex(0x2a2a2a), 0);

    // 创建数据位选择器
    uart_date_roller = lv_roller_create(config_area);
    lv_roller_set_options(uart_date_roller, uart_date_options, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(uart_date_roller, 100, 80);
    lv_obj_set_pos(uart_date_roller, 120, 10);
    lv_obj_set_style_bg_color(uart_date_roller, lv_color_hex(0x2a2a2a), 0);

    // 创建校验位选择器
    uart_parity_roller = lv_roller_create(config_area);
    lv_roller_set_options(uart_parity_roller, uart_parity_options, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(uart_parity_roller, 100, 80);
    lv_obj_set_pos(uart_parity_roller, 230, 10);
    lv_obj_set_style_bg_color(uart_parity_roller, lv_color_hex(0x2a2a2a), 0);

    // 创建停止位选择器
    uart_stop_roller = lv_roller_create(config_area);
    lv_roller_set_options(uart_stop_roller, uart_stop_options, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(uart_stop_roller, 100, 80);
    lv_obj_set_pos(uart_stop_roller, 340, 10);
    lv_obj_set_style_bg_color(uart_stop_roller, lv_color_hex(0x2a2a2a), 0);

    // 创建初始化按钮
    uart_init_btn = lv_btn_create(config_area);
    lv_obj_set_size(uart_init_btn, 100, 30);
    lv_obj_set_pos(uart_init_btn, 10, 60);
    lv_obj_set_style_bg_color(uart_init_btn, lv_color_hex(0x2a2a2a), 0);
    lv_obj_add_event_cb(uart_init_btn, uart_btn_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *init_label = lv_label_create(uart_init_btn);
    lv_label_set_text(init_label, "初始化");
    lv_obj_center(init_label);

    // 创建串口开关
    uart_off_switch = lv_switch_create(config_area);
    lv_obj_set_size(uart_off_switch, 50, 25);
    lv_obj_set_pos(uart_off_switch, 120, 60);

    // 创建发送区域
    lv_obj_t *send_area = lv_obj_create(uart_bg);
    lv_obj_set_size(send_area, 480, 100);
    lv_obj_set_pos(send_area, 0, 110);
    lv_obj_set_style_bg_color(send_area, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(send_area, 0, 0);

    // 创建发送文本框
    uart_send2_label = lv_label_create(send_area);
    lv_obj_set_size(uart_send2_label, 400, 80);
    lv_obj_set_pos(uart_send2_label, 10, 10);
    lv_label_set_text(uart_send2_label, "");

    // 创建发送按钮
    uart_send_btn = lv_btn_create(send_area);
    lv_obj_set_size(uart_send_btn, 60, 30);
    lv_obj_set_pos(uart_send_btn, 410, 10);
    lv_obj_set_style_bg_color(uart_send_btn, lv_color_hex(0x2a2a2a), 0);
    lv_obj_add_event_cb(uart_send_btn, uart_btn_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *send_label = lv_label_create(uart_send_btn);
    lv_label_set_text(send_label, "发送");
    lv_obj_center(send_label);

    // 创建清除发送按钮
    uart_send_clean_btn = lv_btn_create(send_area);
    lv_obj_set_size(uart_send_clean_btn, 60, 30);
    lv_obj_set_pos(uart_send_clean_btn, 410, 50);
    lv_obj_set_style_bg_color(uart_send_clean_btn, lv_color_hex(0x2a2a2a), 0);
    lv_obj_add_event_cb(uart_send_clean_btn, uart_btn_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *clean_send_label = lv_label_create(uart_send_clean_btn);
    lv_label_set_text(clean_send_label, "清除");
    lv_obj_center(clean_send_label);

    // 创建接收区域
    lv_obj_t *recv_area = lv_obj_create(uart_bg);
    lv_obj_set_size(recv_area, 480, 100);
    lv_obj_set_pos(recv_area, 0, 220);
    lv_obj_set_style_bg_color(recv_area, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(recv_area, 0, 0);

    // 创建接收文本框
    uart_get_label = lv_label_create(recv_area);
    lv_obj_set_size(uart_get_label, 400, 80);
    lv_obj_set_pos(uart_get_label, 10, 10);
    lv_label_set_text(uart_get_label, "");

    // 创建清除接收按钮
    uart_get_clean_btn = lv_btn_create(recv_area);
    lv_obj_set_size(uart_get_clean_btn, 60, 30);
    lv_obj_set_pos(uart_get_clean_btn, 410, 10);
    lv_obj_set_style_bg_color(uart_get_clean_btn, lv_color_hex(0x2a2a2a), 0);
    lv_obj_add_event_cb(uart_get_clean_btn, uart_btn_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *clean_recv_label = lv_label_create(uart_get_clean_btn);
    lv_label_set_text(clean_recv_label, "清除");
    lv_obj_center(clean_recv_label);
} 
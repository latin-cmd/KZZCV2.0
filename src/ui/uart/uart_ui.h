#ifndef __UART_UI_H
#define __UART_UI_H

#include "lvgl.h"

/* 串口界面控件 */
extern lv_obj_t *uart_tabview;        // uart选项卡
extern lv_obj_t *uart_btnm;           // 串口助手界面按钮矩阵
extern lv_obj_t *uart_bg;             // UART界面
extern lv_obj_t *uart_init_btn;       // 初始化按钮
extern lv_obj_t *uart_date_roller;    // 数据位选择器
extern lv_obj_t *baundrate_roller;    // 波特率选择器
extern lv_obj_t *uart_stop_roller;    // 停止位选择器
extern lv_obj_t *uart_parity_roller;  // 校验位选择器
extern lv_obj_t *uart_mode_roller;    // 模式选择器
extern lv_obj_t *uart_it_flag_roller; // 中断标志选择器
extern lv_obj_t *uart_it_pr_roller;   // 中断优先级选择器
extern lv_obj_t *uart_it_enable_switch; // 中断使能开关
extern lv_obj_t *uart_enable_switch;  // 串口使能开关

/* 串口配置选项 */
extern const char *baudrate_options;
extern const char *uart_date_options;
extern const char *uart_parity_options;
extern const char *uart_stop_options;
extern const char *uart_mode_options;
extern const char *uart_it_flag_options;
extern const char *uart_it_pr_options;
extern const char *uart_btnm_map[];

/* 串口操作按钮 */
extern lv_obj_t *uart_get_clean_btn;  // 消息接收区域清除按钮
extern lv_obj_t *uart_send_clean_btn; // 消息发送区域清除按钮
extern lv_obj_t *uart_send_btn;       // 消息发送按钮
extern lv_obj_t *uart_off_switch;     // 串口开关
extern lv_obj_t *uart_send2_label;    // 串口发送区域文本框
extern lv_obj_t *uart_get_label;      // 串口接收区域文本框

/* 函数声明 */
void uart_btn_event(lv_event_t *e);
void USART1_IRQHandler(void);

#endif /* __UART_UI_H */ 
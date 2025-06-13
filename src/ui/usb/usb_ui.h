#ifndef __USB_UI_H
#define __USB_UI_H

#include "lvgl.h"

/* USB界面控件 */
extern lv_obj_t *usb_bg;                // usb界面
extern lv_obj_t *usb_tileview;          // usb界面
extern lv_obj_t *usb_tile_1;            // usb平铺视图1
extern lv_obj_t *usb_link_success_label; // usb连接成功文本图标
extern lv_timer_t *usb_check_timer;     // usb连接检测定时器
extern uint8_t usb_link_symbol;         // usb连接是否成功标志位
extern lv_obj_t *usb_link_success_win2; // USB读写状态背景
extern lv_obj_t *usb_link_success_label2; // USB读写状态标题

/* 函数声明 */
void usb_event(lv_event_t *e);
void usb_check_timer_event(lv_event_t *e);

#endif /* __USB_UI_H */ 
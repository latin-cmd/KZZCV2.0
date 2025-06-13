#ifndef __SETTINGS_UI_H
#define __SETTINGS_UI_H

#include "lvgl.h"

/* 设置界面控件 */
extern lv_obj_t *setting_tabview;  // 设置界面选项卡
extern lv_obj_t *setting_bg;       // 设置界面背景
extern lv_obj_t *setting_bg2;      // 设置界面右边背景
extern lv_obj_t *list;             // 设置界面左边选项列表
extern lv_obj_t *setting_bg2_label; // 设置界面右边背景文本
extern lv_obj_t *btn;              // 设置界面列表按钮
extern lv_obj_t *freertos;         // 实时操作系统图片
extern lv_obj_t *lvgl;             // LVGL图片

/* 硬件信息界面线条 */
extern lv_point_t hardware_line[];

/* 函数声明 */
void list_btn_event_cb(lv_event_t *e);

#endif /* __SETTINGS_UI_H */ 
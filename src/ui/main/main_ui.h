#ifndef __MAIN_UI_H
#define __MAIN_UI_H

#include "lvgl.h"

/* 主界面样式 */
extern lv_style_t main_style;
extern lv_style_t world_style;

/* 主界面控件 */
extern lv_obj_t *main_bg;       // 主界面背景
extern lv_obj_t *main_return;   // 返回按钮
extern lv_obj_t *main_return_pic; // 返回按钮图标

/* 通知栏相关 */
extern lv_obj_t *note;          // 下拉通知栏
extern lv_anim_t note_anim;     // 拉通知栏动画对象
extern lv_obj_t *note_down;
extern lv_obj_t *note_down_label;
extern uint8_t note_symbol;     // 通知栏状态

/* 状态图标 */
extern uint8_t wifi_symbol;
extern uint8_t GPS_symbol;
extern uint8_t blue_symbol;

/* WiFi相关控件 */
extern lv_obj_t *wifi_obj;      // 通知界面WIFI基础对象
extern lv_obj_t *wifi_pic;      // 通知界面WIFI图标
extern lv_obj_t *wifi_btn;      // 通知界面WIFI按钮
extern lv_obj_t *wifi_labal;    // 通知界面WIFI文本
extern lv_obj_t *wifi_labal2;   // 主界面WIFI开启图案
extern uint8_t wifi_long_symbol; // WIFI长按

/* 蓝牙相关控件 */
extern lv_obj_t *blue_obj;      // 通知界面蓝牙基础对象
extern lv_obj_t *blue_pic;      // 通知界面蓝牙图标
extern lv_obj_t *blue_btn;      // 通知界面蓝牙按钮
extern lv_obj_t *blue_labal;    // 通知界面蓝牙文本
extern lv_obj_t *blue_labal2;   // 主界面蓝牙开启图案

/* GPS相关控件 */
extern lv_obj_t *GPS_obj;       // 通知界面GPS基础对象
extern lv_obj_t *GPS_pic;       // 通知界面GPS图标
extern lv_obj_t *GPS_btn;       // 通知界面GPS按钮
extern lv_obj_t *GPS_labal;     // 通知界面GPS文本
extern lv_obj_t *GPS_labal2;    // 主界面GPS开启图案

/* 时间和日期显示 */
extern lv_obj_t *time_labal;    // 主界时间
extern lv_obj_t *data_labal;    // 主界面日期
extern lv_obj_t *time_obj;      // 主界面时间背景
extern lv_obj_t *data_obj;      // 主界面日期背景

/* 主界面功能按钮 */
extern lv_obj_t *main_0;        // 主界面设置按钮
extern lv_obj_t *main_set_pic;  // 主界面设置按钮图标
extern lv_obj_t *main_text;     // 主界面文本基础对象
extern lv_obj_t *main_text_label; // 主界面菜单文本框

/* 主界面其他按钮 */
extern lv_obj_t *main_1;        // 主界面设置按钮
extern lv_obj_t *main_set_pic1; // 主界面设置按钮图标
extern lv_obj_t *main_2;        // 主界面设置按钮
extern lv_obj_t *main_set_pic2; // 主界面设置按钮图标
extern lv_obj_t *main_3;        // 主界面设置按钮
extern lv_obj_t *main_set_pic3; // 主界面设置按钮图标
extern lv_obj_t *main_4;        // 主界面设置按钮
extern lv_obj_t *main_set_pic4; // 主界面设置按钮图标
extern lv_obj_t *main_5;        // 主界面设置按钮
extern lv_obj_t *main_set_pic5; // 主界面设置按钮图标
extern lv_obj_t *main_6;        // 主界面设置按钮
extern lv_obj_t *main_set_pic6; // 主界面设置按钮图标
extern lv_obj_t *main_7;        // 主界面设置按钮
extern lv_obj_t *main_set_pic7; // 主界面设置按钮图标
extern lv_obj_t *main_8;        // 主界面设置按钮
extern lv_obj_t *main_set_pic8; // 主界面设置按钮图标

/* 主界面背景 */
extern lv_obj_t *main_bg2;
extern lv_obj_t *main_bg1;

/* 函数声明 */
void KZ_MAIN(void);  // 控制中心主界面函数
void note_y(void *obj, int32_t y);
void note_main_event(lv_event_t *e);
void note_event(lv_event_t *e);
void main_bg_event(lv_event_t *e);
void main_btn_event(lv_event_t *e);

#endif /* __MAIN_UI_H */ 
#ifndef __CHART_UI_H
#define __CHART_UI_H

#include "lvgl.h"

/* 温度图表相关 */
extern lv_obj_t *chat_bg;              // 图表界面
extern lv_obj_t *chart;                // 温度图表
extern lv_chart_series_t *ser1;        // 温度图表数据序列2
extern lv_chart_series_t *ser0;        // 温度图表数据序列1
extern lv_chart_series_t *ser_1;       // 温度图表数据序列3
extern lv_timer_t *chat_temp_timer;    // 温度图表定时器
extern lv_obj_t *chat_temp_enable_switch; // 温度图表使能开关
extern lv_obj_t *chat_temp_clean_btn;  // 温度曲线重置开关
extern lv_obj_t *chat_temp_switch1;    // 数据一开关
extern lv_obj_t *chat_temp_switch2;    // 数据二开关
extern lv_obj_t *chat_temp_switch3;    // 数据三开关
extern lv_obj_t *char_temp_x_slider;   // 温度图表X轴缩放滑块
extern lv_obj_t *char_temp_Y_slider;   // 温度图表Y轴缩放滑块

/* 柱状图相关 */
extern lv_obj_t *chart2;               // 柱状图
extern lv_timer_t *chat_bar_timer;     // 柱状图表定时器
extern lv_chart_series_t *ser2;        // 柱形图数据序列1
extern lv_chart_series_t *ser3;        // 柱形图数据序列2
extern lv_obj_t *chat_bar_enable_switch; // 柱状图表使能开关
extern lv_obj_t *chat_bar_clean_btn;   // 柱状曲线重置开关
extern lv_obj_t *chat_bar_switch1;     // 柱状数据一开关
extern lv_obj_t *chat_bar_switch2;     // 柱状数据二开关
extern lv_obj_t *char_bar_x_slider;    // 柱状图表X轴缩放滑块
extern lv_obj_t *char_bar_Y_slider;    // 柱状图表Y轴缩放滑块

/* 示例数据 */
extern lv_coord_t chat_data2[];
extern lv_coord_t chat_data1[];

/* 函数声明 */
void add_data(lv_timer_t *timer);
void add_data2(lv_timer_t *timer);
void chat_event(lv_event_t *e);
void slider_event_cb(lv_event_t *e);

#endif /* __CHART_UI_H */ 
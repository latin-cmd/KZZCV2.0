#include "chart_ui.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_benchmark.h"
#include "FreeRTOS.h"
#include "task.h"

/* 温度图表相关 */
lv_obj_t *chat_bg;              // 图表界面
lv_obj_t *chart;                // 温度图表
lv_chart_series_t *ser1;        // 温度图表数据序列2
lv_chart_series_t *ser0;        // 温度图表数据序列1
lv_chart_series_t *ser_1;       // 温度图表数据序列3
lv_timer_t *chat_temp_timer;    // 温度图表定时器
lv_obj_t *chat_temp_enable_switch; // 温度图表使能开关
lv_obj_t *chat_temp_clean_btn;  // 温度曲线重置开关
lv_obj_t *chat_temp_switch1;    // 数据一开关
lv_obj_t *chat_temp_switch2;    // 数据二开关
lv_obj_t *chat_temp_switch3;    // 数据三开关
lv_obj_t *char_temp_x_slider;   // 温度图表X轴缩放滑块
lv_obj_t *char_temp_Y_slider;   // 温度图表Y轴缩放滑块

/* 柱状图相关 */
lv_obj_t *chart2;               // 柱状图
lv_timer_t *chat_bar_timer;     // 柱状图表定时器
lv_chart_series_t *ser2;        // 柱形图数据序列1
lv_chart_series_t *ser3;        // 柱形图数据序列2
lv_obj_t *chat_bar_enable_switch; // 柱状图表使能开关
lv_obj_t *chat_bar_clean_btn;   // 柱状曲线重置开关
lv_obj_t *chat_bar_switch1;     // 柱状数据一开关
lv_obj_t *chat_bar_switch2;     // 柱状数据二开关
lv_obj_t *char_bar_x_slider;    // 柱状图表X轴缩放滑块
lv_obj_t *char_bar_Y_slider;    // 柱状图表Y轴缩放滑块

/* 示例数据 */
lv_coord_t chat_data2[] = {25, 26, 27, 29, 30, 31, 32, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46};
lv_coord_t chat_data1[] = {25, 26, 27, 29, 30, 31, 32, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46};

/* 函数实现 */
void add_data(lv_timer_t *timer)
{
    // 添加温度数据
    lv_chart_set_next_value(chart, ser0, lv_rand(20, 90));
    lv_chart_set_next_value(chart, ser1, lv_rand(20, 90));
    lv_chart_set_next_value(chart, ser_1, lv_rand(20, 90));
}

void add_data2(lv_timer_t *timer)
{
    // 添加柱状图数据
    lv_chart_set_next_value(chart2, ser2, lv_rand(20, 90));
    lv_chart_set_next_value(chart2, ser3, lv_rand(20, 90));
}

void chat_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (obj == chat_temp_clean_btn)
        {
            // 清除温度图表数据
            lv_chart_refresh(chart);
        }
        else if (obj == chat_bar_clean_btn)
        {
            // 清除柱状图数据
            lv_chart_refresh(chart2);
        }
    }
}

void slider_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        if (obj == char_temp_x_slider)
        {
            // 调整温度图表X轴缩放
            int32_t value = lv_slider_get_value(obj);
            lv_chart_set_point_count(chart, value);
        }
        else if (obj == char_temp_Y_slider)
        {
            // 调整温度图表Y轴缩放
            int32_t value = lv_slider_get_value(obj);
            lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, value);
        }
        else if (obj == char_bar_x_slider)
        {
            // 调整柱状图X轴缩放
            int32_t value = lv_slider_get_value(obj);
            lv_chart_set_point_count(chart2, value);
        }
        else if (obj == char_bar_Y_slider)
        {
            // 调整柱状图Y轴缩放
            int32_t value = lv_slider_get_value(obj);
            lv_chart_set_range(chart2, LV_CHART_AXIS_PRIMARY_Y, 0, value);
        }
    }
}

void create_chart_ui(void)
{
    // 创建图表界面背景
    chat_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(chat_bg, 480, 320);
    lv_obj_set_pos(chat_bg, 0, 0);
    lv_obj_set_style_bg_color(chat_bg, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(chat_bg, 0, 0);

    // 创建温度图表
    chart = lv_chart_create(chat_bg);
    lv_obj_set_size(chart, 460, 150);
    lv_obj_set_pos(chart, 10, 10);
    lv_obj_set_style_bg_color(chart, lv_color_hex(0x1a1a1a), 0);
    lv_chart_set_point_count(chart, 20);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_div_line_count(chart, 5, 5);

    // 创建温度图表数据序列
    ser0 = lv_chart_add_series(chart, lv_color_hex(0xff0000), LV_CHART_AXIS_PRIMARY_Y);
    ser1 = lv_chart_add_series(chart, lv_color_hex(0x00ff00), LV_CHART_AXIS_PRIMARY_Y);
    ser_1 = lv_chart_add_series(chart, lv_color_hex(0x0000ff), LV_CHART_AXIS_PRIMARY_Y);

    // 创建温度图表控制区域
    lv_obj_t *temp_ctrl = lv_obj_create(chat_bg);
    lv_obj_set_size(temp_ctrl, 460, 50);
    lv_obj_set_pos(temp_ctrl, 10, 170);
    lv_obj_set_style_bg_color(temp_ctrl, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(temp_ctrl, 0, 0);

    // 创建温度图表使能开关
    chat_temp_enable_switch = lv_switch_create(temp_ctrl);
    lv_obj_set_size(chat_temp_enable_switch, 50, 25);
    lv_obj_set_pos(chat_temp_enable_switch, 10, 12);

    // 创建温度图表清除按钮
    chat_temp_clean_btn = lv_btn_create(temp_ctrl);
    lv_obj_set_size(chat_temp_clean_btn, 80, 30);
    lv_obj_set_pos(chat_temp_clean_btn, 70, 10);
    lv_obj_set_style_bg_color(chat_temp_clean_btn, lv_color_hex(0x2a2a2a), 0);
    lv_obj_add_event_cb(chat_temp_clean_btn, chat_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *clean_label = lv_label_create(chat_temp_clean_btn);
    lv_label_set_text(clean_label, "清除");
    lv_obj_center(clean_label);

    // 创建温度图表X轴缩放滑块
    char_temp_x_slider = lv_slider_create(temp_ctrl);
    lv_obj_set_size(char_temp_x_slider, 100, 10);
    lv_obj_set_pos(char_temp_x_slider, 160, 10);
    lv_slider_set_range(char_temp_x_slider, 10, 50);
    lv_obj_add_event_cb(char_temp_x_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 创建温度图表Y轴缩放滑块
    char_temp_Y_slider = lv_slider_create(temp_ctrl);
    lv_obj_set_size(char_temp_Y_slider, 100, 10);
    lv_obj_set_pos(char_temp_Y_slider, 270, 10);
    lv_slider_set_range(char_temp_Y_slider, 50, 200);
    lv_obj_add_event_cb(char_temp_Y_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 创建柱状图
    chart2 = lv_chart_create(chat_bg);
    lv_obj_set_size(chart2, 460, 150);
    lv_obj_set_pos(chart2, 10, 230);
    lv_obj_set_style_bg_color(chart2, lv_color_hex(0x1a1a1a), 0);
    lv_chart_set_type(chart2, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(chart2, 20);
    lv_chart_set_range(chart2, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_div_line_count(chart2, 5, 5);

    // 创建柱状图数据序列
    ser2 = lv_chart_add_series(chart2, lv_color_hex(0xff0000), LV_CHART_AXIS_PRIMARY_Y);
    ser3 = lv_chart_add_series(chart2, lv_color_hex(0x00ff00), LV_CHART_AXIS_PRIMARY_Y);

    // 创建柱状图控制区域
    lv_obj_t *bar_ctrl = lv_obj_create(chat_bg);
    lv_obj_set_size(bar_ctrl, 460, 50);
    lv_obj_set_pos(bar_ctrl, 10, 390);
    lv_obj_set_style_bg_color(bar_ctrl, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(bar_ctrl, 0, 0);

    // 创建柱状图使能开关
    chat_bar_enable_switch = lv_switch_create(bar_ctrl);
    lv_obj_set_size(chat_bar_enable_switch, 50, 25);
    lv_obj_set_pos(chat_bar_enable_switch, 10, 12);

    // 创建柱状图清除按钮
    chat_bar_clean_btn = lv_btn_create(bar_ctrl);
    lv_obj_set_size(chat_bar_clean_btn, 80, 30);
    lv_obj_set_pos(chat_bar_clean_btn, 70, 10);
    lv_obj_set_style_bg_color(chat_bar_clean_btn, lv_color_hex(0x2a2a2a), 0);
    lv_obj_add_event_cb(chat_bar_clean_btn, chat_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *clean_label2 = lv_label_create(chat_bar_clean_btn);
    lv_label_set_text(clean_label2, "清除");
    lv_obj_center(clean_label2);

    // 创建柱状图X轴缩放滑块
    char_bar_x_slider = lv_slider_create(bar_ctrl);
    lv_obj_set_size(char_bar_x_slider, 100, 10);
    lv_obj_set_pos(char_bar_x_slider, 160, 10);
    lv_slider_set_range(char_bar_x_slider, 10, 50);
    lv_obj_add_event_cb(char_bar_x_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 创建柱状图Y轴缩放滑块
    char_bar_Y_slider = lv_slider_create(bar_ctrl);
    lv_obj_set_size(char_bar_Y_slider, 100, 10);
    lv_obj_set_pos(char_bar_Y_slider, 270, 10);
    lv_slider_set_range(char_bar_Y_slider, 50, 200);
    lv_obj_add_event_cb(char_bar_Y_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 创建定时器
    chat_temp_timer = lv_timer_create(add_data, 1000, NULL);
    chat_bar_timer = lv_timer_create(add_data2, 1000, NULL);
} 
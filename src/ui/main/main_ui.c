#include "main_ui.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_benchmark.h"
#include "FreeRTOS.h"
#include "task.h"

/* 主界面样式 */
lv_style_t main_style;
lv_style_t world_style;

/* 主界面控件 */
lv_obj_t *main_bg;       // 主界面背景
lv_obj_t *main_return;   // 返回按钮
lv_obj_t *main_return_pic; // 返回按钮图标

/* 通知栏相关 */
lv_obj_t *note;          // 下拉通知栏
lv_anim_t note_anim;     // 拉通知栏动画对象
lv_obj_t *note_down;
lv_obj_t *note_down_label;
uint8_t note_symbol = 0; // 通知栏状态

/* 状态图标 */
uint8_t wifi_symbol = 0;
uint8_t GPS_symbol = 0;
uint8_t blue_symbol = 0;

/* WiFi相关控件 */
lv_obj_t *wifi_obj;      // 通知界面WIFI基础对象
lv_obj_t *wifi_pic;      // 通知界面WIFI图标
lv_obj_t *wifi_btn;      // 通知界面WIFI按钮
lv_obj_t *wifi_labal;    // 通知界面WIFI文本
lv_obj_t *wifi_labal2;   // 主界面WIFI开启图案
uint8_t wifi_long_symbol = 0; // WIFI长按

/* 蓝牙相关控件 */
lv_obj_t *blue_obj;      // 通知界面蓝牙基础对象
lv_obj_t *blue_pic;      // 通知界面蓝牙图标
lv_obj_t *blue_btn;      // 通知界面蓝牙按钮
lv_obj_t *blue_labal;    // 通知界面蓝牙文本
lv_obj_t *blue_labal2;   // 主界面蓝牙开启图案

/* GPS相关控件 */
lv_obj_t *GPS_obj;       // 通知界面GPS基础对象
lv_obj_t *GPS_pic;       // 通知界面GPS图标
lv_obj_t *GPS_btn;       // 通知界面GPS按钮
lv_obj_t *GPS_labal;     // 通知界面GPS文本
lv_obj_t *GPS_labal2;    // 主界面GPS开启图案

/* 时间和日期显示 */
lv_obj_t *time_labal;    // 主界时间
lv_obj_t *data_labal;    // 主界面日期
lv_obj_t *time_obj;      // 主界面时间背景
lv_obj_t *data_obj;      // 主界面日期背景

/* 主界面功能按钮 */
lv_obj_t *main_0;        // 主界面设置按钮
lv_obj_t *main_set_pic;  // 主界面设置按钮图标
lv_obj_t *main_text;     // 主界面文本基础对象
lv_obj_t *main_text_label; // 主界面菜单文本框

/* 主界面其他按钮 */
lv_obj_t *main_1;        // 主界面设置按钮
lv_obj_t *main_set_pic1; // 主界面设置按钮图标
lv_obj_t *main_2;        // 主界面设置按钮
lv_obj_t *main_set_pic2; // 主界面设置按钮图标
lv_obj_t *main_3;        // 主界面设置按钮
lv_obj_t *main_set_pic3; // 主界面设置按钮图标
lv_obj_t *main_4;        // 主界面设置按钮
lv_obj_t *main_set_pic4; // 主界面设置按钮图标
lv_obj_t *main_5;        // 主界面设置按钮
lv_obj_t *main_set_pic5; // 主界面设置按钮图标
lv_obj_t *main_6;        // 主界面设置按钮
lv_obj_t *main_set_pic6; // 主界面设置按钮图标
lv_obj_t *main_7;        // 主界面设置按钮
lv_obj_t *main_set_pic7; // 主界面设置按钮图标
lv_obj_t *main_8;        // 主界面设置按钮
lv_obj_t *main_set_pic8; // 主界面设置按钮图标

/* 主界面背景 */
lv_obj_t *main_bg2;
lv_obj_t *main_bg1;

/* 函数实现 */
void note_y(void *obj, int32_t y)
{
    lv_obj_set_y((lv_obj_t *)obj, y);
}

void note_main_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (note_symbol == 0)
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, note);
            lv_anim_set_values(&a, lv_obj_get_y(note), 0);
            lv_anim_set_time(&a, 500);
            lv_anim_set_exec_cb(&a, note_y);
            lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
            lv_anim_start(&a);
            note_symbol = 1;
        }
        else
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, note);
            lv_anim_set_values(&a, lv_obj_get_y(note), -100);
            lv_anim_set_time(&a, 500);
            lv_anim_set_exec_cb(&a, note_y);
            lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
            lv_anim_start(&a);
            note_symbol = 0;
        }
    }
}

void note_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (obj == wifi_btn)
        {
            if (wifi_symbol == 0)
            {
                wifi_symbol = 1;
                lv_obj_set_style_img_recolor_opa(wifi_pic, 255, 0);
                lv_obj_set_style_img_recolor(wifi_pic, lv_color_hex(0x00ff00), 0);
            }
            else
            {
                wifi_symbol = 0;
                lv_obj_set_style_img_recolor_opa(wifi_pic, 0, 0);
            }
        }
        else if (obj == blue_btn)
        {
            if (blue_symbol == 0)
            {
                blue_symbol = 1;
                lv_obj_set_style_img_recolor_opa(blue_pic, 255, 0);
                lv_obj_set_style_img_recolor(blue_pic, lv_color_hex(0x00ff00), 0);
            }
            else
            {
                blue_symbol = 0;
                lv_obj_set_style_img_recolor_opa(blue_pic, 0, 0);
            }
        }
        else if (obj == GPS_btn)
        {
            if (GPS_symbol == 0)
            {
                GPS_symbol = 1;
                lv_obj_set_style_img_recolor_opa(GPS_pic, 255, 0);
                lv_obj_set_style_img_recolor(GPS_pic, lv_color_hex(0x00ff00), 0);
            }
            else
            {
                GPS_symbol = 0;
                lv_obj_set_style_img_recolor_opa(GPS_pic, 0, 0);
            }
        }
    }
}

void main_bg_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (note_symbol == 1)
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, note);
            lv_anim_set_values(&a, lv_obj_get_y(note), -100);
            lv_anim_set_time(&a, 500);
            lv_anim_set_exec_cb(&a, note_y);
            lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
            lv_anim_start(&a);
            note_symbol = 0;
        }
    }
}

void main_btn_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (obj == main_0)
        {
            // 设置按钮点击事件
        }
        else if (obj == main_1)
        {
            // 按钮1点击事件
        }
        else if (obj == main_2)
        {
            // 按钮2点击事件
        }
        else if (obj == main_3)
        {
            // 按钮3点击事件
        }
        else if (obj == main_4)
        {
            // 按钮4点击事件
        }
        else if (obj == main_5)
        {
            // 按钮5点击事件
        }
        else if (obj == main_6)
        {
            // 按钮6点击事件
        }
        else if (obj == main_7)
        {
            // 按钮7点击事件
        }
        else if (obj == main_8)
        {
            // 按钮8点击事件
        }
    }
}

void KZ_MAIN(void)
{
    // 创建主界面背景
    main_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_bg, 480, 320);
    lv_obj_set_pos(main_bg, 0, 0);
    lv_obj_set_style_bg_color(main_bg, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(main_bg, 0, 0);
    lv_obj_add_event_cb(main_bg, main_bg_event, LV_EVENT_CLICKED, NULL);

    // 创建通知栏
    note = lv_obj_create(main_bg);
    lv_obj_set_size(note, 480, 100);
    lv_obj_set_pos(note, 0, -100);
    lv_obj_set_style_bg_color(note, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(note, 0, 0);

    // 创建通知栏下拉按钮
    note_down = lv_btn_create(main_bg);
    lv_obj_set_size(note_down, 480, 20);
    lv_obj_set_pos(note_down, 0, 0);
    lv_obj_set_style_bg_color(note_down, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(note_down, 0, 0);
    lv_obj_add_event_cb(note_down, note_main_event, LV_EVENT_CLICKED, NULL);

    // 创建通知栏下拉按钮文本
    note_down_label = lv_label_create(note_down);
    lv_label_set_text(note_down_label, "▼");
    lv_obj_center(note_down_label);

    // 创建WiFi控件
    wifi_obj = lv_obj_create(note);
    lv_obj_set_size(wifi_obj, 100, 60);
    lv_obj_set_pos(wifi_obj, 20, 20);
    lv_obj_set_style_bg_color(wifi_obj, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_width(wifi_obj, 0, 0);

    wifi_pic = lv_img_create(wifi_obj);
    lv_img_set_src(wifi_pic, &wifi_icon);
    lv_obj_center(wifi_pic);

    wifi_btn = lv_btn_create(wifi_obj);
    lv_obj_set_size(wifi_btn, 100, 60);
    lv_obj_set_pos(wifi_btn, 0, 0);
    lv_obj_set_style_bg_opa(wifi_btn, 0, 0);
    lv_obj_set_style_border_width(wifi_btn, 0, 0);
    lv_obj_add_event_cb(wifi_btn, note_event, LV_EVENT_CLICKED, NULL);

    wifi_labal = lv_label_create(wifi_obj);
    lv_label_set_text(wifi_labal, "WiFi");
    lv_obj_align(wifi_labal, LV_ALIGN_BOTTOM_MID, 0, -5);

    // 创建蓝牙控件
    blue_obj = lv_obj_create(note);
    lv_obj_set_size(blue_obj, 100, 60);
    lv_obj_set_pos(blue_obj, 140, 20);
    lv_obj_set_style_bg_color(blue_obj, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_width(blue_obj, 0, 0);

    blue_pic = lv_img_create(blue_obj);
    lv_img_set_src(blue_pic, &bluetooth_icon);
    lv_obj_center(blue_pic);

    blue_btn = lv_btn_create(blue_obj);
    lv_obj_set_size(blue_btn, 100, 60);
    lv_obj_set_pos(blue_btn, 0, 0);
    lv_obj_set_style_bg_opa(blue_btn, 0, 0);
    lv_obj_set_style_border_width(blue_btn, 0, 0);
    lv_obj_add_event_cb(blue_btn, note_event, LV_EVENT_CLICKED, NULL);

    blue_labal = lv_label_create(blue_obj);
    lv_label_set_text(blue_labal, "蓝牙");
    lv_obj_align(blue_labal, LV_ALIGN_BOTTOM_MID, 0, -5);

    // 创建GPS控件
    GPS_obj = lv_obj_create(note);
    lv_obj_set_size(GPS_obj, 100, 60);
    lv_obj_set_pos(GPS_obj, 260, 20);
    lv_obj_set_style_bg_color(GPS_obj, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_width(GPS_obj, 0, 0);

    GPS_pic = lv_img_create(GPS_obj);
    lv_img_set_src(GPS_pic, &gps_icon);
    lv_obj_center(GPS_pic);

    GPS_btn = lv_btn_create(GPS_obj);
    lv_obj_set_size(GPS_btn, 100, 60);
    lv_obj_set_pos(GPS_btn, 0, 0);
    lv_obj_set_style_bg_opa(GPS_btn, 0, 0);
    lv_obj_set_style_border_width(GPS_btn, 0, 0);
    lv_obj_add_event_cb(GPS_btn, note_event, LV_EVENT_CLICKED, NULL);

    GPS_labal = lv_label_create(GPS_obj);
    lv_label_set_text(GPS_labal, "GPS");
    lv_obj_align(GPS_labal, LV_ALIGN_BOTTOM_MID, 0, -5);

    // 创建时间和日期显示
    time_obj = lv_obj_create(main_bg);
    lv_obj_set_size(time_obj, 120, 40);
    lv_obj_set_pos(time_obj, 20, 20);
    lv_obj_set_style_bg_color(time_obj, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_width(time_obj, 0, 0);

    time_labal = lv_label_create(time_obj);
    lv_label_set_text(time_labal, "12:00");
    lv_obj_center(time_labal);

    data_obj = lv_obj_create(main_bg);
    lv_obj_set_size(data_obj, 120, 40);
    lv_obj_set_pos(data_obj, 20, 70);
    lv_obj_set_style_bg_color(data_obj, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_width(data_obj, 0, 0);

    data_labal = lv_label_create(data_obj);
    lv_label_set_text(data_labal, "2024-01-01");
    lv_obj_center(data_labal);

    // 创建主界面功能按钮
    main_0 = lv_btn_create(main_bg);
    lv_obj_set_size(main_0, 100, 100);
    lv_obj_set_pos(main_0, 20, 120);
    lv_obj_set_style_bg_color(main_0, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_width(main_0, 0, 0);
    lv_obj_add_event_cb(main_0, main_btn_event, LV_EVENT_CLICKED, NULL);

    main_set_pic = lv_img_create(main_0);
    lv_img_set_src(main_set_pic, &settings_icon);
    lv_obj_center(main_set_pic);

    main_text = lv_obj_create(main_0);
    lv_obj_set_size(main_text, 100, 20);
    lv_obj_set_pos(main_text, 0, 80);
    lv_obj_set_style_bg_color(main_text, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_width(main_text, 0, 0);

    main_text_label = lv_label_create(main_text);
    lv_label_set_text(main_text_label, "设置");
    lv_obj_center(main_text_label);

    // 创建其他功能按钮
    // ... 创建main_1到main_8的按钮，布局类似main_0
} 
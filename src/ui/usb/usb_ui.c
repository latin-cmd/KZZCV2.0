#include "usb_ui.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_benchmark.h"
#include "FreeRTOS.h"
#include "task.h"

/* USB界面相关 */
lv_obj_t *usb_bg;               // USB界面背景
lv_obj_t *usb_tileview;         // USB平铺视图
lv_obj_t *usb_tile_1;           // USB平铺视图页面1
lv_obj_t *usb_link_success_label; // USB连接成功标签
lv_timer_t *usb_check_timer;    // USB检查定时器
lv_obj_t *usb_link_symbol;      // USB连接符号
lv_obj_t *usb_link_success_win2; // USB连接成功窗口2
lv_obj_t *usb_link_success_label2; // USB连接成功标签2

/* 函数实现 */
void usb_check_timer_cb(lv_timer_t *timer)
{
    // 检查USB连接状态
    if (USB_IsConnected())
    {
        // USB已连接
        lv_obj_clear_flag(usb_link_success_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(usb_link_symbol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(usb_link_success_win2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(usb_link_success_label2, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        // USB未连接
        lv_obj_add_flag(usb_link_success_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(usb_link_symbol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(usb_link_success_win2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(usb_link_success_label2, LV_OBJ_FLAG_HIDDEN);
    }
}

void create_usb_ui(void)
{
    // 创建USB界面背景
    usb_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(usb_bg, 480, 320);
    lv_obj_set_pos(usb_bg, 0, 0);
    lv_obj_set_style_bg_color(usb_bg, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(usb_bg, 0, 0);

    // 创建USB平铺视图
    usb_tileview = lv_tileview_create(usb_bg);
    lv_obj_set_size(usb_tileview, 480, 320);
    lv_obj_set_pos(usb_tileview, 0, 0);
    lv_obj_set_style_bg_color(usb_tileview, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(usb_tileview, 0, 0);

    // 创建USB平铺视图页面1
    usb_tile_1 = lv_tileview_add_tile(usb_tileview, 0, 0, LV_DIR_ALL);

    // 创建USB连接成功标签
    usb_link_success_label = lv_label_create(usb_tile_1);
    lv_obj_set_pos(usb_link_success_label, 10, 10);
    lv_label_set_text(usb_link_success_label, "USB已连接");
    lv_obj_set_style_text_color(usb_link_success_label, lv_color_hex(0x00ff00), 0);
    lv_obj_add_flag(usb_link_success_label, LV_OBJ_FLAG_HIDDEN);

    // 创建USB连接符号
    usb_link_symbol = lv_label_create(usb_tile_1);
    lv_obj_set_pos(usb_link_symbol, 10, 40);
    lv_label_set_text(usb_link_symbol, "●");
    lv_obj_set_style_text_color(usb_link_symbol, lv_color_hex(0x00ff00), 0);
    lv_obj_add_flag(usb_link_symbol, LV_OBJ_FLAG_HIDDEN);

    // 创建USB连接成功窗口2
    usb_link_success_win2 = lv_obj_create(usb_tile_1);
    lv_obj_set_size(usb_link_success_win2, 200, 100);
    lv_obj_set_pos(usb_link_success_win2, 140, 110);
    lv_obj_set_style_bg_color(usb_link_success_win2, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(usb_link_success_win2, 0, 0);
    lv_obj_add_flag(usb_link_success_win2, LV_OBJ_FLAG_HIDDEN);

    // 创建USB连接成功标签2
    usb_link_success_label2 = lv_label_create(usb_link_success_win2);
    lv_obj_center(usb_link_success_label2);
    lv_label_set_text(usb_link_success_label2, "USB已连接");
    lv_obj_set_style_text_color(usb_link_success_label2, lv_color_hex(0x00ff00), 0);
    lv_obj_add_flag(usb_link_success_label2, LV_OBJ_FLAG_HIDDEN);

    // 创建USB检查定时器
    usb_check_timer = lv_timer_create(usb_check_timer_cb, 1000, NULL);
} 
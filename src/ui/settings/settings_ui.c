#include "settings_ui.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_benchmark.h"
#include "FreeRTOS.h"
#include "task.h"

/* 设置界面控件 */
lv_obj_t *setting_tabview;  // 设置界面选项卡
lv_obj_t *setting_bg;       // 设置界面背景
lv_obj_t *setting_bg2;      // 设置界面右边背景
lv_obj_t *list;             // 设置界面左边选项列表
lv_obj_t *setting_bg2_label; // 设置界面右边背景文本
lv_obj_t *btn;              // 设置界面列表按钮
lv_obj_t *freertos;         // 实时操作系统图片
lv_obj_t *lvgl;             // LVGL图片

/* 硬件信息界面线条 */
lv_point_t hardware_line[] = { {0, 0}, {420, 0}, };

/* 函数实现 */
void list_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        // 获取按钮索引
        uint32_t id = lv_obj_get_index(obj);
        
        // 根据索引切换不同的设置页面
        switch (id)
        {
            case 0: // 系统信息
                lv_label_set_text(setting_bg2_label, "系统信息");
                break;
            case 1: // 硬件信息
                lv_label_set_text(setting_bg2_label, "硬件信息");
                break;
            case 2: // 软件信息
                lv_label_set_text(setting_bg2_label, "软件信息");
                break;
            case 3: // 关于
                lv_label_set_text(setting_bg2_label, "关于");
                break;
            default:
                break;
        }
    }
}

void create_settings_ui(void)
{
    // 创建设置界面背景
    setting_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(setting_bg, 480, 320);
    lv_obj_set_pos(setting_bg, 0, 0);
    lv_obj_set_style_bg_color(setting_bg, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(setting_bg, 0, 0);

    // 创建左侧列表
    list = lv_list_create(setting_bg);
    lv_obj_set_size(list, 200, 320);
    lv_obj_set_pos(list, 0, 0);
    lv_obj_set_style_bg_color(list, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(list, 0, 0);

    // 创建列表按钮
    btn = lv_list_add_btn(list, LV_SYMBOL_SETTINGS, "系统信息");
    lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_btn(list, LV_SYMBOL_DRIVE, "硬件信息");
    lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_btn(list, LV_SYMBOL_EDIT, "软件信息");
    lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_btn(list, LV_SYMBOL_INFO, "关于");
    lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);

    // 创建右侧背景
    setting_bg2 = lv_obj_create(setting_bg);
    lv_obj_set_size(setting_bg2, 280, 320);
    lv_obj_set_pos(setting_bg2, 200, 0);
    lv_obj_set_style_bg_color(setting_bg2, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_width(setting_bg2, 0, 0);

    // 创建右侧标题
    setting_bg2_label = lv_label_create(setting_bg2);
    lv_label_set_text(setting_bg2_label, "系统信息");
    lv_obj_align(setting_bg2_label, LV_ALIGN_TOP_MID, 0, 20);

    // 创建FreeRTOS图标
    freertos = lv_img_create(setting_bg2);
    lv_img_set_src(freertos, &freertos_icon);
    lv_obj_align(freertos, LV_ALIGN_CENTER, -50, 0);

    // 创建LVGL图标
    lvgl = lv_img_create(setting_bg2);
    lv_img_set_src(lvgl, &lvgl_icon);
    lv_obj_align(lvgl, LV_ALIGN_CENTER, 50, 0);
} 
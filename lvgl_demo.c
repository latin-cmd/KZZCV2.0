#include "lvgl_demo.h"
#include "./BSP/LED/led.h"
#include "./BSP/TOUCH/touch.h"
#include "FreeRTOS.h"
#include "task.h"

#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_benchmark.h"

#include "queue.h"
#include "semphr.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"
#include "usbd_conf.h"

#include "./MALLOC/malloc.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"

#include "./BSP/ADC/adc3.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

//FreeRTOS资源句柄
QueueHandle_t uart_rx_queue;  //uart接收中断队列

QueueHandle_t temp_queue;  //内部温度传感器队列

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO     1           /* 任务优先级 */
#define START_STK_SIZE      128         /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;         /* 任务句柄 */
void start_task(void *pvParameters);    /* 任务函数 */

/* LV_DEMO_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define LV_DEMO_TASK_PRIO   3           /* 任务优先级 */
#define LV_DEMO_STK_SIZE    4096        /* 任务堆栈大小 */
TaskHandle_t LV_DEMOTask_Handler;       /* 任务句柄 */
void lv_demo_task(void *pvParameters);  /* 任务函数 */

/* LED_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define LED_TASK_PRIO       4           /* 任务优先级 */
#define LED_STK_SIZE        128         /* 任务堆栈大小 */
TaskHandle_t LEDTask_Handler;           /* 任务句柄 */
void led_task(void *pvParameters);      /* 任务函数 */

/* ADC3_TEMP_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
//#define LED_TASK_PRIO       4           /* 任务优先级 */
//#define LED_STK_SIZE        128         /* 任务堆栈大小 */
TaskHandle_t ADC3Task_Handler;           /* 任务句柄 */
void ADC3_task(void *pvParameters);      /* 任务函数 */
/******************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//USBD_HandleTypeDef USBD_Device;             /* USB Device处理结构体 */
extern volatile uint8_t g_usb_state_reg;    /* USB状态 */
extern volatile uint8_t g_device_state;     /* USB连接 情况 */


//LV_FONT_DECLARE(NLVFONT12);
LV_FONT_DECLARE(W1);
LV_FONT_DECLARE(NLVFONT12_05);

//字体颜色---------------------------------------------------------------//
static lv_style_t world_style;
//------------------------------------------------------------------------//

//主界面控件--------------------------------------------------------------//
static lv_style_t main_style; /* 主界面按钮基本样式变量 */

static lv_timer_t * my_timer;
static lv_obj_t*main_bg;       //主界面背景

static lv_obj_t *main_return;               //返回按钮
static lv_obj_t *main_return_pic;           //返回按钮图标
//-----------------------------------------------------
static lv_obj_t*note;       //下拉通知栏
static lv_anim_t note_anim;             // 拉通知栏动画对象
static lv_obj_t*note_down;
static lv_obj_t *note_down_label;
static uint8_t note_symbol = 0; // 通知栏状态

static uint8_t wifi_symbol=0;
static uint8_t GPS_symbol=0;
static uint8_t blue_symbol=0;

static lv_obj_t*wifi_obj;       //通知界面WIFI基础对象
static lv_obj_t*wifi_pic;       //通知界面WIFI图标
static lv_obj_t*wifi_btn;       //通知界面WIFI按钮
static lv_obj_t*wifi_labal;   //通知界面WIFI文本
static lv_obj_t*wifi_labal2;    //主界面WIFI开启图案
static uint8_t wifi_long_symbol;    //WIFI长按

static lv_obj_t*wifi_in_obj;       //WIFI登录界面
static lv_obj_t *keyboard1;          /* 键盘 */
static lv_obj_t *label_name1;        /* 用户名正误提示标签 */
static lv_obj_t *label_pass1;        /* 密码正误提示标签 */

static lv_obj_t*blue_obj;       //通知界面蓝牙基础对象
static lv_obj_t*blue_pic;       //通知界面蓝牙图标
static lv_obj_t*blue_btn;       //通知界面蓝牙按钮
static lv_obj_t*blue_labal;   //通知界面蓝牙文本
static lv_obj_t*blue_labal2;    //主界面蓝牙开启图案

static lv_obj_t*GPS_obj;       //通知界面GPS基础对象
static lv_obj_t*GPS_pic;       //通知界面GPS图标
static lv_obj_t*GPS_btn;       //通知界面GPS按钮
static lv_obj_t*GPS_labal;   //通知界面GPS文本
static lv_obj_t*GPS_labal2;    //主界面GPS开启图案

static lv_obj_t * sound_slider;//音量调节
static lv_obj_t * light_slider;//音量调节

static lv_obj_t*time_labal;    //主界时间
static lv_obj_t*data_labal;    //主界面日期

static lv_obj_t*time_obj;    //主界面时间背景
static lv_obj_t*data_obj;    //主界面日期背景
//---------------------------------------------------------
static lv_obj_t*main_0;       //主界面设置按钮
static lv_obj_t*main_set_pic;   //主界面设置按钮图标

static lv_obj_t*main_0;       //主界面设置按钮
static lv_obj_t*main_set_pic;   //主界面设置按钮图标

static lv_obj_t*main_0;       //主界面设置按钮
static lv_obj_t*main_set_pic;   //主界面设置按钮图标

static lv_obj_t*main_0;       //主界面设置按钮
static lv_obj_t*main_set_pic;   //主界面设置按钮图标
//-------------------------------------------------------
static lv_obj_t*main_0;       //主界面设置按钮
static lv_obj_t*main_set_pic;   //主界面设置按钮图标

static lv_obj_t*main_text;       //主界面文本基础对象
static lv_obj_t*main_text_label; //主界面菜单文本框

static lv_obj_t*main_1;       //主界面设置按钮
static lv_obj_t*main_set_pic1;   //主界面设置按钮图标

static lv_obj_t*main_2;       //主界面设置按钮
static lv_obj_t*main_set_pic2;   //主界面设置按钮图标

static lv_obj_t*main_3;       //主界面设置按钮
static lv_obj_t*main_set_pic3;   //主界面设置按钮图标

static lv_obj_t*main_4;       //主界面设置按钮
static lv_obj_t*main_set_pic4;   //主界面设置按钮图标

static lv_obj_t*main_5;       //主界面设置按钮
static lv_obj_t*main_set_pic5;   //主界面设置按钮图标

static lv_obj_t*main_6;       //主界面设置按钮
static lv_obj_t*main_set_pic6;   //主界面设置按钮图标

static lv_obj_t*main_7;       //主界面设置按钮
static lv_obj_t*main_set_pic7;   //主界面设置按钮图标

static lv_obj_t*main_8;       //主界面设置按钮
static lv_obj_t*main_set_pic8;   //主界面设置按钮图标
static lv_obj_t*main_bg2;
static lv_obj_t*main_bg1;
//---------------------------------------------------
static lv_obj_t*setting_tabview;//设置界面选项卡
static lv_obj_t*setting_bg;//设置界面背景
static lv_obj_t*setting_bg2;//设置界面右边背景
static lv_obj_t*list;//设置界面左边选项列表
static lv_obj_t *setting_bg2_label;//设置界面右边背景文本
static lv_obj_t* btn;//设置界面列表按钮
static lv_obj_t*freertos;//实时操作系统图片
static lv_obj_t*lvgl;//LVGL图片
static lv_point_t hardware_line[] = { {0, 0}, {420, 0},};/* 为硬件信息界面的点创建线条数组 */

static lv_obj_t *uart_tabview;//uart选项卡
static lv_obj_t *uart_btnm;//串口助手界面按钮矩阵
static lv_obj_t*uart_bg;//UART界面
static lv_obj_t *uart_init_btn;
static lv_obj_t *uart_date_roller;
static lv_obj_t *baundrate_roller;
static lv_obj_t *uart_stop_roller;
static lv_obj_t *uart_parity_roller;
static lv_obj_t *uart_mode_roller;
static lv_obj_t *uart_it_flag_roller;
static lv_obj_t *uart_it_pr_roller;
static lv_obj_t *uart_it_enable_switch;
static lv_obj_t *uart_enable_switch;
static const char *baudrate_options =  "1200\n2400\n""4800\n""9600\n""19200\n""38400\n""115200";
static const char *uart_date_options =  "7\n8\n""9";
static const char *uart_parity_options =  "Odd Parity\nEven Parity\n""No Parity";
static const char *uart_stop_options =  "0.5\n1\n""1.5\n""2";
static const char *uart_mode_options =  "TX\n""RX\n""TX/RX";
static const char *uart_it_flag_options =  "TX\n""RX\n""TX/RX";
static const char *uart_it_pr_options =  "0\n""1\n""2\n""3\n""4\n""5\n""6\n""7\n""8\n""9\n""10\n""11\n""12\n""13\n""14\n""15";
static const char * uart_btnm_map[] = { "0","1","2","3","4","5","6","7","8","9","\n","A","B","C","D","E","F","Del","\n","a","b","c","d","e","f",",","__","" };
static lv_obj_t *uart_get_clean_btn;//消息接收区域清除按钮
static lv_obj_t *uart_send_clean_btn;//消息发送区域清除按钮
static lv_obj_t *uart_send_btn;//消息发送按钮
static lv_obj_t *uart_off_switch;//串口开关
static lv_obj_t *uart_send2_label;//串口发送区域文本框
static lv_obj_t *uart_get_label;//串口接收区域文本框

static lv_obj_t*chat_bg;//图表界面
static lv_obj_t* chart;//温度图表
static lv_chart_series_t* ser1;//温度图表数据序列2
static lv_chart_series_t* ser0;//温度图表数据序列1
static lv_chart_series_t* ser_1;//温度图表数据序列3
static lv_timer_t * chat_temp_timer;//温度图表定时器
static lv_obj_t*chat_temp_enable_switch;//温度图表使能开关
static lv_obj_t*chat_temp_clean_btn;//温度曲线重置开关
static lv_obj_t*chat_temp_switch1;//数据一开关
static lv_obj_t*chat_temp_switch2;//数据二开关
static lv_obj_t*chat_temp_switch3;//数据三开关
static lv_obj_t*char_temp_x_slider;//温度图表X轴缩放滑块
static lv_obj_t*char_temp_Y_slider;//温度图表Y轴缩放滑块

static lv_obj_t*chart2;
static lv_timer_t * chat_bar_timer;//柱状图表定时器
static lv_chart_series_t* ser2;//柱形图数据序列1
static lv_chart_series_t* ser3;//柱形图数据序列2
static lv_obj_t*chat_bar_enable_switch;//柱状图表使能开关
static lv_obj_t*chat_bar_clean_btn;//柱状曲线重置开关
static lv_obj_t*chat_bar_switch1;//柱状数据一开关
static lv_obj_t*chat_bar_switch2;//柱状数据二开关
static lv_obj_t*char_bar_x_slider;//柱状图表X轴缩放滑块
static lv_obj_t*char_bar_Y_slider;//柱状图表Y轴缩放滑块
static  lv_coord_t chat_data2[] = {25, 26, 27, 29, 30, 31, 32, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,46,};/* 示例数据 */
static  lv_coord_t chat_data1[] = {25, 26, 27, 29, 30, 31, 32, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,46,};/* 示例数据 */

static lv_obj_t*usb_bg;//usb界面
static lv_obj_t *usb_tileview;//usb界面
static lv_obj_t *usb_tile_1;//usb平铺视图1
static lv_obj_t*usb_link_success_label;//usb连接成功文本图标
static lv_timer_t *usb_check_timer;//usb连接检测定时器
static uint8_t usb_link_symbol=1;//usb连接是否成功标志位
static lv_obj_t*usb_link_success_win2;//USB读写状态背景
static lv_obj_t *usb_link_success_label2;//USB读写状态标题
static lv_obj_t *usb_card_restart_btn;//USB读卡器复位按钮

static lv_obj_t*iot_bg;//iot物联网界面
static lv_obj_t *iot_tileview;//iot物联网界面平铺视图
static lv_obj_t *AC_temp_label;//空调界面温度显示
static lv_obj_t *AC_time_label;//空调定时界面显示
static lv_point_t line_points[] = { {0, 0}, {180, 0} };/* 空调界面线条数组 */
static lv_point_t line_points3[] = { {0, 0}, {100, 0} };/* 空调界面线条数组 */
static lv_point_t line_points2[] = { {0, 0}, {0, 100} };/* 空调界面线条数组 */
static const char *AC_mode_options =  "Auto\n""Cool\n""Dru\n""Heat\n""Fan\n";

static lv_obj_t*timer_bg;//定时器界面
static uint32_t elapsed_time;  // 计时器时间（单位：毫秒）
static uint32_t stopwatch_symbol1;  // 计时器启停标志位
static lv_obj_t*timer_tileview;//定时器界面平铺视图
static lv_obj_t*tile_1;
static lv_obj_t*stopwatch_meter;//秒表仪表（秒）
static lv_obj_t*stopwatch_meter1;//秒表仪表（分）
static lv_meter_scale_t* stopwwatch_scale;//秒刻度
static lv_meter_scale_t* stopwwatch_scale1;//分刻度
static lv_meter_indicator_t* stopwwatch_indic1;//秒指针
static lv_meter_indicator_t* stopwwatch_indic11;//分指针
static lv_obj_t*stopwatch_star_btn;//秒表启停按钮
static lv_obj_t *stopwatch_star_label;//秒表启停按钮文本
static lv_obj_t*stopwatch_reset_btn;//秒表重置按钮
static lv_obj_t*stopwatch_reset_label;//秒表重置按钮文本
static lv_obj_t*stopwatch_data_btn;//秒表数据纪录按钮
static lv_obj_t*stopwatch_get_label;//秒表数据纪录按钮文本
static lv_obj_t*stopwatch_time_label;//秒表计时文本
static lv_obj_t*stopwatch_time_right_bg;//秒表记录栏
static lv_timer_t *stopwatch_timer;//秒表定时器
static lv_obj_t *stopwatch_get_1_bg;//秒表记录栏1号位
static lv_obj_t *stopwatch_get_1_label;
static lv_obj_t *stopwatch_get_2_bg;//秒表记录栏2号位
static lv_obj_t *stopwatch_get_2_label;
static lv_obj_t *stopwatch_get_3_bg;//秒表记录栏3号位
static lv_obj_t *stopwatch_get_3_label;
static lv_obj_t *stopwatch_get_4_bg;//秒表记录栏4号位
static lv_obj_t *stopwatch_get_4_label;
static uint8_t stopwatch_get_number;

static uint32_t a=0;//秒指针转动标记
static uint32_t b=1;//秒指针循环标记
static uint32_t c=0;//分指针转动标记
static uint32_t d=1;//分指针循环标记

static lv_obj_t*tile_2;//计时器页面
static lv_timer_t * timer_timer;//计时器定时器
static lv_obj_t* timer_arc;//计时器进度显示圆弧
static lv_obj_t*timer_star_btn;//计时器启停按钮
static lv_obj_t *timer_star_label;//计时器启停按钮文本
static lv_obj_t*timer_reset_btn;//计时器重置按钮
static lv_obj_t*timer_reset_label;//秒表重置按钮文本
static lv_obj_t*timer_data_btn;//计时器数据纪录按钮
static lv_obj_t*timer_get_label;//计时器数据纪录按钮文本
static lv_obj_t*timer_time_label;//计时器倒计时文本
static lv_obj_t*hour_roller;//计时器小时设置滚轮
static lv_obj_t*min_roller;//计时器小时设置滚轮
static lv_obj_t*second_roller;//计时器秒设置滚轮
static const char *timer_options =  "00\n""01\n""02\n""03\n""04\n""05\n""06\n""07\n""08\n""09\n""10\n""11\n""12\n""13\n""14\n""15\n""16\n""17\n""18\n""19\n""20\n""21\n""22\n""23\n""24\n""25\n""26\n""27\n""28\n""29\n""30\n""31\n""32\n""33\n""34\n""35\n""36\n""37\n""38\n""39\n""40\n""41\n""42\n""43\n""44\n""45\n""46\n""47\n""48\n""49\n""50\n""51\n""52\n""53\n""54\n""55\n""56\n""57\n""58\n""59\n""60";
static uint32_t timer_elapsed_time;  // 计时器倒计时（单位：秒）
static uint32_t timer_begin;  // 计时器倒计时（单位：秒）
static uint8_t timer_symbol1;  // 计时器启停标志位1
static uint8_t timer_symbol2;  // 计时器启停标志位2

static lv_obj_t*tile_3;

static lv_obj_t*file_bg;//file界面

static uint8_t wifi_return_biaozhi[1]={0};
static uint8_t main_btn_symbol[8]={0,0,0,0,0,0,0,0};
//------------------------------------------------------------------------//
///回调事件----------------------------------------------------------///
//iot界面回调函数--------------------------------------------------------//

//usb界面回调函数--------------------------------------------------------//
static void usb_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */

    if(target==usb_card_restart_btn)
    {

    }
}
//USB读卡器定时器--------------------------------------------------------//
static void usb_check_timer_event(lv_event_t *e)
{
    LV_UNUSED(e);


//        //USB正在写入数据----------------------------------------------------------------------------------------------
//	    if (g_usb_state_reg & 0x01)                                       /* 正在写 */
//        {
//        lv_obj_set_size(usb_link_success_win2, 220, 30);/* 设置大小 */
//        lv_label_set_text(usb_link_success_label2, "USB Writing...");
//		}
//		else if(g_usb_state_reg & 0x02)
//		{
//        //USB正在读出数据----------------------------------------------------------------------------------------------
//        lv_obj_set_size(usb_link_success_win2, 220, 30);/* 设置大小 */
//        lv_label_set_text(usb_link_success_label2, "USB Reading...");
//		}
//		else if(g_usb_state_reg & 0x04)
//		{
//        //USB写数据错误----------------------------------------------------------------------------------------------
//        lv_obj_set_size(usb_link_success_win2, 220, 30);/* 设置大小 */
//        lv_label_set_text(usb_link_success_label2, "USB Write Err");
//		}
//		else if(g_usb_state_reg & 0x08)
//		{
//        //USB读数据错误----------------------------------------------------------------------------------------------
//        lv_obj_set_size(usb_link_success_win2, 220, 30);/* 设置大小 */
//        lv_label_set_text(usb_link_success_label2, "USB Read  Err");
//        lv_obj_set_style_text_color(usb_link_success_label2,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
//		}
//		else if(g_device_state == 1)
//		{
//        //USB已经连接----------------------------------------------------------------------------------------------
//        lv_obj_set_size(usb_link_success_win2, 220, 30);/* 设置大小 */
//        lv_label_set_text(usb_link_success_label2, "USB Connected");
//        lv_obj_set_style_text_color(usb_link_success_label2,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
//        }
//	    else if(g_device_state != 1)
//		{
//        //USB已经被拔出----------------------------------------------------------------------------------------------
//        lv_obj_set_size(usb_link_success_win2, 220, 30);/* 设置大小 */
//        lv_label_set_text(usb_link_success_label2, "USB DisConnected");
//		}

}
//串口界面回调函数-------------------------------------------------------//
//串口助手界面按钮回调函数
static void uart_btn_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */
    if(target==uart_get_clean_btn)
    {
        lv_textarea_set_text(uart_get_label, "");
    }
    else if(target==uart_send_clean_btn)
    {
        lv_textarea_set_text(uart_send2_label, "");
    }
    else if(target==uart_send_btn)
    {

        const char*uart_trans=lv_textarea_get_text(uart_send2_label);
        printf("%s",uart_trans);
    }
    else if(target==uart_off_switch)
    {

    }
    else if(target==uart_btnm)
    {
       if (code == LV_EVENT_VALUE_CHANGED)
        {
            uint8_t id;
            id = lv_btnmatrix_get_selected_btn(target); /* 获取按钮索引 */
            /* 更新输入框标签文本 */
            if(id==16)
            {
                lv_textarea_del_char(uart_send2_label);
            }
            else if(id==24)
            {
                lv_textarea_add_text(uart_send2_label," ");
            }
            else
            {
                lv_textarea_add_text(uart_send2_label,lv_btnmatrix_get_btn_text(target, id));

            }

        }
    }
}
//计时器回调函数---------------------------------------------------------//
static void timer_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */
    char time_hour_str[3];
    char time_min_str[3];
    char time_sec_str[3];
    if(target==timer_star_btn)
    {
        if(timer_symbol1==0)
        {
            lv_label_set_text(timer_star_label,LV_SYMBOL_PAUSE );
            timer_symbol1=1;
            timer_symbol2=1;
            lv_roller_get_selected_str(hour_roller,time_hour_str,3);
            lv_roller_get_selected_str(min_roller,time_min_str,3);
            lv_roller_get_selected_str(second_roller,time_sec_str,3);

            /* 将字符串转换为整数 */
            int hours = atoi(time_hour_str);
            int minutes = atoi(time_min_str);
            int seconds = atoi(time_sec_str);

            /* 计算总秒数 */
            timer_elapsed_time = hours * 3600 + minutes * 60 + seconds;
            timer_begin=timer_elapsed_time;
            lv_timer_resume(timer_timer);
            printf("Timer set to %d seconds\n", timer_elapsed_time);  // 调试输出
        }
        else if(timer_symbol2==1)
        {
            lv_label_set_text(timer_star_label,LV_SYMBOL_PLAY );
            lv_timer_pause(timer_timer);
            timer_symbol2=0;
        }
        else if(timer_symbol2==0)
        {
            lv_label_set_text(timer_star_label,LV_SYMBOL_PAUSE );
            lv_timer_resume(timer_timer);
            timer_symbol2=1;
        }

    }
    else if(target==timer_reset_btn)
    {
        timer_symbol1=0;
        lv_timer_pause(timer_timer);
         lv_label_set_text(timer_star_label,LV_SYMBOL_PLAY );
        lv_label_set_text(timer_time_label, "00:00:00");
        lv_arc_set_value( timer_arc, 720); /* 设置  timer_arc 的值 */
    }
}
//计时器定时器------------------------------------------------------------//
static void timer_timer_event(lv_timer_t* timer)
{
    uint16_t arc;//计时器倒计时圆弧指示器值
    LV_UNUSED(timer);
    char time_timer_str[9];
    /* 计算时间，并转换为 "HH:MM:SS" 格式 */
    snprintf(time_timer_str, sizeof(time_timer_str), "%02d:%02d:%02d",
             (timer_elapsed_time / 3600) % 100,   // 小时
             (timer_elapsed_time / 60) % 60,      // 分钟
             timer_elapsed_time % 60);            // 秒
    /* 更新标签显示 */
    lv_label_set_text(timer_time_label, time_timer_str);

    /* 计算圆弧进度，防止除法导致0 */
     arc = (uint16_t)(((float)timer_elapsed_time / timer_begin) * 720.0f);
    lv_arc_set_value( timer_arc, arc); /* 设置  timer_arc 的值 */

    if (timer_elapsed_time == 0)
    {
        lv_timer_pause(timer_timer);
    }
    else
    {
        timer_elapsed_time--;
    }
}
//秒表回调函数------------------------------------------------------------//
static void stopwatch_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */



    if(target==stopwatch_star_btn)
    {
        if(stopwatch_symbol1==0)
        {
            lv_timer_resume(stopwatch_timer);
            lv_label_set_text(stopwatch_star_label,LV_SYMBOL_PAUSE );
            stopwatch_symbol1=1;
        }
        else
        {
            lv_timer_pause(stopwatch_timer);
            lv_label_set_text(stopwatch_star_label,LV_SYMBOL_PLAY );
            stopwatch_symbol1=0;
        }

    }
    else if(target==stopwatch_reset_btn)
    {
        lv_timer_pause(stopwatch_timer);
        stopwatch_symbol1=0;

        a=0;//秒指针转动标记
        b=1;//秒指针循环标记
        c=0;//分指针转动标记
        d=1;//分指针循环标记
        elapsed_time=0;
        lv_meter_set_indicator_value(stopwatch_meter, stopwwatch_indic1,0);/* 设置指针指向的数值 */
        lv_meter_set_indicator_value(stopwatch_meter1, stopwwatch_indic11,0);/* 设置指针指向的数值 */
        lv_label_set_text(stopwatch_time_label, "00:00:00");
        lv_label_set_text(stopwatch_get_1_label,"" );
        lv_label_set_text(stopwatch_get_2_label,"" );
        lv_label_set_text(stopwatch_get_3_label,"" );
        lv_label_set_text(stopwatch_get_4_label,"" );
    }
    else if(target==stopwatch_data_btn)
    {
        if(stopwatch_get_number==0)
        {
            lv_label_set_text(stopwatch_get_1_label,lv_label_get_text(stopwatch_time_label) );
            stopwatch_get_number=1;
        }
        else if(stopwatch_get_number==1)
        {
            lv_label_set_text(stopwatch_get_2_label,lv_label_get_text(stopwatch_time_label) );
            stopwatch_get_number=2;
        }
        else if(stopwatch_get_number==2)
        {
            lv_label_set_text(stopwatch_get_3_label,lv_label_get_text(stopwatch_time_label) );
            stopwatch_get_number=3;
        }
        else if(stopwatch_get_number==3)
        {
            lv_label_set_text(stopwatch_get_4_label,lv_label_get_text(stopwatch_time_label) );
            stopwatch_get_number=0;
        }
    }
}
//秒表定时器-------------------------------------------------------------//
static void stopwatch_timer_event(lv_timer_t* timer)
{
     LV_UNUSED(timer);
     elapsed_time += 10;  // 每次增加10ms
     // 转换为格式化的字符串 (例如：mm:ss:ms)
     char time_str[9];
     snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d",
             (elapsed_time / 60000) % 60,       // 分钟
             (elapsed_time / 1000) % 60,        // 秒
             (elapsed_time / 10) % 100);        // 毫秒 (步进单位是10ms)
     lv_label_set_text(stopwatch_time_label, time_str);

     if(elapsed_time-1000==a)
     {
         if(b>60)b=1;
         a=elapsed_time;
         lv_meter_set_indicator_value(stopwatch_meter, stopwwatch_indic1,b++);/* 设置指针指向的数值 */
     }
     if(elapsed_time-1000*60==c)
     {
         if(d>60)d=1;
         c=elapsed_time;
         lv_meter_set_indicator_value(stopwatch_meter1, stopwwatch_indic11,d++);/* 设置指针指向的数值 */
     }
}
//设置界面文本回调函数---------------------------------------------------//

static void list_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *list_btn = lv_event_get_target(e);                                   /* 获取触发源 */
    //char*a;
    lv_list_get_btn_text(list, list_btn);

    lv_obj_set_size(freertos,0,0);
    lv_obj_set_size(lvgl,0,0);

    if (strcmp(lv_list_get_btn_text(list, list_btn), "Dev Notes") == 0)
        {
            lv_obj_set_size(setting_bg2_label,260,480);
            lv_label_set_text(setting_bg2_label, "      This device is a multifunctional human-machine interaction debugging platform based on the STM32H7 series, combined with the FreeRTOS real-time operating system and the LVGL lightweight embedded graphical interface.\n    The intellectual property owner of this device: Guangdong University of Technology, Department of Electronic Information Engineering, He Junfan.");           /* 获取按钮文本并显示 */
        }
    else if(strcmp(lv_list_get_btn_text(list, list_btn), "Hardware Information")==0)
        {
            lv_obj_set_size(setting_bg2_label,260,480);
            lv_label_set_text(setting_bg2_label, "#FF0000 MCU:# STM32H7IIT6\n\n#FF0000 SDRAM:# W9825G6KH-6 #1f4287 32MB#\n\n#FF0000 SPI Flash:# W25Q256JVEIQ #1f4287 32MB#\n\n#FF0000 NAND Flash:# MT29F4G08ABADAWP:D #1f4287 512MB#\n\n#FF0000 IIC Eeprom:# PT24C02 #1f4287 256Byte#\n\n#FF0000 SCREEN:# WKS35HV024 3.5'TFTLCD (480X320)\n\n#FF0000 LDO:# AMS1117\n\n#FF0000 LSE:# 25MHZ\n\n#FF0000 LSI:# 32.768KHZ");
        }
    else if(strcmp(lv_list_get_btn_text(list, list_btn), "Firmware Information")==0)
        {
            lv_obj_set_size(setting_bg2_label,260,320);
            lv_label_set_text(setting_bg2_label, "\n\n\n#FF0000 Software version:# Control center V2.0\n\n#FF0000 FreeRTOS version:# V9.0\n\n#FF0000 LVGL version:# V8.2");

            lv_obj_set_size(freertos,100,50);
            lv_obj_set_size(lvgl,100,50);
            //lv_obj_t*freertos = lv_img_create(setting_bg2);
            //lv_img_set_src(freertos,&seraphine);//jpg 11.2K以内,png 11.2K以内
            lv_obj_align(freertos,LV_ALIGN_CENTER,0,20);

            //lv_obj_t*lvgl = lv_img_create(setting_bg2);
            //lv_img_set_src(lvgl,&seraphine);//jpg 11.2K以内,png 11.2K以内
            lv_obj_align(lvgl,LV_ALIGN_BOTTOM_MID,0,20);
        }
    else if(strcmp(lv_list_get_btn_text(list, list_btn), "USB")==0)
        {
            lv_obj_set_size(setting_bg2_label,260,700);
            lv_label_set_text(setting_bg2_label, "      The STM32H743 series chips come with 2 USB OTG ports. USB1 is a high-speed USB (USB1 OTG HS), and USB2 is a full-speed USB (USB2 OTG FS). The high-speed USB (HS) requires an external high-speed PHY chip, and this device uses the FS full-speed USB.\n       The STM32H743's USB OTG FS is a dual-role device (DRD) controller, supporting both peripheral and host functions, fully compliant with the USB 2.0 On-The-Go Supplementary Specification. Additionally, the controller can be configured for hostonly mode or peripheral-only mode, fully conforming to the USB 2.0 standard. In host mode, the OTG FS supports full-speed (FS, 12 Mb/s) and low-speed (LS, 1.5 Mb/s) transceivers, while in peripheral mode, it only supports full-speed (FS, 12 Mb/s) transceivers. OTG FS also supports HNP and SRP.\n       The main features of the STM32H743's USB OTG FS can be classified into three categories: general features, host mode features, and peripheral mode features.");
        }
    else if(strcmp(lv_list_get_btn_text(list, list_btn), "GPS"))
        {
            lv_label_set_text(setting_bg2_label, "");
        }
    else if(strcmp(lv_list_get_btn_text(list, list_btn), "UART"))
        {
            lv_label_set_text(setting_bg2_label, "");
        }
    else if(strcmp(lv_list_get_btn_text(list, list_btn), "CAN"))
        {
            lv_label_set_text(setting_bg2_label, "");
        }
    else if(strcmp(lv_list_get_btn_text(list, list_btn), "IOT"))
        {
            lv_label_set_text(setting_bg2_label, "");
        }
    lv_obj_add_state(list_btn, LV_STATE_FOCUS_KEY);                                /* 添加状态（聚焦） */
}
//
//图表-温度读取定时器
static void add_data(lv_timer_t* timer)
{
    LV_UNUSED(timer);

    if(lv_obj_has_state(chat_temp_switch3, LV_STATE_CHECKED)==1)
    {
        lv_chart_set_next_value(chart, ser_1, lv_rand(0, 100));
    }
    if(lv_obj_has_state(chat_temp_switch2, LV_STATE_CHECKED)==1)
    {
        lv_chart_set_next_value(chart,ser1, lv_rand(20, 40));
    }
    if(lv_obj_has_state(chat_temp_switch1, LV_STATE_CHECKED)==1)
    {
        lv_chart_set_next_value(chart,ser0, lv_rand(40, 60));
    }

}


static void add_data2(lv_timer_t* timer)
{
    LV_UNUSED(timer);
    static uint8_t a=0;
    static uint8_t b=0;

    if(lv_obj_has_state(chat_bar_switch1, LV_STATE_CHECKED)==1)
    {
        chat_data2[a]=lv_rand(0, 100);
        lv_chart_set_ext_y_array(chart2, ser2, (lv_coord_t*)chat_data2); /* 为 Y 轴数据点设置一个外部阵列 */
        a++;
    }
    if(lv_obj_has_state(chat_bar_switch2, LV_STATE_CHECKED)==1)
    {
        chat_data1[b]=lv_rand(0, 100);
        lv_chart_set_ext_y_array(chart2, ser3, (lv_coord_t*)chat_data1); /* 为 Y 轴数据点设置一个外部阵列 */
        b++;
    }
        lv_chart_refresh(chart2);/* 更新图表 */
        if(a>18)a=0;
        if(b>18)b=0;
}
static void chat_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */

    if(target==chat_temp_enable_switch)            //启动或者停止图表更新数据/
    {
        if(lv_obj_has_state(chat_temp_enable_switch, LV_STATE_CHECKED)==1)
        {
             lv_timer_resume(chat_temp_timer);
        }
        else
        {
            lv_timer_pause(chat_temp_timer);
        }
    }
    else if(target==chat_bar_enable_switch)
    {
        if(lv_obj_has_state(chat_bar_enable_switch, LV_STATE_CHECKED)==1)
        {
             lv_timer_resume(chat_bar_timer);
        }
        else
        {
            lv_timer_pause(chat_bar_timer);
        }
    }
    else if(target==chat_temp_clean_btn)
    {
        lv_timer_pause(chat_temp_timer);
        lv_chart_set_point_count(chart, 0);
        lv_chart_set_point_count(chart, 20);
        if(lv_obj_has_state(chat_temp_enable_switch, LV_STATE_CHECKED)==1)
        {
             lv_timer_resume(chat_temp_timer);
        }
        else
        {
            lv_timer_pause(chat_temp_timer);
        }
    }
    else if(target==chat_bar_clean_btn)
    {
        uint8_t a=0;
        //lv_timer_pause(chat_bar_timer);
        for(a;a<20;a++)
        {
             chat_data2[a]=0;
             chat_data1[a]=0;
        }
        lv_chart_set_ext_y_array(chart2, ser2, (lv_coord_t*)chat_data2); /* 为 Y 轴数据点设置一个外部阵列 */
        lv_chart_set_ext_y_array(chart2, ser3, (lv_coord_t*)chat_data1); /* 为 Y 轴数据点设置一个外部阵列 */
        lv_chart_refresh(chart2);/* 更新图表 */
    }
}

//图表缩放滑块回调函数
static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */

    if(target==char_temp_x_slider)
    {
        lv_chart_set_zoom_x(chart,lv_slider_get_value(target));
    }
    else if(target==char_temp_Y_slider)
    {
        lv_chart_set_zoom_y(chart,lv_slider_get_value(target));
    }
    else if(target==char_bar_x_slider)
    {
        lv_chart_set_zoom_x(chart2,lv_slider_get_value(target));
    }
    else if(target==char_bar_Y_slider)
    {
        lv_chart_set_zoom_y(chart2,lv_slider_get_value(target));
    }
}

//返回按钮回调函数-------------------------------------------------------//
static void moto_close_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);

        if(wifi_return_biaozhi[0]==1)
        {
            lv_obj_del(wifi_in_obj);
            wifi_return_biaozhi[0]=0;
        }
        else if(main_btn_symbol[0]==1)
        {
            main_btn_symbol[0]=0;
            lv_obj_del( setting_bg);
            lv_obj_align(main_return, LV_ALIGN_TOP_RIGHT, 20, 0);                              /* 设置按钮位置 */
        }
        else if(main_btn_symbol[1]==1)
        {
            main_btn_symbol[1]=0;
            lv_obj_del(uart_tabview);

        }
        else if(main_btn_symbol[2]==1)
        {
            main_btn_symbol[2]=0;
            lv_timer_pause(chat_temp_timer);
            lv_timer_pause(chat_bar_timer);
			vTaskSuspend(ADC3Task_Handler);
            lv_obj_del(chat_bg);
        }
        else if(main_btn_symbol[3]==1)
        {
            main_btn_symbol[3]=0;
            lv_timer_pause( usb_check_timer);
            lv_obj_del(usb_tileview);
			//USBD_Stop(&USBD_Device);
        }
        else if(main_btn_symbol[4]==1)
        {
            main_btn_symbol[4]=0;
            lv_obj_del(iot_tileview);
        }
        else if(main_btn_symbol[5]==1)
        {
            main_btn_symbol[5]=0;
            lv_timer_pause(stopwatch_timer);
            stopwatch_symbol1=0;
            a=0;//秒指针转动标记
            b=1;//秒指针循环标记
            c=0;//分指针转动标记
            d=1;//分指针循环标记
            elapsed_time=0;
            lv_obj_del(timer_bg);
            //-------------------
            timer_symbol1=0;  // 计时器启停标志位1
            timer_symbol2=0; // 计时器启停标志位2
            lv_timer_pause(timer_timer);
        }
        else if(main_btn_symbol[6]==1)
        {
            main_btn_symbol[6]=0;
        }
        lv_obj_add_flag( main_return,LV_OBJ_FLAG_HIDDEN);//主界面隐藏返回按钮
}
//WIFI密码回调函数-----------------------------------------------------//
static void wifi_pass_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);            /* 获取事件类型 */
    lv_obj_t *target = lv_event_get_target(e);              /* 获取触发源 */

    if(code == LV_EVENT_FOCUSED)                            /* 事件类型：被聚焦 */
    {
        lv_keyboard_set_textarea(keyboard1, target);         /* 关联用户名文本框和键盘 */
    }
    else if(code == LV_EVENT_VALUE_CHANGED)                 /* 事件类型：文本框的内容发生变化 */
    {
        const char *txt = lv_textarea_get_text(target);     /* 获取文本框的文本 */

        if(strcmp(txt,"123456") == 0)                       /* 判断密码是否正确 */
        {
            lv_label_set_text(label_pass1, LV_SYMBOL_OK);    /* 密码正确，显示√ */
        }
        else
        {
            lv_label_set_text(label_pass1, LV_SYMBOL_CLOSE);              /* 密码错误，不提示 */
        }
    }
}
//WIFI名字回调函数--------------------------------------------------------------//
static void wifi_name_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);            /* 获取事件类型 */
    lv_obj_t *target = lv_event_get_target(e);              /* 获取触发源 */

    if(code == LV_EVENT_FOCUSED)                            /* 事件类型：被聚焦 */
    {
        lv_keyboard_set_textarea(keyboard1, target);         /* 关联用户名文本框和键盘 */
    }
    else if(code == LV_EVENT_VALUE_CHANGED)                 /* 事件类型：文本框的内容发生变化 */
    {
        const char *txt = lv_textarea_get_text(target);     /* 获取文本框的文本 */

        if(strcmp(txt,"admin") == 0)                        /* 判断用户名是否正确 */
        {
            lv_label_set_text(label_name1, LV_SYMBOL_OK);    /* 用户名正确，显示√ */
        }
        else
        {
            lv_label_set_text(label_name1, LV_SYMBOL_CLOSE);              /* 用户名错误，不提示 */
        }
    }
}

//动画回调函数-----------------------------------------------------//

// 状态栏动画回调函数：设置通知栏的 Y 坐标------------------------//
static void note_y(void *obj, int32_t y)
{
    lv_obj_set_y((lv_obj_t *)obj, y);
    lv_obj_set_y(note_down, y+293);
}
//状态栏二级界面回调函数------------------------------------------//
static void note_main_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */
    if(code==LV_EVENT_LONG_PRESSED)
    {

            lv_obj_clear_flag(main_return,LV_OBJ_FLAG_HIDDEN);//显示返回上一级按钮
            wifi_return_biaozhi[0]=1;


            wifi_in_obj=lv_obj_create (lv_scr_act());                                          /* 创建主界背景 */
            lv_obj_set_size(wifi_in_obj, 480, 320);                                            /* 设置主界背景大小 */
            lv_obj_set_style_bg_color(wifi_in_obj,lv_color_hex(0X1f4287),LV_PART_MAIN);
            lv_obj_set_style_border_width(wifi_in_obj,0,LV_PART_MAIN);
            lv_obj_set_scrollbar_mode(wifi_in_obj, LV_SCROLLBAR_MODE_OFF);                     /* 隐藏滚动条 */
            lv_obj_clear_flag(wifi_in_obj, LV_OBJ_FLAG_SCROLL_ELASTIC);                        // 禁用父对象 A 的弹性滚动
            lv_obj_set_scroll_dir(wifi_in_obj, LV_DIR_NONE);                                   // 仅允许水平方向滚动


         /* 用户名文本框 */
            lv_obj_t *textarea_name = lv_textarea_create(wifi_in_obj);                         /* 创建文本框 */
            lv_obj_set_width(textarea_name, 240);                                              /* 设置宽度 */
            lv_obj_set_style_text_font(textarea_name, &lv_font_montserrat_20, LV_PART_MAIN);                                  /* 设置字体 */
            lv_obj_align(textarea_name, LV_ALIGN_CENTER, -90, -110 );                          /* 设置位置 */
            lv_textarea_set_one_line(textarea_name, true);                                     /* 设置单行模式 */
            lv_textarea_set_max_length(textarea_name, 6);                                      /* 设置输入字符的最大长度 */
            lv_textarea_set_placeholder_text(textarea_name, "Wi-Fi Name");                     /* 设置占位符 */
            lv_obj_add_event_cb(textarea_name, wifi_name_event_cb, LV_EVENT_ALL, NULL);        /* 添加文本框事件回调 */

            /* 用户名正误提示标签 */
            label_name1 = lv_label_create(wifi_in_obj);                                        /* 创建标签 */
            lv_label_set_text(label_name1, "");                                                /* 默认不提示 */
            lv_obj_set_style_text_font(label_name1, &lv_font_montserrat_20, LV_PART_MAIN);                                     /* 设置字体 */
            lv_obj_align_to(label_name1, textarea_name, LV_ALIGN_OUT_RIGHT_MID, 5, 0);         /* 设置位置 */

            /* 密码文本框 */
            lv_obj_t *textarea_pass = lv_textarea_create(wifi_in_obj);                         /* 创建文本框 */
            lv_obj_set_width(textarea_pass, 240);                                              /* 设置宽度 */
            lv_obj_set_style_text_font(textarea_pass, &lv_font_montserrat_20, LV_PART_MAIN);                                  /* 设置字体 */
            lv_obj_align_to(textarea_pass, textarea_name, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);     /* 设置位置 */
            lv_textarea_set_one_line(textarea_pass, true);                                     /* 设置单行模式 */
            lv_textarea_set_password_mode(textarea_pass, true);                                /* 设置密码模式 */
            lv_textarea_set_password_show_time(textarea_pass, 1000);                           /* 设置密码显示时间 */
            lv_textarea_set_max_length(textarea_pass, 8);                                      /* 设置输入字符的最大长度 */
            lv_textarea_set_placeholder_text(textarea_pass, "Password");                       /* 设置占位符 */
            lv_obj_add_event_cb(textarea_pass, wifi_pass_event_cb, LV_EVENT_ALL, NULL);        /* 添加文本框事件回调 */

            /* 密码正误提示标签 */
            label_pass1 = lv_label_create(wifi_in_obj);                                        /* 创建标签 */
            lv_label_set_text(label_pass1, "");                                                /* 默认不提示 */
            lv_obj_set_style_text_font(label_pass1, &lv_font_montserrat_20, LV_PART_MAIN);                                     /* 设置字体 */
            lv_obj_align_to(label_pass1, textarea_pass, LV_ALIGN_OUT_RIGHT_MID, 5, 0);         /* 设置位置 */

            /* 键盘 */
            keyboard1 = lv_keyboard_create(wifi_in_obj);                                       /* 创建键盘 */
            lv_obj_set_size(keyboard1, 480, 160);                                              /* 设置大小 */
            lv_obj_set_pos(keyboard1,0,10);

            lv_obj_set_style_pad_left(keyboard1,0,LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(keyboard1,0,LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(keyboard1,0,LV_STATE_DEFAULT);

            lv_obj_set_scrollbar_mode(wifi_in_obj, LV_SCROLLBAR_MODE_OFF);

            lv_obj_t*sign_in=lv_btn_create(wifi_in_obj);
            lv_obj_set_size(sign_in,120,60);
            lv_obj_set_pos(sign_in,300,30);
            //lv_obj_set_style_bg_color(sign_in,lv_color_hex(0XFFFFFF),LV_PART_MAIN);
            lv_obj_set_style_bg_color(sign_in,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
            lv_obj_set_style_bg_opa(sign_in,LV_OPA_100,LV_STATE_DEFAULT);
            lv_obj_set_style_radius(sign_in,100,LV_STATE_DEFAULT);

            lv_obj_t *sign_in_label = lv_label_create(sign_in);
            lv_label_set_text(sign_in_label,"Sign In ");
            lv_obj_set_style_text_color(sign_in_label,lv_color_hex(0X1f4287),LV_PART_MAIN);
            lv_obj_set_style_text_font(sign_in_label,&lv_font_montserrat_26, LV_STATE_DEFAULT);
            lv_obj_align(sign_in_label, LV_ALIGN_CENTER, 0,0 );

            lv_obj_move_foreground(main_return);            //把返回按钮设置到图层顶端

            wifi_long_symbol=1;
    }

    if(code==LV_EVENT_RELEASED)
    {

        if(wifi_long_symbol==1)
        {

               wifi_long_symbol=0;
        }
        else if(target==wifi_btn)
        {
            if(wifi_symbol==0)
            {
                wifi_labal2 = lv_label_create(lv_scr_act());
                lv_label_set_text(wifi_labal2,LV_SYMBOL_WIFI );
                lv_obj_set_style_text_font(wifi_labal2,&lv_font_montserrat_26, LV_STATE_DEFAULT);
                lv_obj_align(wifi_labal2, LV_ALIGN_TOP_RIGHT, -10,5 );
                lv_obj_set_style_text_color(wifi_labal2,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
                lv_obj_refresh_style(wifi_labal2, LV_PART_ANY, LV_STYLE_PROP_ANY);
                lv_obj_set_style_bg_color(wifi_btn,lv_color_hex(0x0000FF),LV_STATE_DEFAULT);
                wifi_symbol=1;

            }
            else
            {
                lv_obj_del(wifi_labal2);
                lv_obj_set_style_bg_color(wifi_btn,lv_color_hex(0x808080),LV_STATE_DEFAULT);
                wifi_symbol=0;
            }
        }
        else if(target==blue_btn)
        {
            if(blue_symbol==0)
            {
                blue_labal2 = lv_label_create(lv_scr_act());
                lv_label_set_text(blue_labal2,LV_SYMBOL_BLUETOOTH );
                lv_obj_set_style_text_font(blue_labal2,&lv_font_montserrat_26, LV_STATE_DEFAULT);
                lv_obj_align(blue_labal2, LV_ALIGN_TOP_RIGHT, -55,5 );
                lv_obj_set_style_text_color(blue_labal2,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
                lv_obj_refresh_style(blue_labal2, LV_PART_ANY, LV_STYLE_PROP_ANY);
                lv_obj_set_style_bg_color(blue_btn,lv_color_hex(0x0000FF),LV_STATE_DEFAULT);

                blue_symbol=1;

            }
            else
            {
                lv_obj_del(blue_labal2);
                lv_obj_set_style_bg_color(blue_btn,lv_color_hex(0x808080),LV_STATE_DEFAULT);
                blue_symbol=0;
            }

        }
        else if(target==GPS_btn)
        {
            if(GPS_symbol==0)
            {
                GPS_labal2 = lv_label_create(lv_scr_act());
                lv_label_set_text(GPS_labal2,LV_SYMBOL_GPS );
                lv_obj_set_style_text_font(GPS_labal2,&lv_font_montserrat_26, LV_STATE_DEFAULT);
                lv_obj_align(GPS_labal2, LV_ALIGN_TOP_RIGHT, -90,5 );
                lv_obj_set_style_text_color(GPS_labal2,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
                lv_obj_refresh_style(GPS_labal2, LV_PART_ANY, LV_STYLE_PROP_ANY);
                lv_obj_set_style_bg_color(GPS_btn,lv_color_hex(0x0000FF),LV_STATE_DEFAULT);

                GPS_symbol=1;

            }
            else
            {
                lv_obj_del(GPS_labal2);
                lv_obj_set_style_bg_color(GPS_btn,lv_color_hex(0x808080),LV_STATE_DEFAULT);
                GPS_symbol=0;
            }
        }
      }

}
//创建下拉任务栏回调函数------------------------------------------------------//
static void note_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */

    if(code==LV_EVENT_CLICKED)
   {
        if(note_symbol == 0)
        {
        //打开通知栏
        lv_anim_set_values(&note_anim, -320, -25); // 从顶部滑到 Y=0
        lv_label_set_text(note_down_label,LV_SYMBOL_UP );
        lv_anim_start(&note_anim);
        note_symbol = 1;
    //WIFI按钮-----------------------------------------------------------------------------------------//

        wifi_obj=lv_obj_create(note);
        lv_obj_align(wifi_obj,LV_ALIGN_TOP_LEFT,0,0);
        lv_obj_set_size(wifi_obj, 190, 65);                                          /* 设置主界背景大小 */
        lv_obj_set_style_radius(wifi_obj,50,LV_PART_MAIN);
        lv_obj_set_style_bg_color(wifi_obj,lv_color_hex(0xeeeeee),LV_PART_MAIN);
        lv_obj_set_scroll_dir(wifi_obj, LV_DIR_NONE );  // 仅允许水平方向滚动

        wifi_btn= lv_btn_create(wifi_obj);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(wifi_btn, 50, 50);
        lv_obj_set_style_radius(wifi_btn,25,LV_PART_MAIN);                                         /* 设置按钮大小 */
        lv_obj_align(wifi_btn,  LV_ALIGN_LEFT_MID, -10, 0);
        lv_obj_add_event_cb(wifi_btn, note_main_event, LV_EVENT_RELEASED, NULL);
        lv_obj_add_event_cb(wifi_btn, note_main_event,  LV_EVENT_LONG_PRESSED, NULL);
        lv_obj_set_style_bg_color(wifi_btn,lv_color_hex(0x808080),LV_STATE_DEFAULT);
        //lv_indev_set_long_press_repeat_time(indev, 300); // 设置长按后，每 300ms 触发一次 LV_EVENT_LONG_PRESSED_REPEAT

        if(wifi_symbol==0)
        {
            lv_obj_set_style_bg_color(wifi_btn,lv_color_hex(0x808080),LV_STATE_DEFAULT);
        }
        else
        {
            lv_obj_set_style_bg_color(wifi_btn,lv_color_hex(0x0000FF),LV_STATE_DEFAULT);
        }

        wifi_pic = lv_label_create(wifi_btn);
        lv_label_set_text(wifi_pic,LV_SYMBOL_WIFI );
        lv_obj_set_style_text_font(wifi_pic,&lv_font_montserrat_26, LV_STATE_DEFAULT);
        lv_obj_align(wifi_pic, LV_ALIGN_CENTER, 0,0 );

        wifi_labal = lv_label_create( wifi_obj);
        lv_label_set_text(wifi_labal,"WLAN" );
        lv_obj_set_style_text_font(wifi_labal,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(wifi_labal, LV_ALIGN_RIGHT_MID, -40,0 );

    //蓝牙按钮----------------------------------------------------------------------------------------//

        blue_obj=lv_obj_create(note);
        lv_obj_align(blue_obj,LV_ALIGN_TOP_LEFT,0,80);
        lv_obj_set_size(blue_obj, 190, 65);                                          /* 设置主界背景大小 */
        lv_obj_set_style_radius(blue_obj,50,LV_PART_MAIN);
        lv_obj_set_style_bg_color(blue_obj,lv_color_hex(0xeeeeee),LV_PART_MAIN);
        lv_obj_set_scroll_dir(blue_obj, LV_DIR_NONE );  // 仅允许水平方向滚动

        blue_btn= lv_btn_create(blue_obj);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(blue_btn, 50, 50);
        lv_obj_set_style_radius(blue_btn,25,LV_PART_MAIN);                                         /* 设置按钮大小 */
        lv_obj_align(blue_btn,  LV_ALIGN_LEFT_MID, -10, 0);
        lv_obj_add_event_cb(blue_btn, note_main_event, LV_EVENT_RELEASED, NULL);
        lv_obj_set_style_bg_color(blue_btn,lv_color_hex(0x808080),LV_STATE_DEFAULT);

        if(blue_symbol==0)
        {
            lv_obj_set_style_bg_color(blue_btn,lv_color_hex(0x808080),LV_STATE_DEFAULT);
        }
        else
        {
            lv_obj_set_style_bg_color(blue_btn,lv_color_hex(0x0000FF),LV_STATE_DEFAULT);
        }

        blue_pic = lv_label_create(blue_btn);
        lv_label_set_text(blue_pic,LV_SYMBOL_BLUETOOTH );
        lv_obj_set_style_text_font(blue_pic,&lv_font_montserrat_26, LV_STATE_DEFAULT);
        lv_obj_align(blue_pic, LV_ALIGN_CENTER, 0,0 );

        blue_labal = lv_label_create( blue_obj);
        lv_label_set_text(blue_labal,"Bluetooth" );
        lv_obj_set_style_text_font(blue_labal,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(blue_labal, LV_ALIGN_RIGHT_MID, 0,0 );
    //GPS按钮-------------------------------------------------------------------------//

        GPS_obj=lv_obj_create(note);
        lv_obj_align(GPS_obj,LV_ALIGN_TOP_LEFT,0,160);
        lv_obj_set_size(GPS_obj, 190, 65);                                          /* 设置主界背景大小 */
        lv_obj_set_style_radius(GPS_obj,50,LV_PART_MAIN);
        lv_obj_set_style_bg_color(GPS_obj,lv_color_hex(0xeeeeee),LV_PART_MAIN);
        lv_obj_set_scroll_dir(GPS_obj, LV_DIR_NONE );  // 仅允许水平方向滚动

        GPS_btn= lv_btn_create(GPS_obj);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(GPS_btn, 50, 50);
        lv_obj_set_style_radius(GPS_btn,25,LV_PART_MAIN);                                         /* 设置按钮大小 */
        lv_obj_align(GPS_btn,  LV_ALIGN_LEFT_MID, -10, 0);
        lv_obj_add_event_cb(GPS_btn, note_main_event, LV_EVENT_RELEASED, NULL);
        lv_obj_set_style_bg_color(GPS_btn,lv_color_hex(0x808080),LV_STATE_DEFAULT);

        if(GPS_symbol==0)
        {
            lv_obj_set_style_bg_color(GPS_btn,lv_color_hex(0x808080),LV_STATE_DEFAULT);
        }
        else
        {
            lv_obj_set_style_bg_color(GPS_btn,lv_color_hex(0x0000FF),LV_STATE_DEFAULT);
        }

        GPS_pic = lv_label_create(GPS_btn);
        lv_label_set_text(GPS_pic,LV_SYMBOL_GPS );
        lv_obj_set_style_text_font(GPS_pic,&lv_font_montserrat_26, LV_STATE_DEFAULT);
        lv_obj_align(GPS_pic, LV_ALIGN_CENTER, 0,0 );

        GPS_labal = lv_label_create( GPS_obj);
        lv_label_set_text(GPS_labal,"GPS" );
        lv_obj_set_style_text_font(GPS_labal,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(GPS_labal, LV_ALIGN_RIGHT_MID, -60,0 );

    //音量图标------------------------------------------------------------------------//
        /* 滑块 */
        sound_slider = lv_slider_create(note);                             /* 创建滑块 */
        lv_obj_set_size(sound_slider, 70, 180);                               /* 设置大小 */
        lv_obj_align(sound_slider, LV_ALIGN_CENTER, 160,-10 );                                                         /* 设置位置 */
        lv_slider_set_value(sound_slider, 50, LV_ANIM_OFF);                                   /* 设置当前值 */
        lv_slider_set_range(sound_slider,0,100);/* 设置 sound_slider 部件范围值 */
        lv_obj_set_style_bg_opa(sound_slider, LV_OPA_0, LV_PART_KNOB);
        lv_obj_set_style_border_opa(sound_slider, LV_OPA_0, LV_PART_KNOB);

        lv_obj_set_style_bg_color(sound_slider, lv_color_hex(0xFFA500), LV_PART_INDICATOR);
        lv_obj_set_style_radius(sound_slider, 0, LV_PART_INDICATOR);
        //lv_obj_add_event_cb(sound_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 添加事件 */


        lv_obj_t *sound_label = lv_label_create(note);                          /* 创建音量标签 */
        lv_label_set_text(sound_label, LV_SYMBOL_VOLUME_MAX);                           /* 设置文本内容：音量图标 */
        lv_obj_set_style_text_font(sound_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);                /* 设置字体 */
        lv_obj_align_to(sound_label, sound_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);            /* 设置位置 */
    //亮度调节------------------------------------------------------------------------//
        /* 滑块 */
        light_slider = lv_slider_create(note);                             /* 创建滑块 */
        lv_obj_set_size(light_slider, 70, 180);                               /* 设置大小 */
        lv_obj_align(light_slider, LV_ALIGN_CENTER, 60,-10 );                                                         /* 设置位置 */
        lv_slider_set_value(light_slider, 50, LV_ANIM_OFF);                                   /* 设置当前值 */
        lv_slider_set_range(light_slider,0,100);/* 设置 light_slider 部件范围值 */
        lv_obj_set_style_bg_opa(light_slider, LV_OPA_0, LV_PART_KNOB);
        lv_obj_set_style_border_opa(sound_slider, LV_OPA_0, LV_PART_KNOB);
        lv_obj_set_style_bg_color(light_slider, lv_color_hex(0xFFA500), LV_PART_INDICATOR);
        lv_obj_set_style_radius(light_slider, 0, LV_PART_INDICATOR);
        //lv_obj_add_event_cb(light_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 添加事件 */


        lv_obj_t *light_label = lv_label_create(note);                          /* 创建音量标签 */
        lv_label_set_text(light_label, LV_SYMBOL_CHARGE);                           /* 设置文本内容：音量图标 */
        lv_obj_set_style_text_font(light_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);                /* 设置字体 */
        lv_obj_align_to(light_label, light_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);            /* 设置位置 */

        }
        else
        {
           lv_anim_set_values(&note_anim, -25,-290); // 从顶部滑到 Y=0
           lv_label_set_text(note_down_label,LV_SYMBOL_DOWN );
           lv_anim_start(&note_anim);
           note_symbol = 0;

           lv_obj_clean(note);
        }
   }
}

//-----------------------------------------------------------------//
//主界面背景回调事件-----------------------------------------------//
static void main_bg_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */
    if(code == LV_EVENT_SCROLL_END)
    {
      lv_timer_pause( my_timer);
       int move;
	   move=lv_obj_get_scroll_x(main_bg);
      if(move>0&&move<90)
      {
           lv_obj_scroll_to(main_bg, 0, 0, LV_ANIM_ON);
          lv_obj_set_style_shadow_width(main_0, 60, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
          lv_obj_set_style_shadow_spread(main_0,5,LV_STATE_DEFAULT);
          lv_label_set_text(main_text_label,"Setting" );
      }
      else if(move>=90&&move<240)
      {
          lv_obj_scroll_to(main_bg, 160, 0, LV_ANIM_ON);
          lv_obj_set_style_shadow_width(main_1, 60, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
          lv_obj_set_style_shadow_spread(main_1,5,LV_STATE_DEFAULT);
          lv_label_set_text(main_text_label,"UART" );
      }
      else if(move>=240&&move<400)
      {
          lv_obj_scroll_to(main_bg, 320, 0, LV_ANIM_ON);
          lv_obj_set_style_shadow_width(main_2, 60, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
          lv_obj_set_style_shadow_spread(main_2,5,LV_STATE_DEFAULT);
          lv_label_set_text(main_text_label,"Chart" );
      }
      else if(move>=400&&move<560)
      {
          lv_obj_scroll_to(main_bg, 480, 0, LV_ANIM_ON);
          lv_obj_set_style_shadow_width(main_3, 60, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
          lv_obj_set_style_shadow_spread(main_3,5,LV_STATE_DEFAULT);
          lv_label_set_text(main_text_label,"USB" );
      }
      else if(move>=560&&move<720)
      {
          lv_obj_scroll_to(main_bg, 640, 0, LV_ANIM_ON);
          lv_obj_set_style_shadow_width(main_4, 60, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
          lv_obj_set_style_shadow_spread(main_4,5,LV_STATE_DEFAULT);
          lv_label_set_text(main_text_label,"IOT" );
      }
     else if(move>=720&&move<880)
      {
          lv_obj_scroll_to(main_bg, 800, 0, LV_ANIM_ON);
          lv_obj_set_style_shadow_width(main_5, 60, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
          lv_obj_set_style_shadow_spread(main_5,5,LV_STATE_DEFAULT);
          lv_label_set_text(main_text_label,"Timer" );
      }
     else if(move>=880)
      {
          lv_obj_scroll_to(main_bg, 960, 0, LV_ANIM_ON);
          lv_obj_set_style_shadow_width(main_6, 60, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
          lv_obj_set_style_shadow_spread(main_6,5,LV_STATE_DEFAULT);
          lv_label_set_text(main_text_label,"FILE" );
      }

    }
    else if (code == LV_EVENT_SCROLL_BEGIN)
    {
        lv_timer_resume(my_timer);

        lv_obj_set_style_shadow_width(main_6, 0, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
        lv_obj_set_style_shadow_spread(main_6,0,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(main_5, 0, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
        lv_obj_set_style_shadow_spread(main_5,0,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(main_4, 0, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
        lv_obj_set_style_shadow_spread(main_4,0,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(main_3, 0, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
        lv_obj_set_style_shadow_spread(main_3,0,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(main_2, 0, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
        lv_obj_set_style_shadow_spread(main_2,0,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(main_1, 0, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
        lv_obj_set_style_shadow_spread(main_1,0,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(main_0, 0, LV_STATE_DEFAULT); // 设置阴影宽度为 10 像素
        lv_obj_set_style_shadow_spread(main_0,0,LV_STATE_DEFAULT);
    }

}

//主要功能界面回调函数---------------------------------------------//
static void main_btn_event(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */

    if(target==main_0)                             //设置界面选项卡
    {
        lv_obj_clear_flag(main_return,LV_OBJ_FLAG_HIDDEN);//显示返回上一级按钮
        lv_obj_clear_state(main_return,LV_STATE_DISABLED);

        main_btn_symbol[0]=1;


        //设置界面背景-----------------------------------------------------------------------------------//
        setting_bg=lv_obj_create (lv_scr_act());                                               /* 创建主界背景 */
        lv_obj_set_size(setting_bg, 480, 320);                                                 /* 设置主界背景大小 */
        lv_obj_set_style_radius(setting_bg,0,LV_PART_MAIN);
        lv_obj_set_style_bg_color(setting_bg,lv_color_hex(0XD3D3D3),LV_PART_MAIN);
        lv_obj_set_style_border_width(setting_bg,0,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(setting_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(setting_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(setting_bg,  LV_DIR_NONE);                                         // 仅允许水平方向滚动

        //创建右侧背景-----------------------------------------------------------------------------------//
        setting_bg2=lv_obj_create (setting_bg);                                               /* 创建主界背景 */
        lv_obj_set_size(setting_bg2, 300, 320);                                                 /* 设置主界背景大小 */
        lv_obj_align(setting_bg2,LV_ALIGN_RIGHT_MID,14,0);
        lv_obj_set_style_bg_color(setting_bg2,lv_color_hex(0XFFFFFF),LV_PART_MAIN);
        lv_obj_set_style_border_width(setting_bg2,2,LV_PART_MAIN);
        lv_obj_set_style_border_color(setting_bg2,lv_color_hex(0XC0C0C0),LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(setting_bg2, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(setting_bg2, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(setting_bg2,  LV_DIR_VER);                                         // 仅允许水平方向滚动

        //创建右侧背景文本------------------------------------------------------------------------------//

        setting_bg2_label = lv_label_create(setting_bg2);
        lv_obj_set_size(setting_bg2_label,260,480);
        //lv_label_set_text(setting_bg2_label,LV_SYMBOL_LIST );
        lv_obj_set_style_text_font(setting_bg2_label,&lv_font_montserrat_16, LV_STATE_DEFAULT);
        lv_obj_align(setting_bg2_label, LV_ALIGN_TOP_MID, 0,0 );
        lv_label_set_recolor(setting_bg2_label, true);

        //创建列表------------------------------------------------------------------------------------//
        list = lv_list_create(setting_bg);                                                /* 创建列表 */
        lv_obj_set_width(list, 160);                       /* 设置列表宽度 */
        lv_obj_set_height(list,320);                     /* 设置列表高度 */
        lv_obj_align(list,LV_ALIGN_OUT_LEFT_TOP,-10,-5);                                                            /* 设置列表的位置 */
        lv_obj_set_style_text_font(list, &lv_font_montserrat_16, LV_PART_MAIN);                           /* 设置字体 */

        /* 为列表添加按钮 */
        lv_list_add_text(list, LV_SYMBOL_SETTINGS);                                                 /* 添加列表文本 */
        btn = lv_list_add_btn(list, LV_SYMBOL_FILE, "Dev Notes");                             /* 添加按钮 */
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */

        lv_list_add_text(list, "Device Details");                                         /* 添加列表文本 */

        btn = lv_list_add_btn(list, 0, "Hardware Information");                  /* 添加按钮 */
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */

        btn = lv_list_add_btn(list, 0, "Firmware Information");                       /* 添加按钮 */
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */

        lv_list_add_text(list, " Protocol Info");                                                 /* 添加列表文本 */

        btn = lv_list_add_btn(list, LV_SYMBOL_USB, "USB");                             /* 添加按钮 */
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */

        btn = lv_list_add_btn(list, LV_SYMBOL_GPS, "GPS");                          /* 添加按钮 */
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */

        btn = lv_list_add_btn(list, 0, "UART");                              /* 添加按钮 */
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */

        btn = lv_list_add_btn(list, 0, "RS485");                 /* 添加按钮 */
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */

        btn = lv_list_add_btn(list, 0, "CAN");                 /* 添加按钮 */
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */

        btn = lv_list_add_btn(list, 0, "IOT");                 /* 添加按钮 */
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
//图片---------------------------------------------------------------------------//
        freertos = lv_img_create(setting_bg2);

         lvgl = lv_img_create(setting_bg2);

        lv_obj_move_foreground(main_return);            //把返回按钮设置到图层顶端
    }
    else if(target==main_1)
    {
        lv_obj_clear_flag(main_return,LV_OBJ_FLAG_HIDDEN);//显示返回上一级按钮
        lv_obj_clear_state(main_return,LV_STATE_DISABLED);

        main_btn_symbol[1]=1;



        uart_tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 25);        /* 创建选项卡 */
        lv_obj_t *tab1 = lv_tabview_add_tab(uart_tabview, "UART INIT");                                    /* 添加选项卡1 */
        lv_obj_t *tab2 = lv_tabview_add_tab(uart_tabview, "WINDOWS");                                   /* 添加选项卡2 */
        lv_obj_t *tab3 = lv_tabview_add_tab(uart_tabview, "MESSAGE");                                    /* 添加选项卡3 */


        lv_obj_set_scroll_dir(tab1,  LV_DIR_VER);                                         // 仅允许水平方向滚动
        lv_obj_set_scroll_dir(tab2,  LV_DIR_NONE);
        /*****************************************************/
        uart_bg=lv_obj_create (tab1);                                               /* 创建主界背景 */
        lv_obj_set_size(uart_bg, 480, 320);                                                 /* 设置主界背景大小 */
        lv_obj_align(uart_bg,  LV_ALIGN_CENTER,-5,0);
        lv_obj_set_style_bg_color(uart_bg,lv_color_hex(0XF0F0F0),LV_PART_MAIN);
        lv_obj_set_style_border_width(uart_bg,0,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(uart_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(uart_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(uart_bg,  LV_DIR_NONE);                                         // 仅允许水平方向滚动
        //UART1初始化按钮----------------------------------------------------------------------//
        uart_init_btn = lv_btn_create(uart_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(uart_init_btn, 100, 70);                                         /* 设置按钮大小 */
        lv_obj_align(uart_init_btn,  LV_ALIGN_TOP_RIGHT, -85, -5);
        lv_obj_add_event_cb(uart_init_btn, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */
        lv_obj_set_style_bg_color(uart_init_btn,lv_color_hex(0XFF0000),LV_PART_MAIN);

        lv_obj_t *main_set_label = lv_label_create(uart_init_btn);
        lv_label_set_text(main_set_label,"INIT" );
        lv_obj_set_style_text_font(main_set_label,&lv_font_montserrat_30, LV_STATE_DEFAULT);
        lv_obj_align(main_set_label, LV_ALIGN_CENTER, 0,0 );
        //uart初始化选项背景--------------------------------------------------------------------------------------//
        lv_obj_t *uart_setting_bg=lv_obj_create (uart_bg);                                               /* 创建主界背景 */
        lv_obj_set_size(uart_setting_bg, 260, 300);                                                 /* 设置主界背景大小 */
        lv_obj_align(uart_setting_bg, LV_ALIGN_TOP_LEFT, -5, -7);
        lv_obj_set_style_bg_color(uart_setting_bg,lv_color_hex(0XFFFFFF ),LV_PART_MAIN);
        lv_obj_set_style_border_width(uart_setting_bg,2,LV_PART_MAIN);
        lv_obj_set_style_radius(uart_setting_bg,20,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(uart_setting_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(uart_setting_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(uart_setting_bg, LV_DIR_ALL);
        //波特率滚轮----------------------------------------------------------------------------//
        baundrate_roller = lv_roller_create(uart_bg);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options(baundrate_roller, baudrate_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align(baundrate_roller, LV_ALIGN_TOP_LEFT, 110, 0);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width(baundrate_roller, 130);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font(baundrate_roller,&lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count(baundrate_roller, 1);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected(baundrate_roller, 1,LV_ANIM_ON);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color(baundrate_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius(baundrate_roller,10,LV_PART_SELECTED);

        lv_obj_t *baundrate_label = lv_label_create(uart_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(baundrate_label, "Baud rate:");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(baundrate_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align_to(baundrate_label, baundrate_roller, LV_ALIGN_OUT_LEFT_MID, -5, 0 );             /* 设置细分值位置 */
        //数据位滚轮-----------------------------------------------------------------------------//
        uart_date_roller = lv_roller_create(uart_bg);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options(uart_date_roller, uart_date_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align(uart_date_roller, LV_ALIGN_TOP_LEFT, 110, 60);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width(uart_date_roller, 130);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font(uart_date_roller,&lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count(uart_date_roller, 1);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected(uart_date_roller, 1,LV_ANIM_ON);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color(uart_date_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius(uart_date_roller,10,LV_PART_SELECTED);

        lv_obj_t *usart_date_label = lv_label_create(uart_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(usart_date_label, "Data Bits:");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(usart_date_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align_to(usart_date_label, uart_date_roller, LV_ALIGN_OUT_LEFT_MID, -5, 0 );             /* 设置细分值位置 */
        //停止位滚轮------------------------------------------------------------------------------------//
        uart_stop_roller = lv_roller_create(uart_bg);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options(uart_stop_roller, uart_stop_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align(uart_stop_roller, LV_ALIGN_TOP_LEFT, 110, 120);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width(uart_stop_roller, 130);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font(uart_stop_roller,&lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count(uart_stop_roller, 1);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected(uart_stop_roller, 1,LV_ANIM_ON);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color(uart_stop_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius(uart_stop_roller,10,LV_PART_SELECTED);

        lv_obj_t *uart_stop_label = lv_label_create(uart_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(uart_stop_label, "Stop Bits:");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(uart_stop_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align_to(uart_stop_label,uart_stop_roller, LV_ALIGN_OUT_LEFT_MID, -5, 0 );             /* 设置细分值位置 */
        //奇偶校验位滚轮----------------------------------------------------------------------------------//
        uart_parity_roller = lv_roller_create(uart_bg);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options(uart_parity_roller, uart_parity_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align(uart_parity_roller, LV_ALIGN_TOP_LEFT, 110, 180);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width(uart_parity_roller, 130);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font(uart_parity_roller,&lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count(uart_parity_roller, 1);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected(uart_parity_roller, 1,LV_ANIM_ON);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color(uart_parity_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius(uart_parity_roller,10,LV_PART_SELECTED);

        lv_obj_t *uart_partiy_label = lv_label_create(uart_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(uart_partiy_label, "Parity Bit:");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(uart_partiy_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align_to(uart_partiy_label, uart_parity_roller, LV_ALIGN_OUT_LEFT_MID, -5, 0 );             /* 设置细分值位置 */
        //模式滚轮----------------------------------------------------------------------------------//
        uart_mode_roller = lv_roller_create(uart_bg);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options(uart_mode_roller, uart_mode_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align(uart_mode_roller, LV_ALIGN_TOP_LEFT, 110, 240);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width(uart_mode_roller, 130);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font(uart_mode_roller,&lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count(uart_mode_roller, 1);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected(uart_mode_roller, 1,LV_ANIM_ON);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color(uart_mode_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius(uart_mode_roller,10,LV_PART_SELECTED);

        lv_obj_t *uart_mode_label = lv_label_create(uart_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(uart_mode_label, "Mode:");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(uart_mode_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align_to(uart_mode_label, uart_mode_roller, LV_ALIGN_OUT_LEFT_MID, -5, 0 );             /* 设置细分值位置 */

        //UART中断配置栏------------------------------------------------------------------------//
        lv_obj_t *uart_it_bg=lv_obj_create (uart_bg);                                               /* 创建主界背景 */
        lv_obj_set_size(uart_it_bg, 190, 120);                                                 /* 设置主界背景大小 */
        lv_obj_align(uart_it_bg, LV_ALIGN_TOP_RIGHT,5,173);
        lv_obj_set_style_bg_color(uart_it_bg,lv_color_hex(0XFFFFFF ),LV_PART_MAIN);
        lv_obj_set_style_border_width(uart_it_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(uart_it_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(uart_it_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(uart_it_bg, LV_DIR_ALL);
        lv_obj_set_style_radius(uart_it_bg,20,LV_PART_MAIN);
        //uart使能背景-------------------------------------------------------------------------//
        lv_obj_t *uart_enable_bg=lv_obj_create (uart_bg);                                               /* 创建主界背景 */
        lv_obj_set_size(uart_enable_bg, 190, 90);                                                 /* 设置主界背景大小 */
        lv_obj_align(uart_enable_bg, LV_ALIGN_TOP_RIGHT,5,73);
        lv_obj_set_style_bg_color(uart_enable_bg,lv_color_hex(0XFFFFFF ),LV_PART_MAIN);
        lv_obj_set_style_border_width(uart_enable_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(uart_enable_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(uart_enable_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(uart_enable_bg, LV_DIR_NONE);
        lv_obj_set_style_radius(uart_enable_bg,20,LV_PART_MAIN);

        uart_it_enable_switch=lv_switch_create(uart_enable_bg);                                            /* 创建步进电机方向开关 */
        lv_obj_set_size(uart_it_enable_switch,60, 30 );
        lv_obj_align(uart_it_enable_switch, LV_ALIGN_TOP_RIGHT,-5, -7 );             /* 设置位置 */
        lv_obj_set_style_bg_color(uart_it_enable_switch,lv_color_hex(0X1f4287),LV_PART_INDICATOR|LV_STATE_CHECKED);
        //lv_obj_add_event_cb(moto_dir, steep_btn, LV_EVENT_CLICKED, NULL);        /* 设置行程按钮事件 */

        lv_obj_t *uart_it_enable_label = lv_label_create(uart_enable_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(uart_it_enable_label, "IE");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(uart_it_enable_label, &lv_font_montserrat_28, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align_to(uart_it_enable_label, uart_it_enable_switch, LV_ALIGN_LEFT_MID, -70, 0 );             /* 设置细分值位置 */

        uart_enable_switch=lv_switch_create(uart_enable_bg);                                            /* 创建步进电机方向开关 */
        lv_obj_set_size(uart_enable_switch,60, 30 );
        lv_obj_align(uart_enable_switch, LV_ALIGN_TOP_RIGHT,-5,35 );             /* 设置位置 */
        lv_obj_set_style_bg_color(uart_enable_switch,lv_color_hex(0X1f4287),LV_PART_INDICATOR|LV_STATE_CHECKED);
        //lv_obj_add_event_cb(moto_dir, steep_btn, LV_EVENT_CLICKED, NULL);        /* 设置行程按钮事件 */

        lv_obj_t *uart_enable_label = lv_label_create(uart_enable_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(uart_enable_label, "UE");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(uart_enable_label, &lv_font_montserrat_28, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align_to(uart_enable_label,  uart_enable_switch, LV_ALIGN_LEFT_MID, -70, 0 );             /* 设置细分值位置 */

        //中断标志位----------------------------------------------------------------------------------//
        uart_it_flag_roller = lv_roller_create(uart_bg);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options(uart_it_flag_roller, uart_it_flag_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align(uart_it_flag_roller, LV_ALIGN_TOP_RIGHT, -5, 180);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width(uart_it_flag_roller, 90);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font(uart_it_flag_roller,&lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count(uart_it_flag_roller, 1);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected(uart_it_flag_roller, 1,LV_ANIM_ON);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color(uart_it_flag_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius(uart_it_flag_roller,10,LV_PART_SELECTED);

        lv_obj_t *uart_it_flag_label = lv_label_create(uart_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(uart_it_flag_label, "It Flag:");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(uart_it_flag_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align_to(uart_it_flag_label, uart_it_flag_roller, LV_ALIGN_OUT_LEFT_MID, -5, 0 );             /* 设置细分值位置 */
        //中断优先级滚轮----------------------------------------------------------------------------------//
        uart_it_pr_roller = lv_roller_create(uart_bg);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options( uart_it_pr_roller, uart_it_pr_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align( uart_it_pr_roller, LV_ALIGN_TOP_RIGHT, -5,240);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width( uart_it_pr_roller, 90);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font( uart_it_pr_roller,&lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count( uart_it_pr_roller, 1);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected( uart_it_pr_roller, 1,LV_ANIM_ON);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color( uart_it_pr_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius( uart_it_pr_roller,10,LV_PART_SELECTED);

        lv_obj_t *uart_it_pr_label = lv_label_create(uart_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(uart_it_pr_label, "Priority:");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(uart_it_pr_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align_to(uart_it_pr_label, uart_it_pr_roller, LV_ALIGN_OUT_LEFT_MID, -5, 0 );             /* 设置细分值位置 */

        lv_obj_move_foreground(main_return);            //把返回按钮设置到图层顶端

        //串口发送界面//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        lv_obj_t *uart_send_bg=lv_obj_create (tab2);                                               /* 创建主界背景 */
        lv_obj_set_size(uart_send_bg, 480, 300);                                                 /* 设置主界背景大小 */
        lv_obj_align(uart_send_bg, LV_ALIGN_CENTER,0,0);
        lv_obj_set_style_bg_color(uart_send_bg,lv_color_hex(0XFFFFFF),LV_PART_MAIN);
        lv_obj_set_style_border_width(uart_send_bg,0,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(uart_send_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(uart_send_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(uart_send_bg,  LV_DIR_NONE);

        //消息接收窗口背景///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        lv_obj_t *uart_get_bg=lv_obj_create (tab2);                                               /* 创建主界背景 */
        lv_obj_set_size(uart_get_bg, 305, 90);                                                 /* 设置主界背景大小 */
        lv_obj_align(uart_get_bg, LV_ALIGN_TOP_LEFT,0,-10);
        lv_obj_set_style_bg_color(uart_get_bg,lv_color_hex(0XF0F0F0),LV_PART_MAIN);
        lv_obj_set_style_border_width(uart_get_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(uart_get_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(uart_get_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(uart_get_bg,  LV_DIR_VER );
        lv_obj_set_style_radius(uart_get_bg,0,LV_PART_MAIN);
        //消息接收窗口文本框////////////////////////////////////////////////////////////////////////////////////////////
        uart_get_label = lv_textarea_create(uart_get_bg);
        lv_obj_set_size(uart_get_label, 305, 800);
        lv_obj_set_style_text_font(uart_get_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(uart_get_label, LV_ALIGN_TOP_LEFT, 0,0 );
        lv_obj_set_style_text_color(uart_get_label,lv_color_hex(0X000000),LV_STATE_DEFAULT);
        lv_obj_set_style_radius(uart_get_label,0,LV_PART_MAIN);

        lv_obj_set_style_pad_left(uart_get_bg, 0, 0);
        lv_obj_set_style_pad_right(uart_get_bg, 0, 0);
        lv_obj_set_style_pad_top(uart_get_bg, 0, 0);
        lv_obj_set_style_pad_bottom(uart_get_bg, 0, 0);
        //消息发送窗口///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        lv_obj_t *uart_sendsend_bg=lv_obj_create (tab2);                                               /* 创建主界背景 */
        lv_obj_set_size(uart_sendsend_bg, 305, 60);                                                 /* 设置主界背景大小 */
        lv_obj_align_to(uart_sendsend_bg, uart_get_bg,LV_ALIGN_OUT_BOTTOM_MID,0,5);
        lv_obj_set_style_bg_color(uart_sendsend_bg,lv_color_hex(0XF0F0F0),LV_PART_MAIN);
        lv_obj_set_style_border_width(uart_sendsend_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(uart_sendsend_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(uart_sendsend_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(uart_sendsend_bg,  LV_DIR_VER);
        lv_obj_set_style_radius(uart_sendsend_bg,0,LV_PART_MAIN);
        //消息发送窗口文本框
        uart_send2_label =lv_textarea_create(uart_sendsend_bg);
        lv_obj_set_size(uart_send2_label, 305, 300);
        lv_obj_align(uart_send2_label, LV_ALIGN_TOP_LEFT, -3,0 );
        lv_obj_set_style_text_color(uart_send2_label,lv_color_hex(0X000000),LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(uart_send2_label,&lv_font_montserrat_20, LV_PART_MAIN); /* 设置字体 */
        lv_obj_set_style_radius(uart_send2_label,0,LV_PART_MAIN);

        lv_obj_set_style_pad_left(uart_sendsend_bg, 0, 0);
        lv_obj_set_style_pad_right(uart_sendsend_bg, 0, 0);
        lv_obj_set_style_pad_top(uart_sendsend_bg, 0, 0);
        lv_obj_set_style_pad_bottom(uart_sendsend_bg, 0, 0);
        //消息接收区域清除按钮/////////////////////////////////////////////////////////////////////////////////
        uart_get_clean_btn = lv_btn_create(tab2);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(uart_get_clean_btn, 70, 25);                                         /* 设置按钮大小 */
        lv_obj_set_style_bg_color(uart_get_clean_btn,lv_color_hex(0XFF0000),LV_PART_MAIN);
        lv_obj_align_to(uart_get_clean_btn,uart_get_bg ,LV_ALIGN_OUT_RIGHT_MID,5, 0);
        lv_obj_add_event_cb(uart_get_clean_btn,chat_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */
        lv_obj_add_event_cb(uart_get_clean_btn, uart_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置行程按钮事件 */

        lv_obj_t *uart_get_clean_label = lv_label_create(uart_get_clean_btn);
        lv_label_set_text(uart_get_clean_label,"Clean" );
        lv_obj_set_style_text_font(uart_get_clean_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(uart_get_clean_label, LV_ALIGN_CENTER, 0,0 );
        //消息接收区域标题/////////////////////////////////////////////////////////////////////////////////
        lv_obj_t *uart_get_note_bg=lv_obj_create (tab2);                                               /* 创建主界背景 */
        lv_obj_set_size(uart_get_note_bg, 40, 25);                                                 /* 设置主界背景大小 */
        lv_obj_align_to(uart_get_note_bg, uart_get_bg,LV_ALIGN_OUT_RIGHT_MID,0,-30);
        lv_obj_set_style_bg_color(uart_get_note_bg,lv_color_hex(0X1f4287),LV_PART_MAIN);
        lv_obj_set_style_border_width(uart_get_note_bg,0,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(uart_get_note_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(uart_get_note_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(uart_get_note_bg,  LV_DIR_NONE);
        lv_obj_set_style_radius(uart_get_note_bg,0,LV_PART_MAIN);

        lv_obj_t *uart_get_clean_note_label = lv_label_create(uart_get_note_bg);
        lv_label_set_text(uart_get_clean_note_label,"RX" );
        lv_obj_set_style_text_font(uart_get_clean_note_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(uart_get_clean_note_label, LV_ALIGN_CENTER, 0,0 );
        lv_obj_set_style_text_color(uart_get_clean_note_label,lv_color_hex(0XFFFFFF),LV_STATE_DEFAULT);

        //串口开关/////////////////////////////////////////////////////////////////////////////////
        uart_off_switch=lv_btn_create(tab2);                                           /* 创建步进电机方向开关 */
        lv_obj_set_size(uart_off_switch,70, 40 );
        lv_obj_align(uart_off_switch, LV_ALIGN_BOTTOM_RIGHT,10, -70);             /* 设置位置 */
        lv_obj_set_style_bg_color(uart_off_switch,lv_color_hex(0x0000ff),LV_PART_INDICATOR|LV_STATE_CHECKED);
        lv_obj_add_event_cb(uart_off_switch, uart_btn_event, LV_EVENT_CLICKED, NULL);

        lv_obj_t *uart_off_label = lv_label_create(uart_off_switch);
        lv_label_set_text(uart_off_label,"OFF" );
        lv_obj_set_style_text_color(uart_off_label,lv_color_hex(0x0000ff),LV_PART_MAIN);
        lv_obj_set_style_text_font(uart_off_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        //lv_obj_align_to(uart_off_label,uart_off_switch, LV_ALIGN_OUT_LEFT_MID, -5,0 );
        lv_obj_align(uart_off_label,LV_ALIGN_CENTER, 0,0 );

        //消息发送区域清除按钮/////////////////////////////////////////////////////////////////////////////////
        uart_send_clean_btn = lv_btn_create(tab2);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(uart_send_clean_btn, 70, 25);                                         /* 设置按钮大小 */
        lv_obj_set_style_bg_color(uart_send_clean_btn,lv_color_hex(0XFF0000),LV_PART_MAIN);
        lv_obj_align_to(uart_send_clean_btn,uart_sendsend_bg ,LV_ALIGN_OUT_RIGHT_MID,5, 15);
        lv_obj_add_event_cb(uart_send_clean_btn, uart_btn_event, LV_EVENT_CLICKED, NULL);

        lv_obj_t *uart_send_clean_label = lv_label_create(uart_send_clean_btn);
        lv_label_set_text(uart_send_clean_label,"Clean" );
        lv_obj_set_style_text_font(uart_send_clean_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(uart_send_clean_label, LV_ALIGN_CENTER, 0,0 );
         //消息发送区域标题/////////////////////////////////////////////////////////////////////////////////
        lv_obj_t *uart_send_note_bg=lv_obj_create (tab2);                                               /* 创建主界背景 */
        lv_obj_set_size(uart_send_note_bg, 40, 25);                                                 /* 设置主界背景大小 */
        lv_obj_align_to(uart_send_note_bg, uart_sendsend_bg,LV_ALIGN_OUT_RIGHT_MID,0,-15);
        lv_obj_set_style_bg_color(uart_send_note_bg,lv_color_hex(0X1f4287),LV_PART_MAIN);
        lv_obj_set_style_border_width(uart_send_note_bg,0,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(uart_send_note_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(uart_send_note_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(uart_send_note_bg,  LV_DIR_NONE);
        lv_obj_set_style_radius(uart_send_note_bg,0,LV_PART_MAIN);

        lv_obj_t *uart_send_clean_note_label = lv_label_create(uart_send_note_bg);
        lv_label_set_text(uart_send_clean_note_label,"TX" );
        lv_obj_set_style_text_font(uart_send_clean_note_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(uart_send_clean_note_label, LV_ALIGN_CENTER, 0,0 );
        lv_obj_set_style_text_color(uart_send_clean_note_label,lv_color_hex(0XFFFFFF),LV_STATE_DEFAULT);
        //消息发送按钮//////////////////////////////////////////////////////////////////////////////////////////////
        uart_send_btn = lv_btn_create(tab2);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(uart_send_btn, 70, 25);                                         /* 设置按钮大小 */
        lv_obj_set_style_bg_color(uart_send_btn,lv_color_hex(0X66FF66),LV_PART_MAIN);
        lv_obj_align_to(uart_send_btn,uart_send_clean_btn ,LV_ALIGN_OUT_RIGHT_MID,5,0);
        lv_obj_add_event_cb(uart_send_btn, uart_btn_event, LV_EVENT_CLICKED, NULL);

        lv_obj_t *uart_send_label = lv_label_create(uart_send_btn);
        lv_label_set_text(uart_send_label,"Send" );
        lv_obj_set_style_text_font(uart_send_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(uart_send_label, LV_ALIGN_CENTER, 0,0 );
        //uart 按钮矩阵////////////////////////////////////////////////////////////////////////////////////////////////////////
        uart_btnm = lv_btnmatrix_create(tab2);
        lv_btnmatrix_set_map(uart_btnm, uart_btnm_map);
        lv_obj_set_size(uart_btnm, 380, 145);
        lv_obj_set_style_radius(uart_btnm,10,LV_PART_MAIN);
        lv_obj_align(uart_btnm, LV_ALIGN_BOTTOM_LEFT,0,30);
        lv_btnmatrix_set_btn_ctrl(uart_btnm, 0, LV_BTNMATRIX_CTRL_RECOLOR);
        lv_obj_set_style_bg_color(uart_btnm,lv_color_hex(0X1f4287),LV_PART_ITEMS);
        lv_obj_set_style_text_color(uart_btnm,lv_color_hex(0XFFFFFF),LV_PART_ITEMS);
        lv_obj_add_event_cb(uart_btnm, uart_btn_event, LV_EVENT_VALUE_CHANGED, NULL);

        //高级选项-----------------------------------------------------------------------------//
        lv_obj_t *uart_rx_hex_checkbox = lv_checkbox_create(tab2);
        lv_checkbox_set_text(uart_rx_hex_checkbox, "RX Hex");
        lv_obj_set_style_text_font(uart_rx_hex_checkbox,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align_to(uart_rx_hex_checkbox,uart_get_note_bg, LV_ALIGN_OUT_RIGHT_MID, 5,0 );

        lv_obj_t *uart_tx_hex_checkbox = lv_checkbox_create(tab2);
        lv_checkbox_set_text(uart_tx_hex_checkbox, "RX Hex");
        lv_obj_set_style_text_font(uart_tx_hex_checkbox,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align_to(uart_tx_hex_checkbox,uart_send_note_bg, LV_ALIGN_OUT_RIGHT_MID, 5,0 );

    }
    else if(target==main_2)
    {
		vTaskResume(ADC3Task_Handler);//----唤醒内部温度测量任务
		
        lv_obj_clear_flag(main_return,LV_OBJ_FLAG_HIDDEN);//显示返回上一级按钮
        lv_obj_clear_state(main_return,LV_STATE_DISABLED);

        main_btn_symbol[2]=1;

        /*****************************************************/
        //图表界面背景
        chat_bg=lv_obj_create (lv_scr_act());                                               /* 创建主界背景 */
        lv_obj_set_size(chat_bg, 480, 1026);                                                 /* 设置主界背景大小 */
        lv_obj_set_style_radius(chat_bg,0,LV_PART_MAIN);
        lv_obj_set_style_bg_color(chat_bg,lv_color_hex(0XF5F5F5),LV_PART_MAIN);
        lv_obj_set_style_border_width(chat_bg,0,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(chat_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(chat_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(chat_bg,LV_DIR_ALL);                                         // 仅允许水平方向滚动

        lv_obj_t* chat_symbol_bg=lv_obj_create (chat_bg);                                               /* 创建主界背景 */
        lv_obj_set_size(chat_symbol_bg, 300, 60);                                                 /* 设置主界背景大小 */
        lv_obj_align(chat_symbol_bg,LV_ALIGN_TOP_MID,0,0);
        lv_obj_set_style_radius(chat_symbol_bg,10,LV_PART_MAIN);
        lv_obj_set_style_bg_color(chat_symbol_bg,lv_color_hex(0X1f4287),LV_PART_MAIN);
        lv_obj_set_style_border_width(chat_symbol_bg,1,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(chat_symbol_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(chat_symbol_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(chat_symbol_bg,  LV_DIR_NONE);

        lv_obj_t*chat_symbol_label = lv_label_create( chat_symbol_bg);
        lv_label_set_text(chat_symbol_label,"Table" );
        lv_obj_set_style_text_font(chat_symbol_label,&lv_font_montserrat_48, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(chat_symbol_label,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
        lv_obj_align(chat_symbol_label, LV_ALIGN_CENTER, 0,0 );
        //折线图背景--------------------------------------------------------------------//
        lv_obj_t* chat1_bg=lv_obj_create (chat_bg);                                               /* 创建主界背景 */
        lv_obj_set_size(chat1_bg, 440, 360);                                                 /* 设置主界背景大小 */
        lv_obj_align(chat1_bg,LV_ALIGN_TOP_LEFT,0,150);
        lv_obj_set_style_radius(chat1_bg,10,LV_PART_MAIN);
        lv_obj_set_style_bg_color(chat1_bg,lv_color_hex(0XFFFFFF),LV_PART_MAIN);
        lv_obj_set_style_border_width(chat1_bg,3,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(chat1_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(chat1_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(chat1_bg,  LV_DIR_NONE);

        lv_obj_t*chat1_temp_label = lv_label_create( chat1_bg);
        lv_label_set_text(chat1_temp_label,"Temp/('C)" );
        lv_obj_set_style_text_font(chat1_temp_label,&lv_font_montserrat_16, LV_STATE_DEFAULT);
        lv_obj_align(chat1_temp_label, LV_ALIGN_TOP_LEFT, -5,-5 );
        //------------------------------------------------------------------------------//
        lv_obj_t* chat_temp_symbol_bg=lv_obj_create (chat_bg);                                               /* 创建主界背景 */
        lv_obj_set_size(chat_temp_symbol_bg, 200, 50);                                                 /* 设置主界背景大小 */
        lv_obj_align_to(chat_temp_symbol_bg,chat1_bg,LV_ALIGN_OUT_TOP_LEFT,0,3);
        lv_obj_set_style_radius(chat_temp_symbol_bg,10,LV_PART_MAIN);
        lv_obj_set_style_bg_color(chat_temp_symbol_bg,lv_color_hex(0X1f4287),LV_PART_MAIN);
        lv_obj_set_style_border_width(chat_temp_symbol_bg,3,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(chat_temp_symbol_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(chat_temp_symbol_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(chat_temp_symbol_bg,  LV_DIR_NONE);

        lv_obj_t*chat1_temp_symbol_label = lv_label_create( chat_temp_symbol_bg);
        lv_label_set_text(chat1_temp_symbol_label,"Line Chart" );
        lv_obj_set_style_text_font(chat1_temp_symbol_label,&lv_font_montserrat_30, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(chat1_temp_symbol_label,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
        lv_obj_align(chat1_temp_symbol_label, LV_ALIGN_CENTER, 0,0 );
        //--------------------------------------------------------------------------
        lv_obj_t*chat1_temp_Y_label = lv_label_create( chat1_bg);
        lv_label_set_text(chat1_temp_Y_label,"Time(S)" );
        lv_obj_set_style_text_font(chat1_temp_Y_label,&lv_font_montserrat_16, LV_STATE_DEFAULT);
        lv_obj_align(chat1_temp_Y_label, LV_ALIGN_BOTTOM_RIGHT, 5,-20 );
        //图表缩放滑块--------------------------------------------------------------------//
        char_temp_x_slider = lv_slider_create(chat1_bg);                             /* 创建滑块 */
        lv_obj_set_size(char_temp_x_slider, 120, 10);                               /* 设置大小 */
        lv_obj_align(char_temp_x_slider, LV_ALIGN_BOTTOM_RIGHT, -90,-25 );                                                         /* 设置位置 */
        lv_slider_set_value(char_temp_x_slider, 256, LV_ANIM_OFF);                                   /* 设置当前值 */
        lv_slider_set_range(char_temp_x_slider,256,1024);/* 设置 char_temp_x_slider 部件范围值 */

        lv_obj_add_event_cb(char_temp_x_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 添加事件 */

        lv_obj_t *char_temp_x_label = lv_label_create(chat1_bg);                          /* 创建音量标签 */
        lv_label_set_text(char_temp_x_label,"X");                           /* 设置文本内容：音量图标 */
        lv_obj_set_style_text_font(char_temp_x_label,&lv_font_montserrat_16, LV_STATE_DEFAULT);                /* 设置字体 */
        lv_obj_align_to(char_temp_x_label, char_temp_x_slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);            /* 设置位置 */

        char_temp_Y_slider = lv_slider_create(chat1_bg);                             /* 创建滑块 */
        lv_obj_set_size(char_temp_Y_slider, 120, 10);                               /* 设置大小 */
        lv_obj_align(char_temp_Y_slider, LV_ALIGN_BOTTOM_RIGHT, -90,5 );                                                         /* 设置位置 */
        lv_slider_set_value(char_temp_Y_slider, 256, LV_ANIM_OFF);                                   /* 设置当前值 */
        lv_slider_set_range(char_temp_Y_slider,256,1024);/* 设置 char_temp_x_slider 部件范围值 */

        lv_obj_add_event_cb(char_temp_Y_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 添加事件 */

        lv_obj_t *char_temp_Y_label = lv_label_create(chat1_bg);                          /* 创建音量标签 */
        lv_label_set_text(char_temp_Y_label,"Y");                           /* 设置文本内容：音量图标 */
        lv_obj_set_style_text_font(char_temp_Y_label,&lv_font_montserrat_16, LV_STATE_DEFAULT);                /* 设置字体 */
        lv_obj_align_to(char_temp_Y_label, char_temp_Y_slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);            /* 设置位置 */

        //图表控制按钮--------------------------------------------------------------------//
        chat_temp_enable_switch=lv_switch_create(chat1_bg);                                            /* 创建步进电机方向开关 */
        lv_obj_set_size(chat_temp_enable_switch,60, 30 );
        lv_obj_align(chat_temp_enable_switch, LV_ALIGN_BOTTOM_MID,-140, -5);             /* 设置位置 */
        lv_obj_set_style_bg_color(chat_temp_enable_switch,lv_color_hex(0X1f4287),LV_PART_INDICATOR|LV_STATE_CHECKED);
        lv_obj_add_event_cb(chat_temp_enable_switch, chat_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        lv_obj_t *chat_temp_enable_label = lv_label_create(chat1_bg);
        lv_label_set_text(chat_temp_enable_label,"ON" );
        lv_obj_set_style_text_font(chat_temp_enable_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align_to(chat_temp_enable_label,chat_temp_enable_switch, LV_ALIGN_OUT_LEFT_MID, -5,0 );
        //折线增加开关（3个）------------------------------------------------------------------------//
        chat_temp_switch1=lv_switch_create(chat1_bg);                                            /* 创建步进电机方向开关 */
        lv_obj_set_size(chat_temp_switch1,40, 20 );
        lv_obj_align(chat_temp_switch1, LV_ALIGN_TOP_RIGHT,0, 0);             /* 设置位置 */
        lv_obj_set_style_bg_color(chat_temp_switch1,lv_color_hex(0x006400),LV_PART_INDICATOR|LV_STATE_CHECKED);
        lv_obj_add_event_cb(chat_temp_switch1, chat_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */


        lv_obj_t *chat_temp1_label = lv_label_create(chat1_bg);
        lv_label_set_text(chat_temp1_label,"Data 1" );
        lv_obj_set_style_text_color(chat_temp1_label,lv_color_hex(0x006400),LV_PART_MAIN);
        lv_obj_set_style_text_font(chat_temp1_label,&lv_font_montserrat_14, LV_STATE_DEFAULT);
        lv_obj_align_to(chat_temp1_label,chat_temp_switch1, LV_ALIGN_OUT_LEFT_MID, -5,0 );

        chat_temp_switch2=lv_switch_create(chat1_bg);                                            /* 创建步进电机方向开关 */
        lv_obj_set_size(chat_temp_switch2,40, 20 );
        lv_obj_align(chat_temp_switch2, LV_ALIGN_TOP_RIGHT,-100, 0);             /* 设置位置 */
        lv_obj_set_style_bg_color(chat_temp_switch2,lv_color_hex(0xFF0000),LV_PART_INDICATOR|LV_STATE_CHECKED);
        lv_obj_add_event_cb(chat_temp_switch2, chat_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */


        lv_obj_t *chat_temp2_label = lv_label_create(chat1_bg);
        lv_label_set_text(chat_temp2_label,"Data 2" );
        lv_obj_set_style_text_color(chat_temp2_label,lv_color_hex(0xFF0000),LV_PART_MAIN);
        lv_obj_set_style_text_font(chat_temp2_label,&lv_font_montserrat_14, LV_STATE_DEFAULT);
        lv_obj_align_to(chat_temp2_label,chat_temp_switch2, LV_ALIGN_OUT_LEFT_MID, -5,0 );

        chat_temp_switch3=lv_switch_create(chat1_bg);                                            /* 创建步进电机方向开关 */
        lv_obj_set_size(chat_temp_switch3,40, 20 );
        lv_obj_align(chat_temp_switch3, LV_ALIGN_TOP_RIGHT,-200, 0);             /* 设置位置 */
        lv_obj_set_style_bg_color(chat_temp_switch3,lv_color_hex(0x0000FF),LV_PART_INDICATOR|LV_STATE_CHECKED);
        lv_obj_add_event_cb(chat_temp_switch3, chat_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */


        lv_obj_t *chat_temp3_label = lv_label_create(chat1_bg);
        lv_label_set_text(chat_temp3_label,"Data 3" );
        lv_obj_set_style_text_color(chat_temp3_label,lv_color_hex(0x0000FF),LV_PART_MAIN);
        lv_obj_set_style_text_font(chat_temp3_label,&lv_font_montserrat_14, LV_STATE_DEFAULT);
        lv_obj_align_to(chat_temp3_label,chat_temp_switch3, LV_ALIGN_OUT_LEFT_MID, -5,0 );

        //图表清除重置按钮----------------------------------------------------------------//
        chat_temp_clean_btn = lv_btn_create(chat1_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(chat_temp_clean_btn, 70, 40);                                         /* 设置按钮大小 */
        lv_obj_set_style_bg_color(chat_temp_clean_btn,lv_color_hex(0XFF0000),LV_PART_MAIN);
        lv_obj_align(chat_temp_clean_btn,  LV_ALIGN_BOTTOM_MID, -60, 0);
        lv_obj_add_event_cb(chat_temp_clean_btn,chat_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        lv_obj_t *chat_temp_clean_label = lv_label_create(chat_temp_clean_btn);
        lv_label_set_text(chat_temp_clean_label,"Clean" );
        lv_obj_set_style_text_font(chat_temp_clean_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(chat_temp_clean_label, LV_ALIGN_CENTER, 0,0 );
        //温度图表----------------------------------------------------------------------------------
        chart = lv_chart_create(chat1_bg);/* 创建图表部件 */
        lv_obj_set_size(chart,380, 220);/* 设置图表大小 */
        lv_obj_align(chart,LV_ALIGN_TOP_LEFT,25,30);
        lv_chart_set_type(chart, LV_CHART_TYPE_LINE);/* 设置折线图类型 */
        ser1 = lv_chart_add_series(chart,lv_palette_main(LV_PALETTE_RED),LV_CHART_AXIS_SECONDARY_X);/* 添加数据序列 */
        ser0 = lv_chart_add_series(chart,lv_palette_main(LV_PALETTE_GREEN),LV_CHART_AXIS_SECONDARY_X);/* 添加数据序列 */
        ser_1 = lv_chart_add_series(chart,lv_palette_main(LV_PALETTE_BLUE),LV_CHART_AXIS_SECONDARY_X);/* 添加数据序列 */

        lv_chart_set_point_count(chart,20);//设置数据序列为20个点

        //lv_obj_t * cursor =  lv_chart_add_cursor(chart, lv_color_hex(0X1f4287), LV_DIR_HOR);           //设置游标// // 3. 创建游标// 默认游标样式
        //lv_chart_set_cursor_point(chart, ser1, cursor, 2); // 设置游标位置在 series 的第 5 个数据点

        lv_chart_set_div_line_count(chart,51,96);//设置横向和竖向分隔线

        lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 11, 5,true, 40);/* 设置 Y 轴的刻度和刻度标签，主刻度长度为 10，小刻度为 5，主刻度数量为 6，小刻度数量为 5 */
        lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 20, 5,true, 30);/* 设置 X 轴的刻度和刻度标签，主刻度长度为 10，小刻度为 5，主刻度数量为 10，小刻度数量为 2 */

        lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);/* 将旧数据移到左边 */

        lv_chart_set_zoom_x(chart,400);

        //chat_temp_timer=lv_timer_create(add_data, 200, NULL);//温度图表定时器创建

        lv_chart_refresh(chart);/* 更新图表 */

        lv_obj_move_foreground(main_return);            //把返回按钮设置到图层顶端

        //图表二---------------------------------------------------------------------------------------------//
        lv_obj_t* chat2_bg=lv_obj_create (chat_bg);                                               /* 创建主界背景 */
        lv_obj_set_size( chat2_bg, 440, 360);                                                 /* 设置主界背景大小 */
        lv_obj_align( chat2_bg,LV_ALIGN_TOP_LEFT,0,600);
        lv_obj_set_style_radius( chat2_bg,10,LV_PART_MAIN);
        lv_obj_set_style_bg_color( chat2_bg,lv_color_hex(0XFFFFFF),LV_PART_MAIN);
        lv_obj_set_style_border_width( chat2_bg,3,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode( chat2_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag( chat2_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir( chat2_bg,  LV_DIR_NONE);
        //柱状图标题-----------------------------------------------------------------------------------------//
        lv_obj_t* chat_bar2_symbol_bg=lv_obj_create (chat_bg);                                               /* 创建主界背景 */
        lv_obj_set_size(chat_bar2_symbol_bg, 200, 50);                                                 /* 设置主界背景大小 */
        lv_obj_align_to(chat_bar2_symbol_bg,chat2_bg,LV_ALIGN_OUT_TOP_LEFT,0,3);
        lv_obj_set_style_radius(chat_bar2_symbol_bg,10,LV_PART_MAIN);
        lv_obj_set_style_bg_color(chat_bar2_symbol_bg,lv_color_hex(0X1f4287),LV_PART_MAIN);
        lv_obj_set_style_border_width(chat_bar2_symbol_bg,3,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(chat_bar2_symbol_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(chat_bar2_symbol_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(chat_bar2_symbol_bg,  LV_DIR_NONE);

        lv_obj_t*chat1_bar2_symbol_label = lv_label_create( chat_bar2_symbol_bg);
        lv_label_set_text(chat1_bar2_symbol_label,"Bar Chart " );
        lv_obj_set_style_text_font(chat1_bar2_symbol_label,&lv_font_montserrat_30, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(chat1_bar2_symbol_label,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
        lv_obj_align(chat1_bar2_symbol_label, LV_ALIGN_CENTER, 0,0 );

        //图表控制按钮--------------------------------------------------------------------//
        chat_bar_enable_switch=lv_switch_create(chat2_bg);                                            /* 创建步进电机方向开关 */
        lv_obj_set_size(chat_bar_enable_switch,60, 30 );
        lv_obj_align(chat_bar_enable_switch, LV_ALIGN_BOTTOM_MID,-140, -5);             /* 设置位置 */
        lv_obj_set_style_bg_color(chat_bar_enable_switch,lv_color_hex(0X1f4287),LV_PART_INDICATOR|LV_STATE_CHECKED);
        lv_obj_add_event_cb(chat_bar_enable_switch, chat_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        lv_obj_t *chat_bar_enable_label = lv_label_create(chat2_bg);
        lv_label_set_text(chat_bar_enable_label,"ON" );
        lv_obj_set_style_text_font(chat_bar_enable_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align_to(chat_bar_enable_label,chat_bar_enable_switch, LV_ALIGN_OUT_LEFT_MID, -5,0 );
        //折线增加开关（2个）------------------------------------------------------------------------//
        chat_bar_switch1=lv_switch_create(chat2_bg);                                            /* 创建步进电机方向开关 */
        lv_obj_set_size(chat_bar_switch1,40, 20 );
        lv_obj_align(chat_bar_switch1, LV_ALIGN_TOP_RIGHT,0, 0);             /* 设置位置 */
        lv_obj_set_style_bg_color(chat_bar_switch1,lv_color_hex(0x0000ff),LV_PART_INDICATOR|LV_STATE_CHECKED);
        lv_obj_add_event_cb(chat_bar_switch1, chat_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        lv_obj_t *chat_bar_label = lv_label_create(chat2_bg);
        lv_label_set_text(chat_bar_label,"Data 1" );
        lv_obj_set_style_text_color(chat_bar_label,lv_color_hex(0x0000ff),LV_PART_MAIN);
        lv_obj_set_style_text_font(chat_bar_label,&lv_font_montserrat_14, LV_STATE_DEFAULT);
        lv_obj_align_to(chat_bar_label,chat_bar_switch1, LV_ALIGN_OUT_LEFT_MID, -5,0 );

        chat_bar_switch2=lv_switch_create(chat2_bg);                                            /* 创建步进电机方向开关 */
        lv_obj_set_size(chat_bar_switch2,40, 20 );
        lv_obj_align(chat_bar_switch2, LV_ALIGN_TOP_RIGHT,-100, 0);             /* 设置位置 */
        lv_obj_set_style_bg_color(chat_bar_switch2,lv_color_hex(0xFF0000),LV_PART_INDICATOR|LV_STATE_CHECKED);
        lv_obj_add_event_cb(chat_bar_switch2, chat_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        lv_obj_t *chat_bar1_label = lv_label_create(chat2_bg);
        lv_label_set_text(chat_bar1_label,"Data 2" );
        lv_obj_set_style_text_color(chat_bar1_label,lv_color_hex(0xFF0000),LV_PART_MAIN);
        lv_obj_set_style_text_font(chat_bar1_label,&lv_font_montserrat_14, LV_STATE_DEFAULT);
        lv_obj_align_to(chat_bar1_label,chat_bar_switch2, LV_ALIGN_OUT_LEFT_MID, -5,0 );

        //图表清除重置按钮----------------------------------------------------------------//
        chat_bar_clean_btn = lv_btn_create(chat2_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(chat_bar_clean_btn, 70, 40);                                         /* 设置按钮大小 */
        lv_obj_set_style_bg_color(chat_bar_clean_btn,lv_color_hex(0XFF0000),LV_PART_MAIN);
        lv_obj_align(chat_bar_clean_btn,  LV_ALIGN_BOTTOM_MID, -60, 0);
        lv_obj_add_event_cb(chat_bar_clean_btn,chat_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        lv_obj_t *chat_bar_clean_label = lv_label_create(chat_bar_clean_btn);
        lv_label_set_text(chat_bar_clean_label,"Clean" );
        lv_obj_set_style_text_font(chat_bar_clean_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(chat_bar_clean_label, LV_ALIGN_CENTER, 0,0 );

        //图表缩放滑块--------------------------------------------------------------------//
        char_bar_x_slider = lv_slider_create(chat2_bg);                             /* 创建滑块 */
        lv_obj_set_size(char_bar_x_slider, 120, 10);                               /* 设置大小 */
        lv_obj_align(char_bar_x_slider, LV_ALIGN_BOTTOM_RIGHT, -90,-25 );                                                         /* 设置位置 */
        lv_slider_set_value(char_bar_x_slider, 256, LV_ANIM_OFF);                                   /* 设置当前值 */
        lv_slider_set_range(char_bar_x_slider,256,1024);/* 设置 char_bar_x_slider 部件范围值 */

        lv_obj_add_event_cb(char_bar_x_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 添加事件 */

        lv_obj_t *char_bar_x_label = lv_label_create(chat2_bg);                          /* 创建音量标签 */
        lv_label_set_text(char_bar_x_label,"X");                           /* 设置文本内容：音量图标 */
        lv_obj_set_style_text_font(char_bar_x_label,&lv_font_montserrat_16, LV_STATE_DEFAULT);                /* 设置字体 */
        lv_obj_align_to(char_bar_x_label, char_bar_x_slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);            /* 设置位置 */

        char_bar_Y_slider = lv_slider_create(chat2_bg);                             /* 创建滑块 */
        lv_obj_set_size(char_bar_Y_slider, 120, 10);                               /* 设置大小 */
        lv_obj_align(char_bar_Y_slider, LV_ALIGN_BOTTOM_RIGHT, -90,5 );                                                         /* 设置位置 */
        lv_slider_set_value(char_bar_Y_slider, 256, LV_ANIM_OFF);                                   /* 设置当前值 */
        lv_slider_set_range(char_bar_Y_slider,256,1024);/* 设置 char_bar_x_slider 部件范围值 */

        lv_obj_add_event_cb(char_bar_Y_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 添加事件 */

        lv_obj_t *char_bar_Y_label = lv_label_create(chat2_bg);                          /* 创建音量标签 */
        lv_label_set_text(char_bar_Y_label,"Y");                           /* 设置文本内容：音量图标 */
        lv_obj_set_style_text_font(char_bar_Y_label,&lv_font_montserrat_16, LV_STATE_DEFAULT);                /* 设置字体 */
        lv_obj_align_to(char_bar_Y_label, char_bar_Y_slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);            /* 设置位置 */

        //柱状图表----------------------------------------------------------------------------------
        chart2 = lv_chart_create(chat2_bg);/* 创建图表部件 */
        lv_obj_set_size(chart2,380, 220);/* 设置图表大小 */
        lv_obj_align(chart2,LV_ALIGN_TOP_LEFT,30,30);
        lv_chart_set_type(chart2, LV_CHART_TYPE_BAR);/* 设置折线图类型 */
        ser2 = lv_chart_add_series(chart2,lv_palette_main(LV_PALETTE_BLUE),LV_CHART_AXIS_SECONDARY_X);/* 添加数据序列 */
        ser3 = lv_chart_add_series(chart2,lv_palette_main(LV_PALETTE_RED),LV_CHART_AXIS_SECONDARY_X);/* 添加数据序列 */

        lv_chart_set_ext_y_array(chart2, ser2, (lv_coord_t*)chat_data2); /* 为 Y 轴数据点设置一个外部阵列 */
        lv_chart_set_ext_y_array(chart2, ser3, (lv_coord_t*)chat_data1); /* 为 Y 轴数据点设置一个外部阵列 */

        lv_chart_set_point_count(chart2,20);//设置数据序列为20个点
        lv_chart_set_div_line_count(chart2,51,0);//设置横向和竖向分隔线

        lv_chart_set_axis_tick(chart2, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 11, 1,true, 40);/* 设置 Y 轴的刻度和刻度标签，主刻度长度为 10，小刻度为 5，主刻度数量为 6，小刻度数量为 5 */
        lv_chart_set_axis_tick(chart2, LV_CHART_AXIS_PRIMARY_X, 0, 0, 20, 1,true, 30);/* 设置 X 轴的刻度和刻度标签，主刻度长度为 10，小刻度为 5，主刻度数量为 10，小刻度数量为 2 */

        //chat_bar_timer=lv_timer_create(add_data2, 200, NULL);//温度图表定时器创建
    }
    else if(target==main_5)
    {
        lv_obj_clear_flag(main_return,LV_OBJ_FLAG_HIDDEN);//显示返回上一级按钮
        lv_obj_clear_state(main_return,LV_STATE_DISABLED);

        main_btn_symbol[5]=1;

        /****创建平铺视图页面 **********************************/
        //----------------------------------------------------------------------------------定时器背景回调函数
        timer_bg=lv_obj_create (lv_scr_act());                                               /* 创建主界背景 */
        lv_obj_set_size(timer_bg, 480, 320);                                                 /* 设置主界背景大小 */
        lv_obj_set_style_radius(timer_bg,0,LV_PART_MAIN);
        lv_obj_set_style_bg_color(timer_bg,lv_color_hex(0XF5F5F5),LV_PART_MAIN);
        lv_obj_set_style_border_width(timer_bg,0,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(timer_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(timer_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(timer_bg,LV_DIR_NONE);                                         // 仅允许水平方向滚动

        lv_obj_set_style_pad_left(timer_bg, 0, 0);
        lv_obj_set_style_pad_right(timer_bg, 0, 0);
        lv_obj_set_style_pad_top(timer_bg, 0, 0);
        lv_obj_set_style_pad_bottom(timer_bg, 0, 0);
        //----------------------------------------------------------------------------------//
        timer_tileview = lv_tileview_create( timer_bg);                                  /* 创建平铺视图 */
        lv_obj_set_size(timer_bg, 480, 320);
        tile_1 = lv_tileview_add_tile( timer_tileview, 0, 0, LV_DIR_RIGHT );                /* 添加页面1 */
        tile_2 = lv_tileview_add_tile( timer_tileview, 1, 0, LV_DIR_LEFT|LV_DIR_RIGHT );    /* 添加页面2 */
        tile_3 = lv_tileview_add_tile( timer_tileview, 2, 0, LV_DIR_LEFT );                 /* 添加页面3 */

        //lv_obj_set_style_bg_color( timer_tileview,lv_color_hex(0X000000),LV_PART_MAIN);
        //秒表界面（秒 ）--------------------------------------------------------------------------//
        lv_obj_set_style_pad_left(tile_1, 0, 0);
        lv_obj_set_style_pad_right(tile_1, 0, 0);
        lv_obj_set_style_pad_top(tile_1, 0, 0);
        lv_obj_set_style_pad_bottom(tile_1, 0, 0);

        stopwatch_meter = lv_meter_create(tile_1); /* 定义并创建仪表 */
        lv_obj_set_size(stopwatch_meter, 250, 250);
        lv_obj_set_style_bg_color( stopwatch_meter,lv_color_hex(0X000000),LV_PART_MAIN);
        lv_obj_set_style_border_color( stopwatch_meter,lv_color_hex(0XFFA500),LV_PART_MAIN);
        lv_obj_set_style_border_width(stopwatch_meter,3,LV_PART_MAIN);
        lv_obj_set_style_text_color( stopwatch_meter,lv_color_hex(0XFFFFFF),LV_PART_MAIN);
        lv_obj_align(stopwatch_meter,LV_ALIGN_TOP_LEFT,10,10);

        /* 设置仪表刻度 */
        stopwwatch_scale = lv_meter_add_scale(stopwatch_meter); /* 定义并添加刻度 */
        lv_meter_set_scale_ticks(stopwatch_meter, stopwwatch_scale, 60, 4, 8,lv_color_hex(0XFFFFFF));/* 设置小刻度数量为 41，宽度为 1，长度为屏幕高度除以 80，颜色为灰色 */
        lv_meter_set_scale_major_ticks(stopwatch_meter, stopwwatch_scale, 5,6, 18,lv_color_hex(0XFFA500), 10);//设置主刻度的步长为 8,宽度为 1，长度为屏幕高度除以 60，颜色为黑色，刻度与数值的间距为屏幕高度除以 30 */

        //设置仪表指针
        stopwwatch_indic1 = lv_meter_add_needle_line(stopwatch_meter, stopwwatch_scale, 3,lv_color_hex(0XffFFFF), 0);/* 添加仪表指针，该指针宽度为 4，颜色为灰色，长度-10 */
        lv_meter_set_indicator_value(stopwatch_meter, stopwwatch_indic1,0);/* 设置指针指向的数值 */
        /* 设置仪表数值范围、有效角度和旋转角度 */
        lv_meter_set_scale_range( stopwatch_meter, stopwwatch_scale, 0, 60, 360, 270);

        //秒表界面（分）---------------------------------------------------------------------------------//
        stopwatch_meter1 = lv_meter_create(stopwatch_meter); /* 定义并创建仪表 */
        lv_obj_set_size(stopwatch_meter1, 140, 140);
        lv_obj_set_style_bg_color( stopwatch_meter1,lv_color_hex(0X000000),LV_PART_MAIN);
        lv_obj_set_style_border_color( stopwatch_meter1,lv_color_hex(0XFFA500),LV_PART_MAIN);
        lv_obj_set_style_border_width(stopwatch_meter1,0,LV_PART_MAIN);
        lv_obj_set_style_bg_opa( stopwatch_meter1,0,LV_PART_MAIN);
        lv_obj_set_style_text_color( stopwatch_meter1,lv_color_hex(0XFFFFFF),LV_PART_MAIN);
        lv_obj_set_style_text_font( stopwatch_meter1,&lv_font_montserrat_12,LV_PART_MAIN);
        lv_obj_align(stopwatch_meter1,LV_ALIGN_CENTER,0,0);

        /* 设置仪表刻度 */
        stopwwatch_scale1 = lv_meter_add_scale(stopwatch_meter1); /* 定义并添加刻度 */
        lv_meter_set_scale_ticks(stopwatch_meter1, stopwwatch_scale1, 60, 2, 4,lv_color_hex(0XFFFFFF));/* 设置小刻度数量为 41，宽度为 1，长度为屏幕高度除以 80，颜色为灰色 */
        lv_meter_set_scale_major_ticks(stopwatch_meter1, stopwwatch_scale1, 5,3, 9,lv_color_hex(0XFFA500), 10);//设置主刻度的步长为 8,宽度为 1，长度为屏幕高度除以 60，颜色为黑色，刻度与数值的间距为屏幕高度除以 30 */

        //设置仪表指针
        stopwwatch_indic11 = lv_meter_add_needle_line(stopwatch_meter1, stopwwatch_scale1, 4,lv_color_hex(0XFFFFFF), 0);/* 添加仪表指针，该指针宽度为 4，颜色为灰色，长度-10 */
        lv_meter_set_indicator_value(stopwatch_meter1, stopwwatch_indic11,0);/* 设置指针指向的数值 */
        /* 设置仪表数值范围、有效角度和旋转角度 */
        lv_meter_set_scale_range( stopwatch_meter1, stopwwatch_scale1, 0, 60, 360, 270);

        //秒表控制(启停）---------------------------------------------------------------------------------------//
        stopwatch_star_btn = lv_btn_create(tile_1);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(stopwatch_star_btn, 40, 40);                                         /* 设置按钮大小 */
        lv_obj_set_style_radius(stopwatch_star_btn,20,LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(stopwatch_star_btn,lv_color_hex(0XFFA500),LV_STATE_DEFAULT);
        lv_obj_align_to(stopwatch_star_btn, stopwatch_meter,LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

        lv_obj_add_event_cb(stopwatch_star_btn, stopwatch_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        stopwatch_star_label = lv_label_create(stopwatch_star_btn);
        lv_label_set_text(stopwatch_star_label,LV_SYMBOL_PLAY );
        lv_obj_set_style_text_font(stopwatch_star_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(stopwatch_star_label, LV_ALIGN_CENTER, 0,0 );

        //秒表控制(重置）---------------------------------------------------------------------------------------//
        stopwatch_reset_btn = lv_btn_create(tile_1);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(stopwatch_reset_btn, 40, 40);                                         /* 设置按钮大小 */
        lv_obj_set_style_radius(stopwatch_reset_btn,20,LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(stopwatch_reset_btn,lv_color_hex(0XFFA500),LV_STATE_DEFAULT);
        lv_obj_align_to(stopwatch_reset_btn, stopwatch_meter,LV_ALIGN_OUT_BOTTOM_LEFT, 30, 5);

        lv_obj_add_event_cb(stopwatch_reset_btn, stopwatch_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        stopwatch_reset_label = lv_label_create(stopwatch_reset_btn);
        lv_label_set_text(stopwatch_reset_label,LV_SYMBOL_REFRESH );
        lv_obj_set_style_text_font(stopwatch_reset_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(stopwatch_reset_label, LV_ALIGN_CENTER, 0,0 );

        //秒表控制(纪录）---------------------------------------------------------------------------------------//
        stopwatch_data_btn = lv_btn_create(tile_1);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(stopwatch_data_btn, 40, 40);                                         /* 设置按钮大小 */
        lv_obj_set_style_radius(stopwatch_data_btn,20,LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(stopwatch_data_btn,lv_color_hex(0XFFA500),LV_STATE_DEFAULT);
        lv_obj_align_to(stopwatch_data_btn, stopwatch_meter,LV_ALIGN_OUT_BOTTOM_RIGHT, -30, 5);

        lv_obj_add_event_cb(stopwatch_data_btn, stopwatch_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        stopwatch_get_label = lv_label_create(stopwatch_data_btn);
        lv_label_set_text(stopwatch_get_label,LV_SYMBOL_PLUS );
        lv_obj_set_style_text_font(stopwatch_get_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(stopwatch_get_label, LV_ALIGN_CENTER, 0,0 );
        //秒表界面标题背景-----------------------------------------------------------------------------------------//
        lv_obj_t *stopwatch_time_note_bg=lv_obj_create (tile_1);                                               /* 创建主界背景 */
        lv_obj_set_size(stopwatch_time_note_bg, 180, 40);                                                 /* 设置主界背景大小 */
        lv_obj_set_style_radius(stopwatch_time_note_bg,10,LV_PART_MAIN);
        lv_obj_align(stopwatch_time_note_bg, LV_ALIGN_TOP_RIGHT,-25,-5 );
        lv_obj_set_style_bg_color(stopwatch_time_note_bg,lv_color_hex(0XFFA500),LV_PART_MAIN);
        lv_obj_set_style_border_width(stopwatch_time_note_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(stopwatch_time_note_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(stopwatch_time_note_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(stopwatch_time_note_bg,LV_DIR_NONE);

        lv_obj_t *stopwatch_time_note_label = lv_label_create(stopwatch_time_note_bg);
        lv_label_set_text(stopwatch_time_note_label,"Stopwatch" );
        lv_obj_set_style_text_font(stopwatch_time_note_label,&lv_font_montserrat_26, LV_STATE_DEFAULT);
        lv_obj_align(stopwatch_time_note_label, LV_ALIGN_CENTER, 0,0 );
        //计时文本---------------------------------------------------------------------------------------------//
        stopwatch_time_label = lv_label_create(tile_1);
        lv_label_set_text(stopwatch_time_label,"00:00:00" );
        lv_obj_set_style_text_font(stopwatch_time_label,&lv_font_montserrat_38, LV_STATE_DEFAULT);
        lv_obj_align(stopwatch_time_label, LV_ALIGN_TOP_RIGHT, -30,40 );

        lv_obj_move_foreground(main_return);            //把返回按钮设置到图层顶端

        //秒表纪录栏-------------------------------------------------------------------------------//
        stopwatch_time_right_bg=lv_obj_create (tile_1);                                               /* 创建主界背景 */
        lv_obj_set_size(stopwatch_time_right_bg, 180, 230);                                                 /* 设置主界背景大小 */
        lv_obj_set_style_radius(stopwatch_time_right_bg,10,LV_PART_MAIN);
        lv_obj_align_to(stopwatch_time_right_bg,stopwatch_time_label, LV_ALIGN_OUT_BOTTOM_MID,0,0 );
        lv_obj_set_style_bg_color(stopwatch_time_right_bg,lv_color_hex(0XFFFFFF),LV_PART_MAIN);
        lv_obj_set_style_border_width(stopwatch_time_right_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(stopwatch_time_right_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(stopwatch_time_right_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(stopwatch_time_right_bg,LV_DIR_NONE);
        //------------------------------------------------------------------------------------------//
        stopwatch_get_1_bg= lv_obj_create(stopwatch_time_right_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(stopwatch_get_1_bg, 165, 50);                                         /* 设置按钮大小 */
        lv_obj_align(stopwatch_get_1_bg,  LV_ALIGN_TOP_MID, 0, -10);
        lv_obj_set_scroll_dir(stopwatch_get_1_bg,LV_DIR_NONE);                                         // 仅允许水平方向滚动

        lv_obj_set_style_pad_left(stopwatch_get_1_bg, 0, 0);
        lv_obj_set_style_pad_right(stopwatch_get_1_bg, 0, 0);
        lv_obj_set_style_pad_top(stopwatch_get_1_bg, 0, 0);
        lv_obj_set_style_pad_bottom(stopwatch_get_1_bg, 0, 0);

        stopwatch_get_1_label = lv_label_create(stopwatch_get_1_bg);
        lv_label_set_text(stopwatch_get_1_label,"" );
        lv_obj_set_style_text_font(stopwatch_get_1_label,&lv_font_montserrat_36, LV_STATE_DEFAULT);
        lv_obj_align(stopwatch_get_1_label, LV_ALIGN_CENTER, 0,0 );
        //--------------------------------------------------------------------------------------//
        stopwatch_get_2_bg= lv_obj_create(stopwatch_time_right_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(stopwatch_get_2_bg, 165, 50);                                         /* 设置按钮大小 */
        lv_obj_align(stopwatch_get_2_bg,  LV_ALIGN_TOP_MID, 0, 45);
        lv_obj_set_scroll_dir(stopwatch_get_2_bg,LV_DIR_NONE);                                         // 仅允许水平方向滚动

        lv_obj_set_style_pad_left(stopwatch_get_2_bg, 0, 0);
        lv_obj_set_style_pad_right(stopwatch_get_2_bg, 0, 0);
        lv_obj_set_style_pad_top(stopwatch_get_2_bg, 0, 0);
        lv_obj_set_style_pad_bottom(stopwatch_get_2_bg, 0, 0);

        stopwatch_get_2_label = lv_label_create(stopwatch_get_2_bg);
        lv_label_set_text(stopwatch_get_2_label,"" );
        lv_obj_set_style_text_font(stopwatch_get_2_label,&lv_font_montserrat_36, LV_STATE_DEFAULT);
        lv_obj_align(stopwatch_get_2_label, LV_ALIGN_CENTER, 0,0 );
        //------------------------------------------------------------------------------------//
        stopwatch_get_3_bg= lv_obj_create(stopwatch_time_right_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(stopwatch_get_3_bg, 165, 50);                                         /* 设置按钮大小 */
        lv_obj_align(stopwatch_get_3_bg,  LV_ALIGN_TOP_MID, 0, 100);
        lv_obj_set_scroll_dir(stopwatch_get_3_bg,LV_DIR_NONE);                                         // 仅允许水平方向滚动

        lv_obj_set_style_pad_left(stopwatch_get_3_bg, 0, 0);
        lv_obj_set_style_pad_right(stopwatch_get_3_bg, 0, 0);
        lv_obj_set_style_pad_top(stopwatch_get_3_bg, 0, 0);
        lv_obj_set_style_pad_bottom(stopwatch_get_3_bg, 0, 0);

        stopwatch_get_3_label = lv_label_create(stopwatch_get_3_bg);
        lv_label_set_text(stopwatch_get_3_label,"" );
        lv_obj_set_style_text_font(stopwatch_get_3_label,&lv_font_montserrat_36, LV_STATE_DEFAULT);
        lv_obj_align(stopwatch_get_3_label, LV_ALIGN_CENTER, 0,0 );
        //-------------------------------------------------------------------------------------//
        stopwatch_get_4_bg= lv_obj_create(stopwatch_time_right_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(stopwatch_get_4_bg, 165, 50);                                         /* 设置按钮大小 */
        lv_obj_align(stopwatch_get_4_bg,  LV_ALIGN_TOP_MID, 0, 155);
        lv_obj_set_scroll_dir(stopwatch_get_4_bg,LV_DIR_NONE);                                         // 仅允许水平方向滚动

        lv_obj_set_style_pad_left(stopwatch_get_4_bg, 0, 0);
        lv_obj_set_style_pad_right(stopwatch_get_4_bg, 0, 0);
        lv_obj_set_style_pad_top(stopwatch_get_4_bg, 0, 0);
        lv_obj_set_style_pad_bottom(stopwatch_get_4_bg, 0, 0);

        stopwatch_get_4_label = lv_label_create(stopwatch_get_4_bg);
        lv_label_set_text(stopwatch_get_4_label,"" );
        lv_obj_set_style_text_font(stopwatch_get_4_label,&lv_font_montserrat_36, LV_STATE_DEFAULT);
        lv_obj_align(stopwatch_get_4_label, LV_ALIGN_CENTER, 0,0 );

        //计时器---------------------------------------------------------------------------------//

        timer_arc = lv_arc_create(tile_2);
        lv_arc_set_range( timer_arc, 0, 720); /* 设置  timer_arc 的范围 */
        lv_arc_set_bg_angles( timer_arc, 0,360); /* 设置背景弧的起始角度和终止角度 */
        lv_arc_set_angles( timer_arc, 0, 360); /* 设置前景弧的起始角度和终止角度 */
        lv_arc_set_rotation(timer_arc, 270); /* 旋转 180 度 */
        lv_arc_set_value( timer_arc, 720); /* 设置  timer_arc 的值 */
        lv_obj_set_size( timer_arc, 250, 250);
        lv_obj_align( timer_arc,  LV_ALIGN_TOP_LEFT, 10, 10);

        lv_obj_set_style_arc_width(timer_arc, 20, LV_PART_MAIN);  // 设置背景弧宽度
        lv_obj_set_style_arc_width(timer_arc, 20, LV_PART_INDICATOR);  // 设置前景弧宽度
        //lv_obj_set_style_radius(timer_arc, 0, LV_PART_INDICATOR);  // 设置前景弧宽度
        lv_obj_set_style_arc_color(timer_arc, lv_palette_lighten(LV_PALETTE_ORANGE, 2), LV_PART_INDICATOR);

        lv_obj_remove_style(timer_arc, NULL, LV_PART_KNOB); /* 移除旋钮 */
        lv_obj_clear_flag(timer_arc, LV_OBJ_FLAG_CLICKABLE); /* 清除可点击属性 */

        //计时器控制(启停）---------------------------------------------------------------------------------------//
        timer_star_btn = lv_btn_create(tile_2);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(timer_star_btn, 40, 40);                                         /* 设置按钮大小 */
        lv_obj_set_style_radius(timer_star_btn,20,LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(timer_star_btn,lv_color_hex(0XFFA500),LV_STATE_DEFAULT);
        lv_obj_align_to(timer_star_btn, timer_arc,LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

        lv_obj_add_event_cb(timer_star_btn, timer_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        timer_star_label = lv_label_create(timer_star_btn);
        lv_label_set_text(timer_star_label,LV_SYMBOL_PLAY );
        lv_obj_set_style_text_font(timer_star_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(timer_star_label, LV_ALIGN_CENTER, 0,0 );

        //计时器控制(重置）---------------------------------------------------------------------------------------//
        timer_reset_btn = lv_btn_create(tile_2);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(timer_reset_btn, 40, 40);                                         /* 设置按钮大小 */
        lv_obj_set_style_radius(timer_reset_btn,20,LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(timer_reset_btn,lv_color_hex(0XFFA500),LV_STATE_DEFAULT);
        lv_obj_align_to(timer_reset_btn, timer_arc,LV_ALIGN_OUT_BOTTOM_LEFT, 30, 5);

        lv_obj_add_event_cb(timer_reset_btn, timer_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        stopwatch_reset_label = lv_label_create(timer_reset_btn);
        lv_label_set_text(stopwatch_reset_label,LV_SYMBOL_REFRESH );
        lv_obj_set_style_text_font(stopwatch_reset_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(stopwatch_reset_label, LV_ALIGN_CENTER, 0,0 );

        //计时器控制(纪录）---------------------------------------------------------------------------------------//
        timer_data_btn = lv_btn_create(tile_2);                                           /* 创建主界面设置图标 */
        lv_obj_set_size( timer_data_btn, 40, 40);                                         /* 设置按钮大小 */
        lv_obj_set_style_radius( timer_data_btn,20,LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color( timer_data_btn,lv_color_hex(0XFFA500),LV_STATE_DEFAULT);
        lv_obj_align_to( timer_data_btn, timer_arc,LV_ALIGN_OUT_BOTTOM_RIGHT, -30, 5);

        lv_obj_add_event_cb( timer_data_btn, timer_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        timer_get_label = lv_label_create( timer_data_btn);
        lv_label_set_text(timer_get_label,LV_SYMBOL_PLUS );
        lv_obj_set_style_text_font(timer_get_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(timer_get_label, LV_ALIGN_CENTER, 0,0 );

        //计时器文本------------------------------------------------------------------------------------------//
        timer_time_label = lv_label_create(timer_arc);
        lv_label_set_text(timer_time_label,"00:00:00" );
        lv_obj_set_style_text_font(timer_time_label,&lv_font_montserrat_38, LV_STATE_DEFAULT);
        lv_obj_align(timer_time_label, LV_ALIGN_CENTER, 0,0 );

        //计时设置滚轮----------------------------------------------------------------------------------------//

        hour_roller = lv_roller_create(tile_2);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options( hour_roller, timer_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align( hour_roller, LV_ALIGN_TOP_RIGHT, -150, 60);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width( hour_roller, 60);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font( hour_roller,&lv_font_montserrat_18, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count( hour_roller, 5);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected( hour_roller, 0,LV_ANIM_ON);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color( hour_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius( hour_roller,0,LV_PART_SELECTED);

        min_roller = lv_roller_create(tile_2);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options( min_roller, timer_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align( min_roller, LV_ALIGN_TOP_RIGHT, -80, 60);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width( min_roller, 60);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font( min_roller,&lv_font_montserrat_18, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count( min_roller, 5);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected( min_roller, 0,LV_ANIM_ON);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color( min_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius( min_roller,0,LV_PART_SELECTED);

        second_roller = lv_roller_create(tile_2);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options( second_roller, timer_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align( second_roller, LV_ALIGN_TOP_RIGHT, -10, 60);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width( second_roller, 60);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font( second_roller,&lv_font_montserrat_18, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count( second_roller, 5);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected( second_roller, 0,LV_ANIM_OFF);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color( second_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius( second_roller,0,LV_PART_SELECTED);
        //计时器时分秒标题----------------------------------------------------------------------------------------------//
        lv_obj_t *time_hour_bg=lv_obj_create (tile_2);                                               /* 创建主界背景 */
        lv_obj_set_size(time_hour_bg, 60, 30);                                                 /* 设置主界背景大小 */
        lv_obj_set_style_radius(time_hour_bg,10,LV_PART_MAIN);
        lv_obj_align_to(time_hour_bg,hour_roller, LV_ALIGN_OUT_BOTTOM_MID,0,10 );
        lv_obj_set_style_bg_color(time_hour_bg,lv_color_hex(0XFFA500),LV_PART_MAIN);
        lv_obj_set_style_border_width(time_hour_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(time_hour_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(time_hour_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(time_hour_bg,LV_DIR_NONE);

        lv_obj_t *time_hour_label = lv_label_create(time_hour_bg);
        lv_label_set_text(time_hour_label,"Hour" );
        lv_obj_set_style_text_font(time_hour_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(time_hour_label, LV_ALIGN_CENTER, 0,0 );

        //-----------------------------------------------------------------------------------------------------------//
        lv_obj_t *min_hour_bg=lv_obj_create (tile_2);                                               /* 创建主界背景 */
        lv_obj_set_size(min_hour_bg, 60, 30);                                                 /* 设置主界背景大小 */
        lv_obj_set_style_radius(min_hour_bg,10,LV_PART_MAIN);
        lv_obj_align_to(min_hour_bg,min_roller, LV_ALIGN_OUT_BOTTOM_MID,0,10 );
        lv_obj_set_style_bg_color(min_hour_bg,lv_color_hex(0XFFA500),LV_PART_MAIN);
        lv_obj_set_style_border_width(min_hour_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(min_hour_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(min_hour_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(min_hour_bg,LV_DIR_NONE);

        lv_obj_t *min_hour_label = lv_label_create(min_hour_bg);
        lv_label_set_text(min_hour_label,"Min" );
        lv_obj_set_style_text_font(min_hour_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(min_hour_label, LV_ALIGN_CENTER, 0,0 );
        //-----------------------------------------------------------------------------------------------------------//
        lv_obj_t *sec_hour_bg=lv_obj_create (tile_2);                                               /* 创建主界背景 */
        lv_obj_set_size(sec_hour_bg, 60, 30);                                                 /* 设置主界背景大小 */
        lv_obj_set_style_radius(sec_hour_bg,10,LV_PART_MAIN);
        lv_obj_align_to(sec_hour_bg,second_roller, LV_ALIGN_OUT_BOTTOM_MID,0,10 );
        lv_obj_set_style_bg_color(sec_hour_bg,lv_color_hex(0XFFA500),LV_PART_MAIN);
        lv_obj_set_style_border_width(sec_hour_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(sec_hour_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(sec_hour_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(sec_hour_bg,LV_DIR_NONE);

        lv_obj_t *sec_hour_label = lv_label_create(sec_hour_bg);
        lv_label_set_text(sec_hour_label,"Sec" );
        lv_obj_set_style_text_font(sec_hour_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(sec_hour_label, LV_ALIGN_CENTER, 0,0 );
        //计时器标题-----------------------------------------------------------------------------------------------//
        lv_obj_t *timer_note_bg=lv_obj_create (tile_2);                                               /* 创建主界背景 */
        lv_obj_set_size(timer_note_bg, 180, 50);                                                 /* 设置主界背景大小 */
        lv_obj_set_style_radius(timer_note_bg,10,LV_PART_MAIN);
        lv_obj_align(timer_note_bg,LV_ALIGN_TOP_RIGHT,-25,-5 );
        lv_obj_set_style_bg_color(timer_note_bg,lv_color_hex(0XFFA500),LV_PART_MAIN);
        lv_obj_set_style_border_width(timer_note_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(timer_note_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(timer_note_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(timer_note_bg,LV_DIR_NONE);

        lv_obj_t *timer_note_label = lv_label_create(timer_note_bg);
        lv_label_set_text(timer_note_label,"Timer" );
        lv_obj_set_style_text_font(timer_note_label,&lv_font_montserrat_30, LV_STATE_DEFAULT);
        lv_obj_align(timer_note_label, LV_ALIGN_CENTER, 0,0 );
    }
    else if(target==main_3)
    {

        lv_obj_clear_flag(main_return,LV_OBJ_FLAG_HIDDEN);//显示返回上一级按钮
        lv_obj_clear_state(main_return,LV_STATE_DISABLED);

        main_btn_symbol[3]=1;

        lv_timer_resume(usb_check_timer);



        /****创建平铺视图页面 **********************************/
        usb_tileview = lv_tileview_create( lv_scr_act());                                  /* 创建平铺视图 */
        lv_obj_set_size(usb_tileview, 480, 320);
        usb_tile_1 = lv_tileview_add_tile( usb_tileview, 0, 0, LV_DIR_RIGHT );                /* 添加页面1 */
        lv_obj_t *usb_tile_2 = lv_tileview_add_tile( usb_tileview, 1, 0, LV_DIR_LEFT|LV_DIR_RIGHT );    /* 添加页面2 */
        lv_obj_t *usb_tile_3 = lv_tileview_add_tile( usb_tileview, 2, 0, LV_DIR_LEFT );                 /* 添加页面3 */

        //USB读卡器标题-----------------------------------------------------------------------------------------------//
        lv_obj_t *usb_card_bg=lv_obj_create (usb_tile_1);                                               /* 创建主界背景 */
        lv_obj_set_size(usb_card_bg, 190, 30);                                                 /* 设置主界背景大小 */
        lv_obj_set_style_radius(usb_card_bg,10,LV_PART_MAIN);
        lv_obj_align(usb_card_bg,LV_ALIGN_TOP_LEFT,0,5 );
        lv_obj_set_style_bg_color(usb_card_bg,lv_color_hex(0XFFA500),LV_PART_MAIN);
        lv_obj_set_style_border_width(usb_card_bg,2,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(usb_card_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(usb_card_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(usb_card_bg,LV_DIR_NONE);
        lv_obj_set_style_radius(usb_card_bg, 30, LV_STATE_DEFAULT); /* 设置圆角 */

        lv_obj_t *usb_card_label = lv_label_create(usb_card_bg);
        lv_label_set_text(usb_card_label,"USB Card Reader" );
        lv_obj_set_style_text_font(usb_card_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(usb_card_label, LV_ALIGN_CENTER, 0,0 );

        //USB正在写入数据提示------------------------------------------------------------------
        usb_link_success_win2 = lv_obj_create(usb_tile_1); /* 创建usb连接成功窗口 */
        lv_obj_set_size(usb_link_success_win2, 200, 30);/* 设置大小 */
        lv_obj_align(usb_link_success_win2,LV_ALIGN_TOP_MID,80,5); /* 设置位置 */
        lv_obj_set_style_border_width(usb_link_success_win2, 1, LV_STATE_DEFAULT); /* 设置边框宽度 */
        lv_obj_set_style_border_color(usb_link_success_win2, lv_color_hex(0x8a8a8a),LV_STATE_DEFAULT); /* 设置边框颜色 */
        lv_obj_set_style_border_opa(usb_link_success_win2, 100, LV_STATE_DEFAULT); /* 设置边框透明度 */
        lv_obj_set_style_radius(usb_link_success_win2, 30, LV_STATE_DEFAULT); /* 设置圆角 */

        lv_obj_set_scrollbar_mode(usb_link_success_win2, LV_SCROLLBAR_MODE_OFF);              /* 隐藏滚动条 */
        lv_obj_clear_flag(usb_link_success_win2, LV_OBJ_FLAG_SCROLL_ELASTIC);  // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(usb_link_success_win2,  LV_DIR_NONE);  // 仅允许水平方向滚动

        usb_link_success_label2 = lv_label_create(usb_link_success_win2);                                      /* 创建细分值设置标签 */
        lv_label_set_text(usb_link_success_label2, "USB DisConnected");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(usb_link_success_label2, &lv_font_montserrat_20, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align(usb_link_success_label2, LV_ALIGN_CENTER, 0, 0 );

        //usb读卡器重新初始化////////////////////////////////////////////////////////////////////////////////////////////////////
        usb_card_restart_btn = lv_btn_create(usb_tile_1);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(usb_card_restart_btn, 90, 30);                                         /* 设置按钮大小 */
        lv_obj_align_to(usb_card_restart_btn,usb_card_bg,  LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
        lv_obj_set_style_radius(usb_card_restart_btn, 30, LV_STATE_DEFAULT); /* 设置圆角 */
        lv_obj_set_style_bg_color(usb_card_restart_btn,lv_color_hex(0XFFA500),LV_PART_MAIN);
        lv_obj_add_event_cb(usb_card_restart_btn, usb_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        lv_obj_t *usb_card_restart_label = lv_label_create(usb_card_restart_btn);
        lv_label_set_text(usb_card_restart_label,"Restart" );
        lv_obj_set_style_text_font(usb_card_restart_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
        lv_obj_align(usb_card_restart_label, LV_ALIGN_CENTER, 0,0 );
        lv_obj_set_style_text_color(usb_card_restart_label,lv_color_hex(0x000000),LV_STATE_DEFAULT);


        lv_obj_move_foreground(main_return);            //把返回按钮设置到图层顶端

    }
    else if(target==main_4)
    {

        lv_obj_clear_flag(main_return,LV_OBJ_FLAG_HIDDEN);//显示返回上一级按钮
        lv_obj_clear_state(main_return,LV_STATE_DISABLED);

        main_btn_symbol[4]=1;

        //IOT界面平铺视图--------------------------------------------------------//
        /****创建平铺视图页面 **********************************/
        iot_tileview = lv_tileview_create( lv_scr_act());                                  /* 创建平铺视图 */
        lv_obj_set_size(iot_tileview, 480, 320);
        lv_obj_t *iot_tile_1 = lv_tileview_add_tile( iot_tileview, 0, 0, LV_DIR_BOTTOM );                /* 添加页面1 */
        lv_obj_t *iot_tile_2 = lv_tileview_add_tile( iot_tileview, 0, 1, LV_DIR_TOP|LV_DIR_BOTTOM );    /* 添加页面2 */
        lv_obj_t *iot_tile_3 = lv_tileview_add_tile( iot_tileview, 0, 2, LV_DIR_TOP );                 /* 添加页面3 */
        lv_obj_set_style_bg_color(iot_tileview,lv_color_hex(0XFFFFFF),LV_PART_MAIN);

        //空调选项界面背景----------------------------------------------------------//
        lv_obj_t *AC_set_bg=lv_obj_create (iot_tile_1);                                               /* 创建主界背景 */
        lv_obj_set_size(AC_set_bg, 200, 300);                                                 /* 设置主界背景大小 */
        lv_obj_align(AC_set_bg,LV_ALIGN_TOP_RIGHT,-10,10 );
        lv_obj_set_style_bg_color(AC_set_bg,lv_color_hex(0X000000),LV_PART_MAIN);
        lv_obj_set_style_border_width(AC_set_bg,1,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(AC_set_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(AC_set_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(AC_set_bg,LV_DIR_NONE);
        lv_obj_set_style_radius(AC_set_bg, 20, LV_STATE_DEFAULT); /* 设置圆角 */

        AC_temp_label = lv_label_create(AC_set_bg);                                      /* 创建细分值设置标签 */
		lv_label_set_text(AC_temp_label,"22" );
		lv_obj_set_style_text_font(AC_temp_label,&lv_font_montserrat_48, LV_STATE_DEFAULT);
        lv_obj_align(AC_temp_label,LV_ALIGN_TOP_LEFT,-5,10 );
        lv_obj_set_style_text_color(AC_temp_label,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);

        lv_obj_t *AC_temp_note_label = lv_label_create(AC_set_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(AC_temp_note_label, "温度设置");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(AC_temp_note_label, &LVFONT16_04, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align(AC_temp_note_label,LV_ALIGN_TOP_MID,0,-10 );
        lv_obj_set_style_text_color(AC_temp_note_label,lv_color_hex(0x00FFFF),LV_STATE_DEFAULT);

        //空调温度提高按钮--------------------------------------------------------------------------//
        lv_obj_t *AC_temp_up = lv_btn_create(AC_set_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(AC_temp_up, 40, 40);                                         /* 设置按钮大小 */
        lv_obj_align(AC_temp_up, LV_ALIGN_TOP_RIGHT, -45, 15);
        lv_obj_add_event_cb( AC_temp_up, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */
        lv_obj_set_style_radius(AC_temp_up, 40, LV_STATE_DEFAULT); /* 设置圆角 */
        lv_obj_set_style_bg_color(AC_temp_up,lv_color_hex(0X1E90FF),LV_PART_MAIN);

        lv_obj_t *AC_temp_up_label = lv_label_create(AC_temp_up);
        lv_label_set_text(AC_temp_up_label,LV_SYMBOL_UP );
        lv_obj_set_style_text_font(AC_temp_up_label,&lv_font_montserrat_26, LV_STATE_DEFAULT);
        lv_obj_align(AC_temp_up_label, LV_ALIGN_CENTER, 0,0 );

        //空调温度降低提高按钮--------------------------------------------------------------------------//
        lv_obj_t *AC_temp_down = lv_btn_create(AC_set_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(AC_temp_down, 40, 40);                                         /* 设置按钮大小 */
        lv_obj_align(AC_temp_down, LV_ALIGN_TOP_RIGHT, 5, 15);
        lv_obj_add_event_cb( AC_temp_down, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */
        lv_obj_set_style_radius(AC_temp_down, 40, LV_STATE_DEFAULT); /* 设置圆角 */

        lv_obj_t *AC_temp_down_label = lv_label_create(AC_temp_down);
        lv_label_set_text(AC_temp_down_label,LV_SYMBOL_DOWN );
        lv_obj_set_style_text_font(AC_temp_down_label,&lv_font_montserrat_26, LV_STATE_DEFAULT);
        lv_obj_align(AC_temp_down_label, LV_ALIGN_CENTER, 0,0 );

        lv_obj_t*AC_temp_line = lv_line_create(AC_set_bg);
        lv_line_set_points(AC_temp_line , line_points, 2);
        lv_obj_align(AC_temp_line,LV_ALIGN_CENTER,0,-70 );
        lv_obj_set_style_line_color(AC_temp_line, lv_color_hex(0XFFFFFF), LV_PART_MAIN);

        //空调定时选项----------------------------------------------------------------------//
        AC_time_label = lv_label_create(AC_set_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(AC_time_label, "00:00");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(AC_time_label, &lv_font_montserrat_28, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align(AC_time_label,LV_ALIGN_TOP_LEFT,-5,90 );
        lv_obj_set_style_text_color(AC_time_label,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);

        lv_obj_t *AC_time_note_label = lv_label_create(AC_set_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(AC_time_note_label, "定时模式");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(AC_time_note_label, &LVFONT16_04, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align(AC_time_note_label,LV_ALIGN_TOP_MID,0,65 );
        lv_obj_set_style_text_color(AC_time_note_label,lv_color_hex(0x00FFFF),LV_STATE_DEFAULT);

        //空调温度提高按钮--------------------------------------------------------------------------//
        lv_obj_t *AC_time_up = lv_btn_create(AC_set_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(AC_time_up, 40, 40);                                         /* 设置按钮大小 */
        lv_obj_align(AC_time_up, LV_ALIGN_TOP_RIGHT, -45, 85);
        lv_obj_add_event_cb( AC_time_up, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */
        lv_obj_set_style_radius(AC_time_up, 40, LV_STATE_DEFAULT); /* 设置圆角 */
        lv_obj_set_style_bg_color(AC_time_up,lv_color_hex(0X1E90FF),LV_PART_MAIN);

        lv_obj_t *AC_time_up_label = lv_label_create(AC_time_up);
        lv_label_set_text(AC_time_up_label,LV_SYMBOL_UP );
        lv_obj_set_style_text_font(AC_time_up_label,&lv_font_montserrat_26, LV_STATE_DEFAULT);
        lv_obj_align(AC_time_up_label, LV_ALIGN_CENTER, 0,0 );
        //空调温度降低提高按钮--------------------------------------------------------------------------//
        lv_obj_t *AC_time_down = lv_btn_create(AC_set_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(AC_time_down, 40, 40);                                         /* 设置按钮大小 */
        lv_obj_align(AC_time_down, LV_ALIGN_TOP_RIGHT, 5, 85);
        lv_obj_add_event_cb( AC_time_down, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */
        lv_obj_set_style_radius(AC_time_down, 40, LV_STATE_DEFAULT); /* 设置圆角 */

        lv_obj_t *AC_time_down_label = lv_label_create(AC_time_down);
        lv_label_set_text(AC_time_down_label,LV_SYMBOL_DOWN );
        lv_obj_set_style_text_font(AC_time_down_label,&lv_font_montserrat_26, LV_STATE_DEFAULT);
        lv_obj_align(AC_time_down_label, LV_ALIGN_CENTER, 0,0 );

        lv_obj_t*AC_time_line = lv_line_create(AC_set_bg);
        lv_line_set_points(AC_time_line , line_points, 2);
        lv_obj_align(AC_time_line,LV_ALIGN_CENTER,0,0 );
        lv_obj_set_style_line_color(AC_time_line, lv_color_hex(0XFFFFFF), LV_PART_MAIN);

        //空调模式滚轮----------------------------------------------------------------------//
        lv_obj_t*AC_mode_roller = lv_roller_create(AC_set_bg);                                    /* 创建细分值设置滚轮 */
        lv_roller_set_options( AC_mode_roller, AC_mode_options, LV_ROLLER_MODE_NORMAL);            /* 滚轮添加选项、设置正常模式 */
        lv_obj_align( AC_mode_roller, LV_ALIGN_CENTER, 60, 85);                                  /* 设置细分值设置滚轮位置 */
        lv_obj_set_width( AC_mode_roller, 60);/* 设置细分值设置滚轮宽度 */
        lv_obj_set_style_text_font( AC_mode_roller,&lv_font_montserrat_18, LV_STATE_DEFAULT);    /* 设置细分值设置滚轮字体 */
        lv_roller_set_visible_row_count( AC_mode_roller, 3);                                     /* 设置细分值设置滚轮可见选项个数 */
        lv_roller_set_selected( AC_mode_roller, 2,LV_ANIM_ON);                                 /* 设置细分值设置滚轮当前所选项 */
        lv_obj_set_style_bg_color( AC_mode_roller,lv_color_hex(0X1f4287),LV_PART_SELECTED);
        lv_obj_set_style_radius( AC_mode_roller,0,LV_PART_SELECTED);
        lv_obj_set_style_bg_color(AC_mode_roller,lv_color_hex(0X000000),LV_PART_MAIN);
        lv_obj_set_style_border_width(AC_mode_roller,0,LV_PART_MAIN);
        lv_obj_set_style_text_color(AC_mode_roller, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

        //空调模式标题 ----------------------------------------------------------------------//
        lv_obj_t *AC_mode_note_label = lv_label_create(AC_set_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(AC_mode_note_label, "模式");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(AC_mode_note_label, &LVFONT16_04, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align(AC_mode_note_label,LV_ALIGN_CENTER,60,20 );
        lv_obj_set_style_text_color(AC_mode_note_label,lv_color_hex(0x00FFFF),LV_STATE_DEFAULT);

        //分隔线2----------------------------------------------------------------------------//
        lv_obj_t*AC_temp_line2 = lv_line_create(AC_set_bg);
        lv_line_set_points(AC_temp_line2 , line_points2, 2);
        lv_obj_align(AC_temp_line2,LV_ALIGN_CENTER,20,90 );
        lv_obj_set_style_line_color(AC_temp_line2, lv_color_hex(0XFFFFFF), LV_PART_MAIN);

        lv_obj_t* AC_fan_slider = lv_slider_create(AC_set_bg);/* 创建 slider 部件 */
        lv_slider_set_range(AC_fan_slider,0,5);/* 设置 slider 部件范围值 */
        lv_obj_set_size(AC_fan_slider,100,10);
        lv_slider_set_value(AC_fan_slider,2, LV_ANIM_ON);/* 设置 slider 部件当前值 */
        lv_obj_align(AC_fan_slider,LV_ALIGN_CENTER,-40,130 );
        lv_obj_set_style_bg_color( AC_fan_slider,lv_color_hex(0XFFFFFF),LV_PART_KNOB);

        lv_obj_t *AC_fan_label = lv_label_create(AC_set_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(AC_fan_label, "风档");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(AC_fan_label, &LVFONT16_04, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align(AC_fan_label,LV_ALIGN_CENTER,0,115 );
        lv_obj_set_style_text_color(AC_fan_label,lv_color_hex(0x00FFFF),LV_STATE_DEFAULT);

        //分隔线3----------------------------------------------------------------------------//
        lv_obj_t*AC_temp_line3 = lv_line_create(AC_set_bg);
        lv_line_set_points(AC_temp_line3 , line_points3, 2);
        lv_obj_align(AC_temp_line3,LV_ALIGN_CENTER,-40,100 );
        lv_obj_set_style_line_color(AC_temp_line3, lv_color_hex(0XFFFFFF), LV_PART_MAIN);

        //分隔线4----------------------------------------------------------------------------//
        lv_obj_t*AC_temp_line4 = lv_line_create(AC_set_bg);
        lv_line_set_points(AC_temp_line4 , line_points3, 2);
        lv_obj_align(AC_temp_line4,LV_ALIGN_CENTER,-40,50 );
        lv_obj_set_style_line_color(AC_temp_line4, lv_color_hex(0XFFFFFF), LV_PART_MAIN);

        //空调开关------------------------------------------------------------------------------//
        lv_obj_t *AC_ON_switch = lv_switch_create(AC_set_bg);                                           /* 创建主界面设置图标 */
        lv_obj_set_size(AC_ON_switch, 60, 35);                                         /* 设置按钮大小 */
        lv_obj_align(AC_ON_switch, LV_ALIGN_CENTER, -20, 75);
        //lv_obj_add_event_cb( AC_ON_switch, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

        lv_obj_t *AC_ON_label = lv_label_create(AC_set_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(AC_ON_label, "开");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(AC_ON_label, &LVFONT20_04, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align_to(AC_ON_label,AC_ON_switch,LV_ALIGN_OUT_LEFT_MID,-5,0 );
        lv_obj_set_style_text_color(AC_ON_label,lv_color_hex(0x00FFFF),LV_STATE_DEFAULT);
		

        //空调图片----------------------------------------------------------------------------//
		lv_png_init(); /* 初始化 PNG 解码库 */
        lv_obj_t * AC_img = lv_img_create(iot_tile_1);
        lv_img_set_src(AC_img, "0:/PICTURE/PNG/AC.bin");//SD卡路径
		//lv_img_set_src(AC_img, "1:/SYSTEM/PICTURE/mlljt.png");//FLASH路径
        lv_obj_align(AC_img,LV_ALIGN_LEFT_MID,20,-30);
		
		lv_obj_t *AC_note_bg=lv_obj_create (iot_tile_1);                                               /* 创建主界背景 */
        lv_obj_set_size(AC_note_bg, 240, 30);                                                 /* 设置主界背景大小 */
        lv_obj_align(AC_note_bg,LV_ALIGN_BOTTOM_LEFT,25,-60 );
        lv_obj_set_style_bg_color(AC_note_bg,lv_color_hex(0X000000),LV_PART_MAIN);
        lv_obj_set_style_border_width(AC_note_bg,0,LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(AC_note_bg, LV_SCROLLBAR_MODE_OFF);                          /* 隐藏滚动条 */
        lv_obj_clear_flag(AC_note_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);                             // 禁用父对象 A 的弹性滚动
        lv_obj_set_scroll_dir(AC_note_bg,LV_DIR_NONE);
        lv_obj_set_style_radius(AC_note_bg, 0, LV_STATE_DEFAULT); /* 设置圆角 */

        /* 设置渐变背景色 */
        lv_obj_set_style_bg_grad_dir(AC_note_bg, LV_GRAD_DIR_HOR, LV_PART_MAIN); // 水平方向渐变
        lv_obj_set_style_bg_color(AC_note_bg, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // 左侧白色
        lv_obj_set_style_bg_grad_color(AC_note_bg, lv_color_hex(0x000000), LV_PART_MAIN); // 右侧黑色
        lv_obj_set_style_bg_opa(AC_note_bg, LV_OPA_COVER, LV_PART_MAIN); // 确保背景可见

        lv_obj_t *AC_note_label = lv_label_create(AC_note_bg);                                      /* 创建细分值设置标签 */
        lv_label_set_text(AC_note_label, "Air Conditioning ");                                      /* 设置细分值设置文本内容 */
        lv_obj_set_style_text_font(AC_note_label, &lv_font_montserrat_16, LV_STATE_DEFAULT);    /* 设置细分值设置字体 */
        lv_obj_align(AC_note_label,LV_ALIGN_RIGHT_MID,0,0 );
        lv_obj_set_style_text_color(AC_note_label,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
		
		
		
		
        lv_obj_move_foreground(main_return);            //把返回按钮设置到图层顶端

    }
}
//---------------------------------------------------------------//
///-----------------------------------------------------------------///

// 定时器回调函数
static void my_timer_cb(lv_timer_t * timer)
{
   int move;

	move=lv_obj_get_scroll_x(main_bg);
	//printf("%d",move);
	if(move<1150)
    {
    lv_obj_set_pos(main_bg2,move,0);
    }
//第一个
    if(move<0)
    {
        uint8_t size1=120*(160+move)/160;
        if (size1<60)size1=60;
        if (size1>120)size1=120;
        lv_obj_set_size(main_0,size1,size1);
    }
	if(move<30&&move>0)
    {
        lv_obj_set_size(main_0,120,120);
    }
	if(move>30&&move<190)
    {
        uint8_t size1=120*160/(move-30+160);
        lv_obj_set_size(main_0,size1,size1);
    }
//第二个
    if(move>=0&&move<160)
    {
        uint8_t size1=120*(160+move)/320;
        lv_obj_set_size(main_1,size1,size1);
    }
    if(move>200&&move<360)
    {
        uint8_t size1=120*160/(move-200+160);
        lv_obj_set_size(main_1,size1,size1);
    }
//第三个
    if(move<140)
    {
        lv_obj_set_size(main_2,60,60);
    }
    if(move>=140&&move<300)
    {
        uint8_t size1=120*(160+move-140)/320;
        lv_obj_set_size(main_2,size1,size1);
    }
    if(move>340&&move<500)
    {
        uint8_t size1=120*160/(move-340+160);
        lv_obj_set_size(main_2,size1,size1);
    }
//第四个
    if(move<290)
    {
        lv_obj_set_size(main_3,60,60);
    }
    if(move>=290&&move<450)
    {
        uint8_t size1=120*(160+move-290)/320;
        lv_obj_set_size(main_3,size1,size1);
    }
    if(move>500&&move<660)
    {
        uint8_t size1=120*160/(move-500+160);
        lv_obj_set_size(main_3,size1,size1);
    }
//第五个
    if(move<450)
    {
        lv_obj_set_size(main_4,60,60);
    }
    if(move>=450&&move<610)
    {
        uint8_t size1=120*(160+move-450)/320;
        lv_obj_set_size(main_4,size1,size1);
    }
    if(move>670&&move<830)
    {
        uint8_t size1=120*160/(move-670+160);
        lv_obj_set_size(main_4,size1,size1);
    }
//第六个
    if(move<620)
    {
        lv_obj_set_size(main_5,60,60);
    }
    if(move>=620&&move<780)
    {
        uint8_t size1=120*(160+move-620)/320;
        lv_obj_set_size(main_5,size1,size1);
    }
    if(move>840&&move<1000)
    {
        uint8_t size1=120*160/(move-840+160);
        lv_obj_set_size(main_5,size1,size1);
    }
//第7个
    if(move<790)
    {
        lv_obj_set_size(main_6,60,60);
    }
    if(move>=790&&move<950)
    {
        uint8_t size1=120*(160+move-790)/320;
        lv_obj_set_size(main_6,size1,size1);
    }
    if(move>990&&move<1150)
    {
        uint8_t size1=120*160/(move-990+160);
        lv_obj_set_size(main_6,size1,size1);
    }
}



void KZ_MAIN(void)//控制中心主界面函数
{
	vTaskSuspend(ADC3Task_Handler);
	
    lv_png_init(); /* 初始化 PNG 解码库 */
//-------------------------------------------------------------------------------/滚动菜单定时器创建
    my_timer = lv_timer_create(my_timer_cb, 1, NULL);

    chat_temp_timer=lv_timer_create(add_data, 200, NULL);//温度图表定时器创建
    lv_timer_pause(chat_temp_timer);

    chat_bar_timer=lv_timer_create(add_data2, 200, NULL);//柱状图表定时器创建
    lv_timer_pause(chat_bar_timer);

    //创建秒表定时器
    stopwatch_timer=lv_timer_create(stopwatch_timer_event, 10, NULL);
    lv_timer_pause(stopwatch_timer);

    //创建计时器定时器
    timer_timer=lv_timer_create(timer_timer_event, 1000, NULL);
    lv_timer_pause( timer_timer);

    //创建USB检测定时器
    usb_check_timer=lv_timer_create(usb_check_timer_event, 100, NULL);
    lv_timer_pause( usb_check_timer);


//字体颜色本地样式-----------------------------------------------------------------------------------------------
    lv_style_init(&world_style);
    lv_style_set_text_color(&world_style, lv_color_hex(0X1f4287));
//------------------------------------------------------------------------------/主界面按钮基本样式创建
    lv_style_init(&main_style); /* 初始化样式 */
    lv_style_set_radius(&main_style,60);
    lv_style_set_border_width(&main_style, 3); /* 设置边框的宽度 */
    lv_style_set_border_color(&main_style,lv_color_hex(0Xdbe2ef));
    lv_style_set_bg_color(&main_style, lv_color_hex(0x3d84a8)); // 设置背景颜色（起始颜色 - 橙色）
 //   lv_style_set_bg_grad_color(&main_style, lv_color_hex(0xdbe2ef));// 设置渐变颜色（结束颜色 - 白色）
 //   lv_style_set_bg_grad_dir(&main_style, LV_GRAD_DIR_VER);// 设置渐变方向（垂直渐变）

    // 添加阴影
   // lv_style_set_shadow_color(&main_style,lv_color_hex(0x3f72af));  // 阴影颜色（黑色）
    lv_style_set_shadow_opa(&main_style, LV_OPA_0);  // 阴影透明度（50%）
//------------------------------------------------------------------------------/
    lv_obj_set_style_bg_color(lv_scr_act(),lv_color_black(),LV_PART_MAIN);
    lv_obj_set_scroll_dir(lv_scr_act(), LV_DIR_VER);                                   // 仅允许水平方向滚动
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLL_ELASTIC);                        // 禁用父对象 A 的弹性滚动
//主界面背景--------------------------------------------------------------------------------

    main_bg=lv_obj_create (lv_scr_act());                                        /* 创建主界背景 */
    lv_obj_set_size(main_bg, 480, 320);                                          /* 设置主界背景大小 */
    lv_obj_set_style_bg_color(main_bg,lv_color_hex(0X1f4287),LV_PART_MAIN);
    lv_obj_set_style_border_width(main_bg,0,LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(main_bg, LV_SCROLLBAR_MODE_OFF);              /* 隐藏滚动条 */
    lv_obj_clear_flag(main_bg, LV_OBJ_FLAG_SCROLL_ELASTIC);  // 禁用父对象 A 的弹性滚动
    lv_obj_set_scroll_dir(main_bg, LV_DIR_ALL);  // 仅允许水平方向滚动

     // 设置背景渐变起始颜色
    //lv_obj_set_style_bg_grad_color(main_bg,lv_color_hex(0Xf9f7f7), 0);  // 从左到右渐变，起始颜色为蓝色
    // 设置背景渐变结束颜色
    //lv_obj_set_style_bg_grad_color(main_bg, lv_color_hex(0X112d4e), LV_PART_MAIN);  // 结束颜色为红色
    // 设置渐变方向为水平
    //lv_obj_set_style_bg_grad_dir(main_bg, LV_GRAD_DIR_VER, LV_PART_MAIN);

    lv_obj_add_event_cb(main_bg, main_bg_event, LV_EVENT_SCROLL_BEGIN, NULL);
    lv_obj_add_event_cb(main_bg, main_bg_event, LV_EVENT_SCROLL_END, NULL);

    lv_obj_set_style_pad_left(main_bg, 0, 0);
    lv_obj_set_style_pad_right(main_bg, 0, 0);
    lv_obj_set_style_pad_top(main_bg, 0, 0);
    lv_obj_set_style_pad_bottom(main_bg, 0, 0);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//创建返回上一级按钮
    main_return = lv_btn_create(lv_scr_act());                                        //返回上一级按钮创建//
    lv_obj_set_size(main_return, 40, 30);                                             /* 设置按钮大小 */
    lv_obj_align(main_return, LV_ALIGN_TOP_RIGHT, 20, 0);                              /* 设置按钮位置 */
    lv_obj_set_style_bg_color(main_return,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
    lv_obj_set_style_bg_opa(main_return,LV_OPA_100,LV_STATE_DEFAULT);
    lv_obj_set_style_radius(main_return,20,LV_STATE_DEFAULT);

    main_return_pic = lv_label_create( main_return);                                   //返回按钮标签创建
    lv_label_set_text(main_return_pic,LV_SYMBOL_LEFT );
    lv_obj_set_style_text_font(main_return_pic,&lv_font_montserrat_26, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(main_return_pic,lv_color_hex(0X1f4287),LV_PART_MAIN);
    lv_obj_align(main_return_pic, LV_ALIGN_CENTER, -10,0 );
    lv_obj_move_foreground(main_return);                                               //把返回按钮设置到图层顶端

    lv_obj_add_event_cb(main_return, moto_close_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

    lv_obj_add_flag(main_return,LV_OBJ_FLAG_HIDDEN);                                  //主界面隐藏返回按钮
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//主界面左上角时间日期-------------------------------------------------------------------------
    data_obj=lv_obj_create (lv_scr_act());                                        /* 创建主界背景 */
    lv_obj_set_size(data_obj, 180,40);                                          /* 设置主界背景大小 */
    lv_obj_align(data_obj, LV_ALIGN_TOP_LEFT, -20,10 );
    lv_obj_set_style_radius(data_obj,20,LV_PART_MAIN);
    lv_obj_set_style_bg_color(data_obj,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
    lv_obj_set_style_border_width(data_obj,4,LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(data_obj, LV_SCROLLBAR_MODE_OFF);              /* 隐藏滚动条 */
    lv_obj_clear_flag(data_obj, LV_OBJ_FLAG_SCROLL_ELASTIC);  // 禁用父对象 A 的弹性滚动
    lv_obj_set_scroll_dir(data_obj,  LV_DIR_NONE);  // 仅允许水平方向滚动

    lv_obj_set_style_pad_left(data_obj, 0, 0);
    lv_obj_set_style_pad_right(data_obj, 0, 0);
    lv_obj_set_style_pad_top(data_obj, 5, 0);
    lv_obj_set_style_pad_bottom(data_obj, 5, 0);

    time_obj=lv_obj_create (lv_scr_act());                                        /* 创建主界背景 */
    lv_obj_set_size(time_obj, 180,40);                                          /* 设置主界背景大小 */
    lv_obj_align(time_obj, LV_ALIGN_TOP_LEFT, -20,60 );
    lv_obj_set_style_radius(time_obj,20,LV_PART_MAIN);
    lv_obj_set_style_bg_color(time_obj,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
    lv_obj_set_style_border_width(time_obj,4,LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(time_obj, LV_SCROLLBAR_MODE_OFF);              /* 隐藏滚动条 */
    lv_obj_clear_flag(time_obj, LV_OBJ_FLAG_SCROLL_ELASTIC);  // 禁用父对象 A 的弹性滚动
    lv_obj_set_scroll_dir(time_obj,  LV_DIR_NONE);  // 仅允许水平方向滚动

    lv_obj_set_style_pad_left(time_obj, 0, 0);
    lv_obj_set_style_pad_right(time_obj, 0, 0);
    lv_obj_set_style_pad_top(time_obj, 5, 0);
    lv_obj_set_style_pad_bottom(time_obj, 5, 0);

    time_labal = lv_label_create( data_obj);
    lv_label_set_text(time_labal,"8:00 AM" );
    lv_obj_set_style_text_font(time_labal,&LVFONT16_04, LV_STATE_DEFAULT);
    lv_obj_align(time_labal, LV_ALIGN_LEFT_MID, 30,0 );

    data_labal = lv_label_create( time_obj);
    lv_label_set_text(data_labal,"Fir 2025/2/14" );
    lv_obj_set_style_text_font(data_labal,&lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_align(data_labal, LV_ALIGN_LEFT_MID, 25,0 );
    

//主界面菜单文本-------------------------------------------------------------------------------
    main_text=lv_obj_create (lv_scr_act());                                        /* 创建主界背景 */
    lv_obj_set_size(main_text, 120, 45);                                          /* 设置主界背景大小 */
    lv_obj_align(main_text,LV_ALIGN_CENTER,0,130);
    lv_obj_set_style_bg_color(main_text,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
    lv_obj_set_style_radius(main_text,25,LV_PART_MAIN);

    main_text_label = lv_label_create( main_text);
    lv_label_set_text(main_text_label,"Setting" );
    lv_obj_set_style_text_font(main_text_label,&lv_font_montserrat_26, LV_STATE_DEFAULT);
    lv_obj_align(main_text_label, LV_ALIGN_CENTER, 0,0 );
    lv_obj_set_scrollbar_mode(main_text, LV_SCROLLBAR_MODE_OFF);              /* 隐藏滚动条 */
    lv_obj_clear_flag(main_text, LV_OBJ_FLAG_SCROLL_ELASTIC);  // 禁用父对象 A 的弹性滚动
    lv_obj_set_scroll_dir(main_text, LV_DIR_NONE );  // 仅允许水平方向滚动

//下拉通知栏------------------------------------------------------------------------------------
    note=lv_obj_create (lv_scr_act());                                        /* 创建下拉通知栏 */
    lv_obj_set_size( note, 440, 255);                                          /* 设置下拉通知栏大小 */
    lv_obj_align(note,LV_ALIGN_CENTER,0,-320);
    lv_obj_set_style_radius(note,30,LV_PART_MAIN);
    lv_obj_set_scroll_dir( note, LV_DIR_NONE );

    note_down=lv_btn_create (lv_scr_act());                                        /* 创建下拉通知栏 */
    lv_obj_set_size( note_down, 120, 45);                                          /* 设置下拉通知栏大小 */
    lv_obj_set_style_bg_color(note_down,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
    lv_obj_align_to(note_down,note,LV_ALIGN_OUT_BOTTOM_MID,0,35);
    lv_obj_set_style_radius(note_down,20,LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(note_down, LV_SCROLLBAR_MODE_OFF);              /* 隐藏滚动条 */
    lv_obj_add_event_cb(note_down, note_event, LV_EVENT_CLICKED, NULL);

    note_down_label = lv_label_create(note_down);
    lv_label_set_text(note_down_label,LV_SYMBOL_DOWN );
    lv_obj_set_style_text_font(note_down_label,&lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_align(note_down_label, LV_ALIGN_CENTER, 0,0 );

    // 初始化动画
    lv_anim_init(&note_anim);
    lv_anim_set_var(&note_anim, note); // 设置动画对象
    lv_anim_set_exec_cb(&note_anim, note_y); // 设置动画回调
    lv_anim_set_time(&note_anim, 600); // 设置动画时长（600ms）
    lv_anim_set_path_cb(&note_anim, lv_anim_path_ease_out); // 设置动画路径（缓出效果）

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//背景装饰带
    main_bg1=lv_obj_create(main_bg);
    lv_obj_set_pos(main_bg1,0,110);
    lv_obj_set_size(main_bg1, 1480, 100);                                          /* 设置主界背景大小 */
    lv_obj_set_style_radius(main_bg1,0,LV_PART_MAIN);
    lv_obj_clear_state(main_bg1,LV_STATE_SCROLLED);
    lv_obj_set_style_bg_color(main_bg1,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
//背景装饰圆
    main_bg2=lv_obj_create(main_bg);
    lv_obj_align(main_bg2,LV_ALIGN_CENTER,0,0);
    lv_obj_set_size(main_bg2, 160, 160);                                          /* 设置主界背景大小 */
    lv_obj_set_style_radius(main_bg2,100,LV_PART_MAIN);
    lv_obj_set_style_bg_color(main_bg2,lv_color_hex(0xeeeeee),LV_PART_MAIN);
   // lv_obj_set_style_border_width(main_bg2,10,LV_PART_MAIN);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    main_0 = lv_btn_create(main_bg);                                           /* 创建主界面设置图标 */
    lv_obj_set_size(main_0, 120, 120);                                         /* 设置按钮大小 */
    lv_obj_align(main_0,  LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(main_0, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

    lv_obj_t *main_set_label = lv_label_create(main_0);
    lv_label_set_text(main_set_label,LV_SYMBOL_SETTINGS );
    lv_obj_set_style_text_font(main_set_label,&lv_font_montserrat_48, LV_STATE_DEFAULT);
    lv_obj_align(main_set_label, LV_ALIGN_CENTER, 0,0 );

///////////////////////////////////////////////////////////////////////////////////////////////////////////
    main_1 = lv_btn_create(main_bg);                                           /* 创建主界面设置图标 */
    lv_obj_set_size(main_1, 120, 120);                                         /* 设置按钮大小 */
    lv_obj_align(main_1, LV_ALIGN_CENTER,160, 0);
    lv_obj_add_event_cb(main_1, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
    main_2 = lv_btn_create(main_bg);                                           /* 创建主界面设置图标 */
    lv_obj_set_size(main_2, 120, 120);                                         /* 设置按钮大小 */
    lv_obj_align(main_2, LV_ALIGN_CENTER, 320, 0);
    lv_obj_add_event_cb( main_2, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

    lv_obj_t *main_chat_label = lv_label_create(main_2);
    lv_label_set_text(main_chat_label,LV_SYMBOL_LIST );
    lv_obj_set_style_text_font(main_chat_label,&lv_font_montserrat_48, LV_STATE_DEFAULT);
    lv_obj_align(main_chat_label, LV_ALIGN_CENTER, 0,0 );

////////////////////////////////////////////////////////////////////////////////////////////////////////////
    main_3 = lv_btn_create(main_bg);                                           /* 创建主界面设置图标 */
    lv_obj_set_size(main_3, 120, 120);                                         /* 设置按钮大小 */
    lv_obj_align(main_3, LV_ALIGN_CENTER, 480, 0);
    lv_obj_add_event_cb(main_3, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

    lv_obj_t *main_USB_label = lv_label_create(main_3);
    lv_label_set_text(main_USB_label,LV_SYMBOL_USB );
    lv_obj_set_style_text_font(main_USB_label,&lv_font_montserrat_48, LV_STATE_DEFAULT);
    lv_obj_align(main_USB_label, LV_ALIGN_CENTER, 0,0 );
////////////////////////////////////////////////////////////////////////////////////////////////////////////
    main_4 = lv_btn_create(main_bg);                                           /* 创建主界面设置图标 */
    lv_obj_set_size(main_4, 120, 120);                                         /* 设置按钮大小 */
    lv_obj_align(main_4, LV_ALIGN_CENTER, 640, 0);
    lv_obj_add_event_cb(main_4, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

    lv_obj_t *main_iot_label = lv_label_create(main_4);
    lv_label_set_text(main_iot_label,LV_SYMBOL_HOME );
    lv_obj_set_style_text_font(main_iot_label,&lv_font_montserrat_48, LV_STATE_DEFAULT);
    lv_obj_align(main_iot_label, LV_ALIGN_CENTER, 0,0 );

////////////////////////////////////////////////////////////////////////////////////////////////////////////
    main_5 = lv_btn_create(main_bg);                                           /* 创建主界面设置图标 */
    lv_obj_set_size(main_5, 120, 120);                                         /* 设置按钮大小 */
    lv_obj_align(main_5, LV_ALIGN_CENTER, 800, 0);
    lv_obj_add_event_cb(main_5, main_btn_event, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

    lv_obj_t *main_bell_label = lv_label_create(main_5);
    lv_label_set_text(main_bell_label,LV_SYMBOL_BELL );
    lv_obj_set_style_text_font(main_bell_label,&lv_font_montserrat_48, LV_STATE_DEFAULT);
    lv_obj_align(main_bell_label, LV_ALIGN_CENTER, 0,0 );
////////////////////////////////////////////////////////////////////////////////////////////////////////////
    main_6 = lv_btn_create(main_bg);                                           /* 创建主界面设置图标 */
    lv_obj_set_size(main_6, 120, 120);                                         /* 设置按钮大小 */
    lv_obj_align(main_6, LV_ALIGN_CENTER, 960, 0);

    lv_obj_t *FILE_label = lv_label_create(main_6);
    lv_label_set_text(FILE_label,LV_SYMBOL_DIRECTORY );
    lv_obj_set_style_text_font(FILE_label,&lv_font_montserrat_48, LV_STATE_DEFAULT);
    lv_obj_align(FILE_label, LV_ALIGN_CENTER, 0,0 );

////////////////////////////////////////////////////////////////////////////////////////////////////////////
    lv_obj_add_style(main_0, &main_style, LV_STATE_DEFAULT);
    lv_obj_add_style(main_1, &main_style, LV_STATE_DEFAULT);
    lv_obj_add_style(main_2, &main_style, LV_STATE_DEFAULT);
    lv_obj_add_style(main_3, &main_style, LV_STATE_DEFAULT);
    lv_obj_add_style(main_4, &main_style, LV_STATE_DEFAULT);
    lv_obj_add_style(main_5, &main_style, LV_STATE_DEFAULT);
    lv_obj_add_style(main_6, &main_style, LV_STATE_DEFAULT);

    lv_obj_add_style(note_down_label, &world_style, LV_STATE_DEFAULT);
    lv_obj_add_style(main_text_label, &world_style, LV_STATE_DEFAULT);
    lv_obj_add_style(data_labal, &world_style, LV_STATE_DEFAULT);
    lv_obj_add_style(time_labal, &world_style, LV_STATE_DEFAULT);
}











///******************************************************************************************************/
///*FreeRTOS配置*/

////FreeRTOS资源句柄
//QueueHandle_t uart_rx_queue;  //uart接收中断队列

//QueueHandle_t temp_queue;  //内部温度传感器队列

///* START_TASK 任务 配置
// * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
// */
//#define START_TASK_PRIO     1           /* 任务优先级 */
//#define START_STK_SIZE      128         /* 任务堆栈大小 */
//TaskHandle_t StartTask_Handler;         /* 任务句柄 */
//void start_task(void *pvParameters);    /* 任务函数 */

///* LV_DEMO_TASK 任务 配置
// * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
// */
//#define LV_DEMO_TASK_PRIO   3           /* 任务优先级 */
//#define LV_DEMO_STK_SIZE    4096        /* 任务堆栈大小 */
//TaskHandle_t LV_DEMOTask_Handler;       /* 任务句柄 */
//void lv_demo_task(void *pvParameters);  /* 任务函数 */

///* LED_TASK 任务 配置
// * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
// */
//#define LED_TASK_PRIO       4           /* 任务优先级 */
//#define LED_STK_SIZE        128         /* 任务堆栈大小 */
//TaskHandle_t LEDTask_Handler;           /* 任务句柄 */
//void led_task(void *pvParameters);      /* 任务函数 */

///* ADC3_TEMP_TASK 任务 配置
// * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
// */
////#define LED_TASK_PRIO       4           /* 任务优先级 */
////#define LED_STK_SIZE        128         /* 任务堆栈大小 */
//TaskHandle_t ADC3Task_Handler;           /* 任务句柄 */
//void ADC3_task(void *pvParameters);      /* 任务函数 */
///******************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "lvgl.h"

lv_obj_t *win;

void timer_cb(lv_timer_t * tmr)
{
	static int dir = 0;
	static int y = 0;
	if(dir == 0)
	{	  
		y += 3;
		if(y >= 100) dir = 1;
	}	
	else
	{
	  y -= 3;
		if(y <= 0) dir = 0;
	}
	lv_obj_scroll_to_y(win,y,0);
}

void test_fps()
{
	int i;
	win = lv_obj_create(lv_scr_act());
	lv_obj_set_size(win, LV_PCT(100), LV_PCT(100));	
	lv_obj_set_flex_flow(win, LV_FLEX_FLOW_ROW_WRAP);	
	lv_obj_set_flex_align(win,LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);	 
	
	for(i = 0;i < 100;i++)
	 {
		 lv_obj_t * btn1 = lv_btn_create(win);
		 lv_obj_t * label = lv_label_create(btn1);
		 lv_label_set_text(label, "Hello world");
	 }
	
	lv_timer_create(timer_cb, 20, 0);      // 20ms
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////


void lvgl_demo(void)
{
	uart_rx_queue=xQueueCreate(128,sizeof(char));//创建语音识别消息队列，消息长度128
	temp_queue=xQueueCreate(1,sizeof(int32_t));//创建语音识别消息队列，消息长度128
	
    lv_init();                                          /* lvgl系统初始化 */
    lv_port_disp_init();                                /* lvgl显示接口初始化,放在lv_init()的后面 */
    lv_port_indev_init();                               /* lvgl输入接口初始化,放在lv_init()的后面 */
	



    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传递给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */

    vTaskStartScheduler();                              /* 开启任务调度 */
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */
	
	

    /* 创建LVGL任务 */
    xTaskCreate((TaskFunction_t )lv_demo_task,
                (const char*    )"lv_demo_task",
                (uint16_t       )LV_DEMO_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )2,
                (TaskHandle_t*  )&LV_DEMOTask_Handler);

    /* LED测试任务 */
    xTaskCreate((TaskFunction_t )led_task,
                (const char*    )"led_task",
                (uint16_t       )LED_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )3,
                (TaskHandle_t*  )&LEDTask_Handler);
				
	    /* 内部温度任务 */
    xTaskCreate((TaskFunction_t )ADC3_task,
                (const char*    )"ADC3_task",
                (uint16_t       )256,
                (void*          )NULL,
                (UBaseType_t    )4,
                (TaskHandle_t*  )&ADC3Task_Handler);

    taskEXIT_CRITICAL();            /* 退出临界区 */
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
}

/**
 * @brief       LVGL运行例程
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void lv_demo_task(void *pvParameters)
{

	
    KZ_MAIN();//LVGLUI函数
	uint8_t uart_a[1]={0};
	int32_t temp_a=0;
	BaseType_t K = pdFALSE;
	

	
    while(1)
    {
		K = xQueueReceive(uart_rx_queue,&uart_a,0 );
		if(K==pdTRUE&&main_btn_symbol[1]==1)//判断是否在串口助手界面，在的话才接收串口消息队列
		{

			 lv_textarea_add_text(uart_get_label, (const char *)&uart_a[0]);
		}
		//K = xQueueReceive(temp_queue,&temp_a,0 );
		if(main_btn_symbol[2]==1)
		{
			
			K = xQueueReceive(temp_queue,&temp_a,0 );
			if(K==pdTRUE)
			{
			printf("%d\n\r",temp_a/100);
			}
			 //lv_textarea_add_text(uart_get_label, (const char *)&uart_a[0]);
		}
		tp_dev.scan(0);	            /*触摸扫描*/
        lv_timer_handler();         /* LVGL计时器 */
        vTaskDelay(2);
    }
}



void USART1_IRQHandler(void)
{
    /* 判断是否为接收中断 (RXNE: 接收缓冲区非空) */
    if (__HAL_UART_GET_FLAG(&g_uart1_handle, UART_FLAG_RXNE)) 
    {
		BaseType_t xHigherPriorityTaskWoken;//创建FreeRTOS中断专用切换函数参数
        uint8_t data = (uint8_t)(g_uart1_handle.Instance->RDR & 0xFF); // 读取接收数据寄存器，自动清除 RXNE

		xQueueSendFromISR(uart_rx_queue, &data, &xHigherPriorityTaskWoken);
		if(xHigherPriorityTaskWoken==pdTRUE)
		{
			// 发起任务调度
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			 
		}
        
        LED0_TOGGLE();
        
    }
}

void ADC3_task(void *pvParameters)
{
	int32_t temp;
    while(1)
    {
		temp = adc3_get_temperature();      /* 得到温度值 */
		
		xQueueOverwrite(temp_queue,&temp);//把温度值传给温度消息队列，覆盖写
		//xQueueSend(temp_queue, &temp,portMAX_DELAY);//把温度值传给温度消息队列
		LED0_TOGGLE();
		vTaskDelay(100);
    }
}



void led_task(void *pvParameters)
{
    while(1)
    {
        LED1_TOGGLE();
        vTaskDelay(2000);
    }
}


/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include "../../lvgl.h"
/* 导入lcd驱动头文件 */
#include "./BSP/LCD/lcd.h"
#include "./MALLOC/malloc.h"

/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES     (480)    /* 屏幕宽度 */
#define MY_DISP_VER_RES     (320)    /* 屏幕高度 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
/* 显示设备初始化函数 */
static void disp_init(void);
/* 显示设备刷新函数 */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);



//static lv_color_t buf_2_1[480 * 320]  __attribute__((at(0xC0000000)));;            /*A screen sized buffer*/
//static lv_color_t buf_2_2[480 * 320]  __attribute__((at(0xC0000000+480*320*2)));;            /*Another screen sized buffer*/





	
//static lv_color_t buf_2_1[480 * 160]  __attribute__((at(0x2001E000+2)));;            /*A screen sized buffer*/
//static lv_color_t buf_2_2[480 * 160]  __attribute__((at(0x2001E000+2+32*470*2)));;            /*Another screen sized buffer*/


extern DMA_HandleTypeDef hdma_lcd;

 static lv_disp_drv_t *active_disp_drv = NULL; // 全局变量，存储当前显示驱动指针

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/*
函数名：lv_port_disp_init
功能：初始化并注册显示设备
备注：无
*/
void lv_port_disp_init(void)
{
    /*-------------------------
     * 初始化显示设备
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * 创建一个绘图缓冲区
     *----------------------------*/

    /**
     * LVGL 需要一个缓冲区用来绘制小部件
     * 随后，这个缓冲区的内容会通过显示设备的 `flush_cb`(显示设备刷新函数) 复制到显示设备上
     * 这个缓冲区的大小需要大于显示设备一行的大小
     *
     * 这里有3中缓冲配置:
     * 1. 单缓冲区:
     *      LVGL 会将显示设备的内容绘制到这里，并将他写入显示设备。
     *
     * 2. 双缓冲区:
     *      LVGL 会将显示设备的内容绘制到其中一个缓冲区，并将他写入显示设备。
     *      需要使用 DMA 将要显示在显示设备的内容写入缓冲区。
     *      当数据从第一个缓冲区发送时，它将使 LVGL 能够将屏幕的下一部分绘制到另一个缓冲区。
     *      这样使得渲染和刷新可以并行执行。
     *
     * 3. 全尺寸双缓冲区
     *      设置两个屏幕大小的全尺寸缓冲区，并且设置 disp_drv.full_refresh = 1。
     *      这样，LVGL将始终以 'flush_cb' 的形式提供整个渲染屏幕，您只需更改帧缓冲区的地址。
     */

    /* 单缓冲区示例) */
//    static lv_disp_draw_buf_t draw_buf_dsc_1;
//    static lv_color_t buf_1[MY_DISP_HOR_RES * 320];                          /* 设置缓冲区的大小为屏幕的全尺寸大小 */
//    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 320);    /* 初始化显示缓冲区 */

    /* 双缓冲区示例) */
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 160];                         /* 设置缓冲区的大小为 10 行屏幕的大小 */
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 160];                        /* 设置另一个缓冲区的大小为 10 行屏幕的大小 */
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 160);   /* 初始化显示缓冲区 */

    /* 全尺寸双缓冲区示例) 并且在下面设置 disp_drv.full_refresh = 1 */
//      static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES/4];            /* 设置一个全尺寸的缓冲区 */
//    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /* 设置另一个全尺寸的缓冲区 */
//      static lv_color_t buf_3_1[480 * 320]  __attribute__((at(0xC0000000)));;            /*A screen sized buffer*/
//      static lv_color_t buf_3_2[480 * 320]  __attribute__((at(0xC0000000+480*320*2)));;            /*Another screen sized buffer*/
//      lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, 480*320);   /* 初始化显示缓冲区 */

    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
//    static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[480 * 320];            /*A screen sized buffer*/
//    static lv_color_t buf_3_2[480 * 320];            /*Another screen sized buffer*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, 480 * 320);   /*Initialize the display buffer*/

    /*-----------------------------------
     * 在 LVGL 中注册显示设备
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /* 显示设备的描述符 */
    lv_disp_drv_init(&disp_drv);                    /* 初始化为默认值 */
    /* 建立访问显示设备的函数  */

    /* 设置显示设备的分辨率
     * 这里采用了动态获取的方式，
     * 在实际项目中，通常所使用的屏幕大小是固定的，因此可以直接设置为屏幕的大小 */
    disp_drv.hor_res = lcddev.width;
    disp_drv.ver_res = lcddev.height;

    /* 用来将缓冲区的内容复制到显示设备 */
    disp_drv.flush_cb = disp_flush;

//    /* 设置显示缓冲区 */
//    disp_drv.draw_buf = &draw_buf_dsc_2;
	
	    /* 设置显示缓冲区 */
    disp_drv.draw_buf = &draw_buf_dsc_2;
	
	    /* 设置显示缓冲区 */
//    disp_drv.draw_buf = &draw_buf_dsc_1;

    /* 全尺寸双缓冲区示例)*/
    //disp_drv.full_refresh = 1

    /* 如果您有GPU，请使用颜色填充内存阵列
     * 注意，你可以在 lv_conf.h 中使能 LVGL 内置支持的 GPUs
     * 但如果你有不同的 GPU，那么可以使用这个回调函数。 */
    //disp_drv.gpu_fill_cb = gpu_fill;

    /* 注册显示设备 */
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*
函数名：disp_init
功能：初始化显示设备和必要的外围设备
备注：无
*/
static void disp_init(void)
{
    /*You code here*/
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/

/*
函数名：disp_flush
功能：将内部缓冲区的内容刷新到显示屏上的特定区域
参数：disp_drv    : 显示设备
      area        : 要刷新的区域，包含了填充矩形的对角坐标
      color_p     : 颜色数组
备注：可以使用 DMA 或者任何硬件在后台加速执行这个操作
      但是，需要在刷新完成后调用函数 'lv_disp_flush_ready()'
*/

static uint32_t  size2;
static uint8_t  DMA2_LVGL;
static lv_color_t * color_p1;



static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	
//	 SCB_CleanDCache();  // 清理 D-Cache
//		/* 在指定区域内填充指定颜色块 */
//	  lcd_color_fill(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_p);

//    /* 重要!!!
//     * 通知图形库，已经刷新完毕了 */
//    lv_disp_flush_ready(disp_drv);
//	
////DMA2+单全局缓冲////////////////////////////////////////////////////

    // 获取显示区域的坐标
    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t x2 = area->x2;
    uint16_t y2 = area->y2;

	
    // 计算传输的像素数量
    uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);
	
	lcd_wr_regno(lcddev.setxcmd);
    lcd_wr_data(x1 >> 8);
    lcd_wr_data(x1 & 0XFF);
	lcd_wr_data(x2 >> 8);
    lcd_wr_data(x2 & 0XFF);
	
    lcd_wr_regno(lcddev.setycmd);
    lcd_wr_data(y1 >> 8);
    lcd_wr_data(y1 & 0XFF);
	lcd_wr_data(y2 >> 8);
    lcd_wr_data(y2 & 0XFF);
	
	
	lcd_write_ram_prepare();// 准备写入 GRAM
	
    //保存当前显示驱动指针
    active_disp_drv = disp_drv;

    // 禁用 DMA 以重新配置
    HAL_DMA_Abort(&hdma_lcd);

    if(size>65535)
	{
		    // 清理整个 D-Cache
    SCB_CleanDCache();  // 清理 D-Cache
		// 配置 DMA 传输
		HAL_DMA_Start_IT(&hdma_lcd, (uint32_t)color_p,(uint32_t)&LCD->LCD_RAM,65535);
		size2=size-65535;
		color_p1=color_p+65535;
		DMA2_LVGL=1;
	}
	else
	{
		    // 清理整个 D-Cache
    SCB_CleanDCache();  // 清理 D-Cache
		HAL_DMA_Start_IT(&hdma_lcd, (uint32_t)color_p,(uint32_t)&LCD->LCD_RAM,size);
		DMA2_LVGL=0;
	}
	
	 // 内存屏障，确保数据同步
    __DSB();
////////////////////////////////////////////////////////////////
	
}

//DMA2加速///////////////////////////////////////////////////////
void DMA2_Stream0_IRQHandler(void)
{
    // 检查是否是传输完成中断
    if (__HAL_DMA_GET_FLAG(&hdma_lcd, DMA_FLAG_TCIF0_4) != RESET)
    {
        // 清除传输完成中断标志位
        __HAL_DMA_CLEAR_FLAG(&hdma_lcd, DMA_FLAG_TCIF0_4);

       if(DMA2_LVGL==1)
	   {
			if(size2>65535)
			{		
				    
				SCB_CleanDCache();  // 清理 D-Cache
				 // 禁用 DMA 以重新配置
                HAL_DMA_Abort(&hdma_lcd);
				HAL_DMA_Start_IT(&hdma_lcd, (uint32_t)color_p1,(uint32_t)&LCD->LCD_RAM,65535);// 配置 DMA 传输
				color_p1=color_p1+65535;
				size2=size2-65535;
			}
			else if(size2>0&&size2<65535)
			{
				SCB_CleanDCache();  // 清理 D-Cache
				 // 禁用 DMA 以重新配置
                HAL_DMA_Abort(&hdma_lcd);
				HAL_DMA_Start_IT(&hdma_lcd, (uint32_t)color_p1,(uint32_t)&LCD->LCD_RAM,size2);// 配置 DMA 传输
				size2=0;
			}
			else if(size2==0)
			{
				lv_disp_flush_ready(active_disp_drv);// 通知 LVGL 刷新完成
				
				DMA2_LVGL=0;
			}
		}
	   else
	   {
				lv_disp_flush_ready(active_disp_drv);// 通知 LVGL 刷新完成
		        DMA2_LVGL=0;
	   }
		
    }

}

//DMA2加速///////////////////////////////////////////////////////
//void DMA2_Stream0_IRQHandler(void)
//{
//    // 检查是否是传输完成中断
//    if (__HAL_DMA_GET_FLAG(&hdma_lcd, DMA_FLAG_TCIF0_4) != RESET)
//    {
//        // 清除传输完成中断标志位
//        __HAL_DMA_CLEAR_FLAG(&hdma_lcd, DMA_FLAG_TCIF0_4);

//        // 通知 LVGL 刷新完成
//        lv_disp_flush_ready(active_disp_drv);
//    }
//}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
/*
函数名：gpu_fill
功能：使用 GPU 进行颜色填充
参数：disp_drv    : 显示设备
      dest_buf    : 目标缓冲区
      dest_width  : 目标缓冲区的宽度
      fill_area   : 填充的区域
      color       : 颜色数组
备注：如有 MCU 有硬件加速器 (GPU),那么可以用它来为内存进行颜色填充
*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif

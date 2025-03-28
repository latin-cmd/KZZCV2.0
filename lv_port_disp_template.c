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
/* ����lcd����ͷ�ļ� */
#include "./BSP/LCD/lcd.h"
#include "./MALLOC/malloc.h"

/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES     (480)    /* ��Ļ��� */
#define MY_DISP_VER_RES     (320)    /* ��Ļ�߶� */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
/* ��ʾ�豸��ʼ������ */
static void disp_init(void);
/* ��ʾ�豸ˢ�º��� */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);



//static lv_color_t buf_2_1[480 * 320]  __attribute__((at(0xC0000000)));;            /*A screen sized buffer*/
//static lv_color_t buf_2_2[480 * 320]  __attribute__((at(0xC0000000+480*320*2)));;            /*Another screen sized buffer*/





	
//static lv_color_t buf_2_1[480 * 160]  __attribute__((at(0x2001E000+2)));;            /*A screen sized buffer*/
//static lv_color_t buf_2_2[480 * 160]  __attribute__((at(0x2001E000+2+32*470*2)));;            /*Another screen sized buffer*/


extern DMA_HandleTypeDef hdma_lcd;

 static lv_disp_drv_t *active_disp_drv = NULL; // ȫ�ֱ������洢��ǰ��ʾ����ָ��

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
��������lv_port_disp_init
���ܣ���ʼ����ע����ʾ�豸
��ע����
*/
void lv_port_disp_init(void)
{
    /*-------------------------
     * ��ʼ����ʾ�豸
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * ����һ����ͼ������
     *----------------------------*/

    /**
     * LVGL ��Ҫһ����������������С����
     * �����������������ݻ�ͨ����ʾ�豸�� `flush_cb`(��ʾ�豸ˢ�º���) ���Ƶ���ʾ�豸��
     * ����������Ĵ�С��Ҫ������ʾ�豸һ�еĴ�С
     *
     * ������3�л�������:
     * 1. ��������:
     *      LVGL �Ὣ��ʾ�豸�����ݻ��Ƶ����������д����ʾ�豸��
     *
     * 2. ˫������:
     *      LVGL �Ὣ��ʾ�豸�����ݻ��Ƶ�����һ����������������д����ʾ�豸��
     *      ��Ҫʹ�� DMA ��Ҫ��ʾ����ʾ�豸������д�뻺������
     *      �����ݴӵ�һ������������ʱ������ʹ LVGL �ܹ�����Ļ����һ���ֻ��Ƶ���һ����������
     *      ����ʹ����Ⱦ��ˢ�¿��Բ���ִ�С�
     *
     * 3. ȫ�ߴ�˫������
     *      ����������Ļ��С��ȫ�ߴ绺�������������� disp_drv.full_refresh = 1��
     *      ������LVGL��ʼ���� 'flush_cb' ����ʽ�ṩ������Ⱦ��Ļ����ֻ�����֡�������ĵ�ַ��
     */

    /* ��������ʾ��) */
//    static lv_disp_draw_buf_t draw_buf_dsc_1;
//    static lv_color_t buf_1[MY_DISP_HOR_RES * 320];                          /* ���û������Ĵ�СΪ��Ļ��ȫ�ߴ��С */
//    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 320);    /* ��ʼ����ʾ������ */

    /* ˫������ʾ��) */
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 160];                         /* ���û������Ĵ�СΪ 10 ����Ļ�Ĵ�С */
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 160];                        /* ������һ���������Ĵ�СΪ 10 ����Ļ�Ĵ�С */
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 160);   /* ��ʼ����ʾ������ */

    /* ȫ�ߴ�˫������ʾ��) �������������� disp_drv.full_refresh = 1 */
//      static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES/4];            /* ����һ��ȫ�ߴ�Ļ����� */
//    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /* ������һ��ȫ�ߴ�Ļ����� */
//      static lv_color_t buf_3_1[480 * 320]  __attribute__((at(0xC0000000)));;            /*A screen sized buffer*/
//      static lv_color_t buf_3_2[480 * 320]  __attribute__((at(0xC0000000+480*320*2)));;            /*Another screen sized buffer*/
//      lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, 480*320);   /* ��ʼ����ʾ������ */

    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
//    static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[480 * 320];            /*A screen sized buffer*/
//    static lv_color_t buf_3_2[480 * 320];            /*Another screen sized buffer*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, 480 * 320);   /*Initialize the display buffer*/

    /*-----------------------------------
     * �� LVGL ��ע����ʾ�豸
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /* ��ʾ�豸�������� */
    lv_disp_drv_init(&disp_drv);                    /* ��ʼ��ΪĬ��ֵ */
    /* ����������ʾ�豸�ĺ���  */

    /* ������ʾ�豸�ķֱ���
     * ��������˶�̬��ȡ�ķ�ʽ��
     * ��ʵ����Ŀ�У�ͨ����ʹ�õ���Ļ��С�ǹ̶��ģ���˿���ֱ������Ϊ��Ļ�Ĵ�С */
    disp_drv.hor_res = lcddev.width;
    disp_drv.ver_res = lcddev.height;

    /* �����������������ݸ��Ƶ���ʾ�豸 */
    disp_drv.flush_cb = disp_flush;

//    /* ������ʾ������ */
//    disp_drv.draw_buf = &draw_buf_dsc_2;
	
	    /* ������ʾ������ */
    disp_drv.draw_buf = &draw_buf_dsc_2;
	
	    /* ������ʾ������ */
//    disp_drv.draw_buf = &draw_buf_dsc_1;

    /* ȫ�ߴ�˫������ʾ��)*/
    //disp_drv.full_refresh = 1

    /* �������GPU����ʹ����ɫ����ڴ�����
     * ע�⣬������� lv_conf.h ��ʹ�� LVGL ����֧�ֵ� GPUs
     * ��������в�ͬ�� GPU����ô����ʹ������ص������� */
    //disp_drv.gpu_fill_cb = gpu_fill;

    /* ע����ʾ�豸 */
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*
��������disp_init
���ܣ���ʼ����ʾ�豸�ͱ�Ҫ����Χ�豸
��ע����
*/
static void disp_init(void)
{
    /*You code here*/
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/

/*
��������disp_flush
���ܣ����ڲ�������������ˢ�µ���ʾ���ϵ��ض�����
������disp_drv    : ��ʾ�豸
      area        : Ҫˢ�µ����򣬰����������εĶԽ�����
      color_p     : ��ɫ����
��ע������ʹ�� DMA �����κ�Ӳ���ں�̨����ִ���������
      ���ǣ���Ҫ��ˢ����ɺ���ú��� 'lv_disp_flush_ready()'
*/

static uint32_t  size2;
static uint8_t  DMA2_LVGL;
static lv_color_t * color_p1;



static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	
//	 SCB_CleanDCache();  // ���� D-Cache
//		/* ��ָ�����������ָ����ɫ�� */
//	  lcd_color_fill(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_p);

//    /* ��Ҫ!!!
//     * ֪ͨͼ�ο⣬�Ѿ�ˢ������� */
//    lv_disp_flush_ready(disp_drv);
//	
////DMA2+��ȫ�ֻ���////////////////////////////////////////////////////

    // ��ȡ��ʾ���������
    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t x2 = area->x2;
    uint16_t y2 = area->y2;

	
    // ���㴫�����������
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
	
	
	lcd_write_ram_prepare();// ׼��д�� GRAM
	
    //���浱ǰ��ʾ����ָ��
    active_disp_drv = disp_drv;

    // ���� DMA ����������
    HAL_DMA_Abort(&hdma_lcd);

    if(size>65535)
	{
		    // �������� D-Cache
    SCB_CleanDCache();  // ���� D-Cache
		// ���� DMA ����
		HAL_DMA_Start_IT(&hdma_lcd, (uint32_t)color_p,(uint32_t)&LCD->LCD_RAM,65535);
		size2=size-65535;
		color_p1=color_p+65535;
		DMA2_LVGL=1;
	}
	else
	{
		    // �������� D-Cache
    SCB_CleanDCache();  // ���� D-Cache
		HAL_DMA_Start_IT(&hdma_lcd, (uint32_t)color_p,(uint32_t)&LCD->LCD_RAM,size);
		DMA2_LVGL=0;
	}
	
	 // �ڴ����ϣ�ȷ������ͬ��
    __DSB();
////////////////////////////////////////////////////////////////
	
}

//DMA2����///////////////////////////////////////////////////////
void DMA2_Stream0_IRQHandler(void)
{
    // ����Ƿ��Ǵ�������ж�
    if (__HAL_DMA_GET_FLAG(&hdma_lcd, DMA_FLAG_TCIF0_4) != RESET)
    {
        // �����������жϱ�־λ
        __HAL_DMA_CLEAR_FLAG(&hdma_lcd, DMA_FLAG_TCIF0_4);

       if(DMA2_LVGL==1)
	   {
			if(size2>65535)
			{		
				    
				SCB_CleanDCache();  // ���� D-Cache
				 // ���� DMA ����������
                HAL_DMA_Abort(&hdma_lcd);
				HAL_DMA_Start_IT(&hdma_lcd, (uint32_t)color_p1,(uint32_t)&LCD->LCD_RAM,65535);// ���� DMA ����
				color_p1=color_p1+65535;
				size2=size2-65535;
			}
			else if(size2>0&&size2<65535)
			{
				SCB_CleanDCache();  // ���� D-Cache
				 // ���� DMA ����������
                HAL_DMA_Abort(&hdma_lcd);
				HAL_DMA_Start_IT(&hdma_lcd, (uint32_t)color_p1,(uint32_t)&LCD->LCD_RAM,size2);// ���� DMA ����
				size2=0;
			}
			else if(size2==0)
			{
				lv_disp_flush_ready(active_disp_drv);// ֪ͨ LVGL ˢ�����
				
				DMA2_LVGL=0;
			}
		}
	   else
	   {
				lv_disp_flush_ready(active_disp_drv);// ֪ͨ LVGL ˢ�����
		        DMA2_LVGL=0;
	   }
		
    }

}

//DMA2����///////////////////////////////////////////////////////
//void DMA2_Stream0_IRQHandler(void)
//{
//    // ����Ƿ��Ǵ�������ж�
//    if (__HAL_DMA_GET_FLAG(&hdma_lcd, DMA_FLAG_TCIF0_4) != RESET)
//    {
//        // �����������жϱ�־λ
//        __HAL_DMA_CLEAR_FLAG(&hdma_lcd, DMA_FLAG_TCIF0_4);

//        // ֪ͨ LVGL ˢ�����
//        lv_disp_flush_ready(active_disp_drv);
//    }
//}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
/*
��������gpu_fill
���ܣ�ʹ�� GPU ������ɫ���
������disp_drv    : ��ʾ�豸
      dest_buf    : Ŀ�껺����
      dest_width  : Ŀ�껺�����Ŀ��
      fill_area   : ��������
      color       : ��ɫ����
��ע������ MCU ��Ӳ�������� (GPU),��ô����������Ϊ�ڴ������ɫ���
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

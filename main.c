
 
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LCD/lcd.h"
#include "./USMART/usmart.h"
#include "./BSP/KEY/key.h"
#include "./MALLOC/malloc.h"
#include "./BSP/TIMER/timer.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/ADC/adc3.h"
#include "./BSP/SDRAM/sdram.h"


#include "lvgl.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "lvgl_demo.h"
#include "./BSP/QSPI/qspi.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./FATFS/exfuns/exfuns.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"
#include "usbd_conf.h"



LV_FONT_DECLARE(W1);

//USBD_HandleTypeDef USBD_Device;             /* USB Device处理结构体 */
//extern volatile uint8_t g_usb_state_reg;    /* USB状态 */
//extern volatile uint8_t g_device_state;     /* USB连接 情况 */

int main(void)
{ 
    sys_cache_enable();                     /* 使能L1-Cache */
    HAL_Init();                             /* 初始化HAL库 */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	
    sys_stm32_clock_init(208, 5, 2, 4);     /* 设置时钟, 520Mhz */
    delay_init(520);                        /* 延时初始化 */
    usart_init(115200);                     /* 初始化USART */
    usmart_init(260);                       /* 初始化USMART */
	
//	sys_stm32_clock_init(176, 5, 2, 4);            /* 设置时钟, 400Mhz */
//	delay_init(440);                        /* 延时初始化 */
//    usart_init(115200);                     /* 初始化USART */
//    usmart_init(220);                       /* 初始化USMART */
	
////	sys_stm32_clock_init(192, 5, 2, 4);            /* 设置时钟, 400Mhz */
////	delay_init(480);                        /* 延时初始化 */
////    usart_init(115200);                     /* 初始化USART */
////    usmart_init(240);                       /* 初始化USMART */

	
    led_init();                             /* 初始化LED */
    mpu_memory_protection();                /* 保护相关存储区域 */
	sdram_init();                           /* 初始化SDRAM */
    lcd_init();                             /* 初始化LCD */  
    key_init();                             /* 初始化按键 */
	tp_dev.init();                          /* 触摸屏初始化 */
	
    my_mem_init(SRAMIN);                    /* 初始化内部内存池(AXI) */
    my_mem_init(SRAM12);                    /* 初始化SRAM12内存池(SRAM1+SRAM2) */
    my_mem_init(SRAM4);                     /* 初始化SRAM4内存池(SRAM4) */
    my_mem_init(SRAMDTCM);                  /* 初始化DTCM内存池(DTCM) */
    my_mem_init(SRAMITCM);                  /* 初始化ITCM内存池(ITCM) */
	
	norflash_init();                        /* 初始化SPI FLASH */
	sd_init();								/* 初始化SD卡 */
	adc3_init();                            /* 初始化ADC3(使能内部温度传感器) */

		
	 DMA_Init_LCD();
	  lvgl_demo();                            /* lvgl例程 */
}


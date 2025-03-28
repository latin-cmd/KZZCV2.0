
 
#include "stdlib.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/LCD/lcdfont.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/spi_lcd.h"


/* lcd_ex.c��Ÿ���LCD����IC�ļĴ�����ʼ�����ִ���,�Լ�lcd.c,��.c�ļ�
 * ��ֱ�Ӽ��뵽��������,ֻ��lcd.c���õ�,����ͨ��include����ʽ���.(��Ҫ��
 * �����ļ��ٰ�����.c�ļ�!!����ᱨ��!)
 */
#include "./BSP/LCD/lcd_ex.c"


/* LCD�Ļ�����ɫ�ͱ���ɫ */
uint32_t g_point_color = 0XF800;    /* ������ɫ */
uint32_t g_back_color  = 0XFFFF;    /* ����ɫ */


/* SPI������ʹ�ñ�־
 * 0: ��ʹ��SPI������
 * 1: ʹ��SPI������
 */
uint8_t g_spi_mode = 0;

/* ST7789 8λ8080������ʹ�ñ�־
 * 0: û��ʹ��ST7789 8λ8080������
 * 1: ʹ�õ���ST7789 8λ8080������
 */
uint8_t st7789_bus_8bit = 0;

/* ����LCD��Ҫ���� */
_lcd_dev lcddev;


/**
 * @brief       LCDд����
 * @param       data: Ҫд�������
 * @retval      ��
 */
void lcd_wr_data(volatile uint16_t data)
{
    data = data;            /* ʹ��-O2�Ż���ʱ��,����������ʱ */
  
    if (g_spi_mode == 0)    /* ����ʹ��SPI������ */
    {
        LCD->LCD_RAM = data;
    }
    else
    {
        SPI_DataWrite(data);
    }
}

/**
 * @brief       LCDд�Ĵ������/��ַ����
 * @param       regno: �Ĵ������/��ַ
 * @retval      ��
 */
void lcd_wr_regno(volatile uint16_t regno)
{
    regno = regno;          /* ʹ��-O2�Ż���ʱ��,����������ʱ */
  
    if (g_spi_mode == 0)
    {
        LCD->LCD_REG = regno;    /* д��Ҫд�ļĴ������ */
    }
    else
    {
        SPI_CmdWrite(regno);     /* д��Ҫд�ļĴ������ */ 
    }     }

/**
 * @brief       LCDд�Ĵ���
 * @param       regno:�Ĵ������/��ַ
 * @param       data:Ҫд�������
 * @retval      ��
 */
void lcd_write_reg(uint16_t regno, uint16_t data)
{
    if (g_spi_mode == 0)
    {
        LCD->LCD_REG = regno;    /* д��Ҫд�ļĴ������ */
        LCD->LCD_RAM = data;     /* д������ */
    }
    else
    {
        SPI_CmdWrite(regno);     /* д��Ҫд�ļĴ������ */
        SPI_DataWrite(data);     /* д������ */    
    }
}

/**
 * @brief       LCD������
 * @param       ��
 * @retval      ��ȡ��������
 */
static uint16_t lcd_rd_data(void)
{
    volatile uint16_t ram;  /* ��ֹ���Ż� */
    ram = LCD->LCD_RAM;
    return ram;
}

/**
 * @brief       LCD��ʱ����,�����ڲ�����mdk -O1ʱ���Ż�ʱ��Ҫ���õĵط�
 * @param       i:��ʱ����ֵ
 * @retval      ��
 */
static void lcd_opt_delay(uint32_t i)
{
    while (i--);
}

/**
 * @brief       ׼��дGRAM
 * @param       ��
 * @retval      ��
 */
void lcd_write_ram_prepare(void)
{
    if (g_spi_mode == 0)
    {
        LCD->LCD_REG = lcddev.wramcmd;
    }
    else
    {
        SPI_CmdWrite(lcddev.wramcmd);    
    }  
}

/**
 * @brief       ��ȡĳ�������ɫֵ
 * @param       x,y:����
 * @retval      �˵����ɫ
 */
uint32_t lcd_read_point(uint16_t x, uint16_t y)
{
    uint16_t r = 0, g = 0, b = 0;
    uint16_t color = 0;

    if (x >= lcddev.width || y >= lcddev.height)return 0;   /* �����˷�Χ,ֱ�ӷ��� */

    lcd_set_cursor(x, y);           /* �������� */

    if (g_spi_mode == 0)
    {
        if (lcddev.id == 0X5510)
        {
            lcd_wr_regno(0X2E00);   /* 5510 ���Ͷ�GRAMָ�� */
        }
        else
        {
            lcd_wr_regno(0X2E);     /* ����IC(7796/5310/7789)���Ͷ�GRAMָ�� */
        }

        r = lcd_rd_data();          /* �ٶ�(dummy read) */

        if (st7789_bus_8bit == 1)   /* 8λ8080���ڶ���ɫ���� */
        {
            lcd_opt_delay(2);
            r = lcd_rd_data() & 0XFF;
            lcd_opt_delay(2);
            g = lcd_rd_data() & 0XFF;
            lcd_opt_delay(2);
            b = lcd_rd_data() & 0XFF;   /* ��8λ������Ч */
            
            return (((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
        }
    
        lcd_opt_delay(2);
        r = lcd_rd_data();          /* ʵ��������ɫ */
        
        if (lcddev.id == 0X7796) return r;  /* 7796 һ�ζ�ȡһ������ֵ */

        /* 5310/5510/7789 Ҫ��2�ζ��� */
        lcd_opt_delay(2);
        b = lcd_rd_data();
        g = r & 0XFF;               /* ���� 5310/5510/7789, ��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ */
        g <<= 8;
        
        return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  /* 5310/5510/7789 ��Ҫ��ʽת��һ�� */
    }
    else
    {
        lcd_opt_delay(2);  
        color = lcd_read_ram(0X2E);   /* 7796/7789���Ͷ�GRAMָ�� */
        
        return color;                 /* ���ض�ȡ��������ֵ */    
    }
}

/**
 * @brief       LCD������ʾ
 * @param       ��
 * @retval      ��
 */
void lcd_display_on(void)
{
    if (lcddev.id == 0X5510)    /* 5510������ʾָ�� */
    {
        lcd_wr_regno(0X2900);   /* ������ʾ */
    }
    else                        /* 5310/7789/7796 �ȷ��Ϳ�����ʾָ�� */
    {
        lcd_wr_regno(0X29);     /* ������ʾ */
    }
}

/**
 * @brief       LCD�ر���ʾ
 * @param       ��
 * @retval      ��
 */
void lcd_display_off(void)
{
    if (lcddev.id == 0X5510)    /* 5510�ر���ʾָ�� */
    {
        lcd_wr_regno(0X2800);   /* �ر���ʾ */
    }
    else                        /* 5310/7789/7796 �ȷ��Ϳ�����ʾָ�� */
    {
        lcd_wr_regno(0X28);     /* �ر���ʾ */
    }
}

/**
 * @brief       ���ù��λ��
 * @param       x,y: ����
 * @retval      ��
 */
void lcd_set_cursor(uint16_t x, uint16_t y)
{
    if (lcddev.id == 0X5510)  /* 5510�������� */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(x >> 8);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(x & 0XFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(y & 0XFF);
    }
    else                      /* 5310/7789/7796�������� */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(x >> 8);
        lcd_wr_data(x & 0XFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_data(y & 0XFF);
    }
}

/**
 * @brief       ����LCD���Զ�ɨ�跽��
 * @note
 *              ע��:�����������ܻ��ܵ��˺������õ�Ӱ��,
 *              ����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
 *
 * @param       dir:0~7,����8������(���嶨���lcd.h)
 * @retval      ��
 */
void lcd_scan_dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;

    /* ����ʱ��IC�ı�ɨ�跽������ʱ, IC���ı�ɨ�跽�� */
    if (lcddev.dir == 1)
    {
        switch (dir)   /* ����ת�� */
        {
            case 0:
                dir = 6;
                break;

            case 1:
                dir = 7;
                break;

            case 2:
                dir = 4;
                break;

            case 3:
                dir = 5;
                break;

            case 4:
                dir = 1;
                break;

            case 5:
                dir = 0;
                break;

            case 6:
                dir = 3;
                break;

            case 7:
                dir = 2;
                break;
        }
    }
 
    /* ����ɨ�跽ʽ ���� 0X36/0X3600 �Ĵ��� bit 5,6,7 λ��ֵ */
    switch (dir)
    {
        case L2R_U2D:/* ������,���ϵ��� */
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U:/* ������,���µ��� */
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D:/* ���ҵ���,���ϵ��� */
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U:/* ���ҵ���,���µ��� */
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R:/* ���ϵ���,������ */
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L:/* ���ϵ���,���ҵ��� */
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R:/* ���µ���,������ */
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L:/* ���µ���,���ҵ��� */
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    dirreg = 0X36;  /* �Ծ��󲿷�����IC, ��0X36�Ĵ������� */

    if (lcddev.id == 0X5510)
    {
        dirreg = 0X3600;    /* ����5510, ����������ic�ļĴ����в��� */
    }

    /* 7789(16λ8080����MCU��) & 7796 Ҫ����BGRλ */
    if ((lcddev.id == 0X7789 && st7789_bus_8bit == 0 && g_spi_mode == 0) || lcddev.id == 0X7796)
    {
        regval |= 0X08;
    }

    lcd_write_reg(dirreg, regval);

    if (regval & 0X20)
    {
        if (lcddev.width < lcddev.height)   /* ����X,Y */
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
    else
    {
        if (lcddev.width > lcddev.height)   /* ����X,Y */
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }

    /* ������ʾ����(����)��С */
    if (lcddev.id == 0X5510)
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setxcmd + 2);
        lcd_wr_data((lcddev.width - 1) >> 8);
        lcd_wr_regno(lcddev.setxcmd + 3);
        lcd_wr_data((lcddev.width - 1) & 0XFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setycmd + 2);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_regno(lcddev.setycmd + 3);
        lcd_wr_data((lcddev.height - 1) & 0XFF);
    }
    else
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(0);
        lcd_wr_data(0);
        lcd_wr_data((lcddev.width - 1) >> 8);
        lcd_wr_data((lcddev.width - 1) & 0XFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(0);
        lcd_wr_data(0);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_data((lcddev.height - 1) & 0XFF);
    }
}

/**
 * @brief       ����
 * @param       x,y: ����
 * @param       color: �����ɫ
 * @retval      ��
 */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{    
    lcd_set_cursor(x, y);       /* ���ù��λ�� */
    lcd_write_ram_prepare();    /* ��ʼд��GRAM */
  
    if (g_spi_mode == 0)
    {
        if (st7789_bus_8bit == 1)
        {
            LCD->LCD_RAM = color >> 8;
        }
        
        LCD->LCD_RAM = color;
    }
    else
    {
        SPI_DataWrite_Pixel(color);        
    }
}

/**
 * @brief       ����LCD��ʾ����
 * @param       dir:0,����; 1,����
 * @retval      ��
 */
void lcd_display_dir(uint8_t dir)
{
    lcddev.dir = dir;   /* ����/���� */
    
    if (dir == 0)       /* ���� */
    {
        lcddev.width = 240;
        lcddev.height = 320;

        if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0X2C00;  /* ����д��GRAM��ָ�� */
            lcddev.setxcmd = 0X2A00;  /* ����дX����ָ�� */
            lcddev.setycmd = 0X2B00;  /* ����дY����ָ�� */
            lcddev.width = 480;       /* ���ÿ��480 */
            lcddev.height = 800;      /* ���ø߶�800 */
        }
        else   /* ����IC, ����: 5310/7789/7796��IC */
        {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
        }

        if (lcddev.id == 0X5310 || lcddev.id == 0X7796)    /* �����5310/7796 ���ʾ�� 320*480�ֱ��� */
        {
            lcddev.width = 320;
            lcddev.height = 480;
        }
    }
    else                /* ���� */
    {
        lcddev.width = 320;         /* Ĭ�Ͽ�� */
        lcddev.height = 240;        /* Ĭ�ϸ߶� */

        if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0X2C00;  /* ����д��GRAM��ָ�� */
            lcddev.setxcmd = 0X2A00;  /* ����дX����ָ�� */
            lcddev.setycmd = 0X2B00;  /* ����дY����ָ�� */
            lcddev.width = 800;       /* ���ÿ��800 */
            lcddev.height = 480;      /* ���ø߶�480 */
        }
        else   /* ����IC, ����: 5310/7789/7796��IC */
        {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
        }

        if (lcddev.id == 0X5310 || lcddev.id == 0X7796)    /* �����5310/7796 ���ʾ�� 320*480�ֱ��� */
        {
            lcddev.width = 480;
            lcddev.height = 320;
        }
    }

    lcd_scan_dir(DFT_SCAN_DIR);     /* Ĭ��ɨ�跽�� */
}

/**
 * @brief       ���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
 * @param       sx,sy:������ʼ����(���Ͻ�)
 * @param       width,height:���ڿ�Ⱥ͸߶�,�������0!!
 * @note        �����С:width*height.
 *
 * @retval      ��
 */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;

    if (lcddev.id == 0X5510)     /* 5510���ô��� */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(sx & 0XFF);
        lcd_wr_regno(lcddev.setxcmd + 2);
        lcd_wr_data(twidth >> 8);
        lcd_wr_regno(lcddev.setxcmd + 3);
        lcd_wr_data(twidth & 0XFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(sy & 0XFF);
        lcd_wr_regno(lcddev.setycmd + 2);
        lcd_wr_data(theight >> 8);
        lcd_wr_regno(lcddev.setycmd + 3);
        lcd_wr_data(theight & 0XFF);
    }
    else    /* 5310/7789/7796���ô��� */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_data(sx & 0XFF);
        lcd_wr_data(twidth >> 8);
        lcd_wr_data(twidth & 0XFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_data(sy & 0XFF);
        lcd_wr_data(theight >> 8);
        lcd_wr_data(theight & 0XFF);
    }
}

/**
 * @brief       ��ʼ��LCD
 * @note        �ó�ʼ���������Գ�ʼ�������ͺŵ�LCD(�����.c�ļ���ǰ�������)
 *
 * @param       ��
 * @retval      ��
 */
void lcd_init(void)
{
    LCD_CS_GPIO_CLK_ENABLE();   /* LCD_CS��ʱ��ʹ�� */
    LCD_WR_GPIO_CLK_ENABLE();   /* LCD_WR��ʱ��ʹ�� */
    LCD_RD_GPIO_CLK_ENABLE();   /* LCD_RD��ʱ��ʹ�� */
    LCD_RS_GPIO_CLK_ENABLE();   /* LCD_RS��ʱ��ʹ�� */
    LCD_BL_GPIO_CLK_ENABLE();   /* LCD_BL��ʱ��ʹ�� */
    LCD_RST_GPIO_CLK_ENABLE();  /* LCD_RST��ʱ��ʹ�� */

    RCC->AHB4ENR |= 3 << 3;     /* ʹ��PD,PE */
    RCC->AHB3ENR |= 1 << 12;    /* ʹ��FMCʱ�� */

    sys_gpio_set(LCD_CS_GPIO_PORT, LCD_CS_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* LCD_CS����ģʽ���� */

    sys_gpio_set(LCD_WR_GPIO_PORT, LCD_WR_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* LCD_WR����ģʽ���� */

    sys_gpio_set(LCD_RD_GPIO_PORT, LCD_RD_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* LCD_RD����ģʽ���� */

    sys_gpio_set(LCD_RS_GPIO_PORT, LCD_RS_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* LCD_RS����ģʽ���� */

    sys_gpio_set(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);  /* LCD_BL����ģʽ����(�������) */

    sys_gpio_set(LCD_RST_GPIO_PORT, LCD_RST_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);  /* LCD_RST����ģʽ����(�������) */
                 
    sys_gpio_af_set(LCD_CS_GPIO_PORT, LCD_CS_GPIO_PIN, 12); /* LCD_CS��, AF12 */
    sys_gpio_af_set(LCD_WR_GPIO_PORT, LCD_WR_GPIO_PIN, 12); /* LCD_WR��, AF12 */
    sys_gpio_af_set(LCD_RD_GPIO_PORT, LCD_RD_GPIO_PIN, 12); /* LCD_RD��, AF12 */
    sys_gpio_af_set(LCD_RS_GPIO_PORT, LCD_RS_GPIO_PIN, 12); /* LCD_RS��, AF12 */

    /* LCD_D0~LCD_D15 IO�ڳ�ʼ�� */
    sys_gpio_set(GPIOD, (3 << 0) | (7 << 8) | (3 << 14), SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* PD0,1,8,9,10,14,15   AF OUT */
    sys_gpio_set(GPIOE, (0X1FF << 7), SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);                      /* PE7~15  AF OUT */

    sys_gpio_af_set(GPIOD, (3 << 0) | (7 << 8) | (3 << 14), 12);/* PD0,1,8,9,10,14,15   AF12 */
    sys_gpio_af_set(GPIOE, (0X1FF << 7), 12);                   /* PE7~15  AF12 */

    /* fmc_ker_ck����pll2_r_ck = 220Mhz
     * �Ĵ�������
     * bank1��NE1~4,ÿһ����һ��BCR+TCR�������ܹ��˸��Ĵ�����
     * ��������ʹ��NE4 ��Ҳ�Ͷ�ӦBTCR[6], [7]
     */
    LCD_FMC_BCRX = 0X00000000;  /* BCR�Ĵ������� */
    LCD_FMC_BTRX = 0X00000000;  /* BTR�Ĵ������� */
    LCD_FMC_BWTRX = 0X00000000; /* BWTR�Ĵ������� */

    /* ����BCR�Ĵ��� ʹ���첽ģʽ */
    LCD_FMC_BCRX |= 1 << 12;    /* �洢��дʹ�� */
    LCD_FMC_BCRX |= 1 << 14;    /* ��дʹ�ò�ͬ��ʱ�� */
    LCD_FMC_BCRX |= 1 << 4;     /* �洢�����ݿ��Ϊ16bit */

    /* ����BTR�Ĵ���, ��ʱ����ƼĴ��� */
    LCD_FMC_BTRX |= 0 << 28;    /* ģʽA */
    LCD_FMC_BTRX |= 15 << 0;    /* ��ַ����ʱ��(ADDSET)Ϊ15��fmc_ker_ck 1/220M = 4.5ns * 15 = 67.5ns */

    /* ��ΪҺ������IC�Ķ����ݵ�ʱ���ٶȲ���̫��,�����Ǹ�������оƬ */
    LCD_FMC_BTRX |= 78 << 8;    /* ���ݱ���ʱ��(DATAST)Ϊ78��fmc_ker_ck = 4.5ns * 78 = 351ns */

    /* дʱ����ƼĴ��� */
    LCD_FMC_BWTRX |= 0 << 28;   /* ģʽA */
    LCD_FMC_BWTRX |= 15 << 0;   /* ��ַ����ʱ��(ADDSET)Ϊ15��fmc_ker_ck = 4.5ns * 15 = 67.5n */

    /* ĳЩҺ������IC��д�ź���������Ҳ��50ns�� */
    LCD_FMC_BWTRX |= 15 << 8;   /* ���ݱ���ʱ��(DATAST)Ϊ15��fmc_ker_ck = 4.5ns * 15 = 67.5n */

    /* ʹ��BANK,����x */
    LCD_FMC_BCRX |= 1 << 0;     /* ʹ��BANK������x */
    LCD_FMC_BCRX |= (uint32_t)1 << 31;  /* ʹ��FMC */

    lcd_opt_delay(0XFFFFF);     /* ��ʼ��FMC��,����ȴ�һ��ʱ����ܿ�ʼ��ʼ�� */

    /* LCD��λ */
	  LCD_RST(1);
	  delay_ms(10);
	  LCD_RST(0);
	  delay_ms(50);
	  LCD_RST(1); 
		delay_ms(200); 

    /* ����7796 ID�Ķ�ȡ */
    lcd_wr_regno(0XD3);
    lcddev.id = lcd_rd_data();  /* dummy read */
    lcddev.id = lcd_rd_data();  /* ����0X00 */
    lcddev.id = lcd_rd_data();  /* ��ȡ0X77 */
    lcddev.id <<= 8;
    lcddev.id |= lcd_rd_data(); /* ��ȡ0X96 */

    if (lcddev.id != 0X7796)    /* ����7796,���Կ����ǲ���ST7789 */
    {
        lcd_wr_regno(0X04);
        lcddev.id = lcd_rd_data();      /* dummy read */
        lcddev.id = lcd_rd_data();      /* ����0X85 */
        lcddev.id = lcd_rd_data();      /* ��ȡ0X85 */
        lcddev.id <<= 8;
        lcddev.id |= lcd_rd_data();     /* ��ȡ0X52 */
        
        if (lcddev.id == 0X8552)        /* ��8552��IDת����7789 */
        {
            lcddev.id = 0x7789;
        }
        else                            /* �����ǲ���ST7789 8λ8080���� */
        {
            lcd_wr_regno(0X04);
            lcddev.id = lcd_rd_data();  /* dummy read */
            lcddev.id = lcd_rd_data();  /* ����0X85 */
            lcddev.id = lcd_rd_data();  /* ��ȡ0X85 */
            lcddev.id <<= 8;
            lcddev.id |= lcd_rd_data() & 0XFF;     /* ��ȡ0X52 */
           
            if (lcddev.id == 0X8552)    /* ��8552��IDת����7789 */
            {
                lcddev.id = 0x7789;
                st7789_bus_8bit = 1;    /* ʹ��ST7789 8λ8080������ */      
            }          
        }
        
        if (lcddev.id != 0x7789)        /* Ҳ����ST7789,�����ǲ���NT35310 */
        {
            lcd_wr_regno(0XD4);
            lcddev.id = lcd_rd_data();  /* dummy read */
            lcddev.id = lcd_rd_data();  /* ����0X01 */
            lcddev.id = lcd_rd_data();  /* ����0X53 */
            lcddev.id <<= 8;
            lcddev.id |= lcd_rd_data(); /* �������0X10 */

            if (lcddev.id != 0X5310)    /* Ҳ����NT35310,���Կ����ǲ���NT35510 */
            {
                /* ������Կ�������ṩ,�հἴ�ɣ� */
                lcd_write_reg(0xF000, 0x0055);
                lcd_write_reg(0xF001, 0x00AA);
                lcd_write_reg(0xF002, 0x0052);
                lcd_write_reg(0xF003, 0x0008);
                lcd_write_reg(0xF004, 0x0001);
                
                lcd_wr_regno(0xC500);           /* ��ȡID��8λ */
                lcddev.id = lcd_rd_data();      /* ����0X55 */
                lcddev.id <<= 8;

                lcd_wr_regno(0xC501);           /* ��ȡID��8λ */
                lcddev.id |= lcd_rd_data();     /* ����0X10 */               
            }
        }

        if ((lcddev.id != 0X7789) && (lcddev.id != 0x7796) && (lcddev.id != 0x5310) && (lcddev.id != 0x5510))
        {
            g_spi_mode = 1;                     /* ʹ��SPI������ */

            spi_lcd_init();                     /* SPI LCD�ӿڳ�ʼ�� */    

            /* ����7796 ID�Ķ�ȡ */
            lcddev.id = lcd_read_id(0XD3);      /* ��ȡID */   

            if (lcddev.id != 0X7796)            /* ����7796,���Կ����ǲ���ST7789 */
            {
                dummy_clock();                
                lcddev.id = lcd_read_id(0X04);  
               
                if (lcddev.id == 0X8552)        /* ��8552��IDת����7789 */
                {
                    lcddev.id = 0x7789;
                }
            }   
            
            printf("SPI LCD MODE\r\n");            
        }                
    }

    /* �ر�ע��, �����main�����������δ���1��ʼ��, ��Ῠ����printf
     * ����(������f_putc����), ����, �����ʼ������1, �������ε�����
     * ���� printf ��� !!!!!!!
     */
    printf("LCD ID:%x\r\n", lcddev.id); /* ��ӡLCD ID */

    if (lcddev.id == 0X7789)
    {
        lcd_ex_st7789_reginit();    /* ִ��ST7789��ʼ�� */
    }
    else if (lcddev.id == 0x5310)
    {
        lcd_ex_nt35310_reginit();   /* ִ��NT35310��ʼ�� */
    }
    else if (lcddev.id == 0x7796)
    {
        lcd_ex_st7796_reginit();    /* ִ��ST7796��ʼ�� */
    }
    else if (lcddev.id == 0x5510)
    {
        lcd_ex_nt35510_reginit();   /* ִ��NT35510��ʼ�� */
    }

    /* ���ڲ�ͬ��Ļ��дʱ��ͬ�������ʱ����Ը����Լ�����Ļ�����޸�
      �������ϳ����߶�ʱ��Ҳ����Ӱ�죬��Ҫ�Լ���������޸ģ� */
    /* MCU����ʼ������Ժ�,���� */
    if (g_spi_mode == 0) 
    {
        if (lcddev.id == 0X7789)  
        {
            /* ��������дʱ����ƼĴ�����ʱ�� */
            LCD_FMC_BWTRX &= ~(0XF << 0);   /* ��ַ����ʱ��(ADDSET)���� */
            LCD_FMC_BWTRX &= ~(0XF << 8);   /* ���ݱ���ʱ������ */
            LCD_FMC_BWTRX |= 5 << 0;        /* ��ַ����ʱ��(ADDSET)Ϊ5��fmc_ker_ck = 22.5 ns */
            LCD_FMC_BWTRX |= 5 << 8;        /* ���ݱ���ʱ��(DATAST)Ϊ5��fmc_ker_ck = 22.5 ns */
        }
        else if (lcddev.id == 0X5510)
        {
            /* ��������дʱ����ƼĴ�����ʱ�� */
            LCD_FMC_BWTRX &= ~(0XF << 0);   /* ��ַ����ʱ��(ADDSET)���� */
            LCD_FMC_BWTRX &= ~(0XF << 8);   /* ���ݱ���ʱ������ */
            LCD_FMC_BWTRX |= 3 << 0;        /* ��ַ����ʱ��(ADDSET)Ϊ3��fmc_ker_ck = 13.5 ns */
            LCD_FMC_BWTRX |= 3 << 8;        /* ���ݱ���ʱ��(DATAST)Ϊ3��fmc_ker_ck = 13.5 ns */
        }
        else if (lcddev.id == 0X5310 || lcddev.id == 0X7796)
        {
            /* ��������дʱ����ƼĴ�����ʱ�� */
            LCD_FMC_BWTRX &= ~(0XF << 0);   /* ��ַ����ʱ��(ADDSET)���� */
            LCD_FMC_BWTRX &= ~(0XF << 8);   /* ���ݱ���ʱ������ */
            LCD_FMC_BWTRX |= 3 << 0;        /* ��ַ����ʱ��(ADDSET)Ϊ3��fmc_ker_ck = 13.5 ns */
            LCD_FMC_BWTRX |= 3 << 8;        /* ���ݱ���ʱ��(DATAST)Ϊ3��fmc_ker_ck = 13.5 ns */
        }
    }
    
    lcd_display_dir(LCD_DIR);    /* ��Ļ���÷��� */
    
    if (g_spi_mode == 0)   
    {      
        LCD_BL(1);               /* �������� */
    }
    else
    {
        SPI_LCD_BL(1);           /* SPI�������������� */
    }    
    
    lcd_clear(WHITE);            /* ���� */
}

/**
 * @brief       ��������
 * @param       color: Ҫ��������ɫ
 * @retval      ��
 */
void lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = lcddev.width;

    totalpoint *= lcddev.height;    /* �õ��ܵ��� */
    lcd_set_cursor(0x00, 0x0000);   /* ���ù��λ�� */
    lcd_write_ram_prepare();        /* ��ʼд��GRAM */

    if (g_spi_mode == 0)
    {
        for (index = 0; index < totalpoint; index++)
        {
            if (st7789_bus_8bit == 1)
            {
                LCD->LCD_RAM = color >> 8;
            }
            
            LCD->LCD_RAM = color;
        }
    }
    else
    {
        for (index = 0; index < totalpoint; index++)
        {
            SPI_DataWrite_Pixel(color);
        }        
    }
}

/**
 * @brief       ��ָ����������䵥����ɫ
 * @param       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * @param       color: Ҫ������ɫ
 * @retval      ��
 */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t i, j;
    uint16_t xlen = 0;
    
    xlen = ex - sx + 1;
  
    for (i = sy; i <= ey; i++)
    {
        lcd_set_cursor(sx, i);      /* ���ù��λ�� */
        lcd_write_ram_prepare();    /* ��ʼд��GRAM */

        if (g_spi_mode == 0)
        {
            for (j = 0; j < xlen; j++)
            {
                if (st7789_bus_8bit == 1)
                {
                    LCD->LCD_RAM = color >> 8;  /* ��ʾ��ɫ */
                }
                
                LCD->LCD_RAM = color;           /* ��ʾ��ɫ */
            }
        }
        else
        {
            for (j = 0; j < xlen; j++)
            {
                SPI_DataWrite_Pixel(color);     /* ��ʾ��ɫ */
            }            
        }
    }
}

/**
 * @brief       ��ָ�����������ָ����ɫ��
 * @param       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * @param       color: Ҫ������ɫ�����׵�ַ
 * @retval      ��
 */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height, width;
    uint16_t i, j;    
    
    width = ex - sx + 1;            /* �õ����Ŀ�� */
    height = ey - sy + 1;           /* �߶� */

    for (i = 0; i < height; i++)
    {
        lcd_set_cursor(sx, sy + i); /* ���ù��λ�� */
        lcd_write_ram_prepare();    /* ��ʼд��GRAM */

        if (g_spi_mode == 0)
        {
            for (j = 0; j < width; j++)
            {
                if (st7789_bus_8bit == 1)
                {
                    LCD->LCD_RAM = color[i * width + j] >> 8;  /* д������ */
                }
                
                LCD->LCD_RAM = color[i * width + j];           /* д������ */
            }
        }
        else
        {
            for (j = 0; j < width; j++)
            {
                SPI_DataWrite_Pixel(color[i * width + j]);     /* д������ */
            }           
        }
    }
}

/**
 * @brief       ����
 * @param       x1,y1: �������
 * @param       x2,y2: �յ�����
 * @param       color: �ߵ���ɫ
 * @retval      ��
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;          /* ������������ */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)incx = 1;       /* ���õ������� */
    else if (delta_x == 0)incx = 0; /* ��ֱ�� */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; /* ˮƽ�� */
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x;  /* ѡȡ�������������� */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )   /* ������� */
    {
        lcd_draw_point(row, col, color);   /* ���� */
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief       ��ˮƽ��
 * @param       x,y  : �������
 * @param       len  : �߳���
 * @param       color: ���ε���ɫ
 * @retval      ��
 */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > lcddev.width) || (y > lcddev.height))return;

    lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief       ������
 * @param       x1,y1: �������
 * @param       x2,y2: �յ�����
 * @param       color: ���ε���ɫ
 * @retval      ��
 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * @brief       ��Բ
 * @param       x0,y0: Բ��������
 * @param       r    : �뾶
 * @param       color: Բ����ɫ
 * @retval      ��
 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       /* �ж��¸���λ�õı�־ */

    while (a <= b)
    {
        lcd_draw_point(x0 + a, y0 - b, color);  /* 5 */
        lcd_draw_point(x0 + b, y0 - a, color);  /* 0 */
        lcd_draw_point(x0 + b, y0 + a, color);  /* 4 */
        lcd_draw_point(x0 + a, y0 + b, color);  /* 6 */
        lcd_draw_point(x0 - a, y0 + b, color);  /* 1 */
        lcd_draw_point(x0 - b, y0 + a, color);
        lcd_draw_point(x0 - a, y0 - b, color);  /* 2 */
        lcd_draw_point(x0 - b, y0 - a, color);  /* 7 */
        a++;

        /* ʹ��Bresenham�㷨��Բ */
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * @brief       ���ʵ��Բ
 * @param       x,y  : Բ��������
 * @param       r    : �뾶
 * @param       color: Բ����ɫ
 * @retval      ��
 */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            /* draw lines from outside */
            if (xr > imax)
            {
                lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }

            xr--;
        }

        /* draw lines from inside (center) */
        lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

/**
 * @brief       ��ָ��λ����ʾһ���ַ�
 * @param       x,y   : ����
 * @param       chr   : Ҫ��ʾ���ַ�:' '--->'~'
 * @param       size  : �����С 12/16/24/32
 * @param       mode  : ���ӷ�ʽ(1); �ǵ��ӷ�ʽ(0);
 * @param       color : �ַ�����ɫ;
 * @retval      ��
 */
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */
    chr = chr - ' ';    /* �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩ */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];  /* ����1206���� */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];  /* ����1608���� */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];  /* ����2412���� */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];  /* ����3216���� */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* ��ȡ�ַ��ĵ������� */

        for (t1 = 0; t1 < 8; t1++)  /* һ���ֽ�8���� */
        {
            if (temp & 0x80)        /* ��Ч��,��Ҫ��ʾ */
            {
                lcd_draw_point(x, y, color);        /* �������,Ҫ��ʾ����� */
            }
            else if (mode == 0)     /* ��Ч�㲢��ѡ��ǵ��ӷ�ʽ */
            {
                lcd_draw_point(x, y, g_back_color); /* ������ɫ,�൱������㲻��ʾ(ע�ⱳ��ɫ��ȫ�ֱ�������) */
            }

            temp <<= 1; /* ��λ, �Ա��ȡ��һ��λ��״̬ */
            y++;

            if (y >= lcddev.height)return;      /* �������� */

            if ((y - y0) == size)               /* ��ʾ��һ����? */
            {
                y = y0;                         /* y���긴λ */
                x++;                            /* x������� */

                if (x >= lcddev.width)return;   /* x���곬������ */

                break;
            }
        }
    }
}

/**
 * @brief       ƽ������, m^n
 * @param       m: ����
 * @param       n: ָ��
 * @retval      m��n�η�
 */
static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}

/**
 * @brief       ��ʾlen������(��λΪ0����ʾ)
 * @param       x,y   : ��ʼ����
 * @param       num   : ��ֵ(0 ~ 2^32)
 * @param       len   : ��ʾ���ֵ�λ��
 * @param       size  : ѡ������ 12/16/24/32
 * @param       color : ���ֵ���ɫ;
 * @retval      ��
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* ������ʾλ��ѭ�� */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;   /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))   /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2) * t, y, ' ', size, 0, color); /* ��ʾ�ո�,ռλ */
                continue;   /* �����¸�һλ */
            }
            else
            {
                enshow = 1; /* ʹ����ʾ */
            }
        }

        lcd_show_char(x + (size / 2) * t, y, temp + '0', size, 0, color);  /* ��ʾ�ַ� */
    }
}

/**
 * @brief       ��չ��ʾlen������(��λ��0Ҳ��ʾ)
 * @param       x,y   : ��ʼ����
 * @param       num   : ��ֵ(0 ~ 2^32)
 * @param       len   : ��ʾ���ֵ�λ��
 * @param       size  : ѡ������ 12/16/24/32
 * @param       mode  : ��ʾģʽ
 *              [7]:0,�����;1,���0.
 *              [6:1]:����
 *              [0]:0,�ǵ�����ʾ;1,������ʾ.
 * @param       color : ���ֵ���ɫ;
 * @retval      ��
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* ������ʾλ��ѭ�� */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;    /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))   /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                if (mode & 0X80)   /* ��λ��Ҫ���0 */
                {
                    lcd_show_char(x + (size / 2) * t, y, '0', size, mode & 0X01, color);  /* ��0ռλ */
                }
                else
                {
                    lcd_show_char(x + (size / 2) * t, y, ' ', size, mode & 0X01, color);  /* �ÿո�ռλ */
                }

                continue;
            }
            else
            {
                enshow = 1; /* ʹ����ʾ */
            }
        }

        lcd_show_char(x + (size / 2) * t, y, temp + '0', size, mode & 0X01, color);
    }
}

/**
 * @brief       ��ʾ�ַ���
 * @param       x,y         : ��ʼ����
 * @param       width,height: �����С
 * @param       size        : ѡ������ 12/16/24/32
 * @param       p           : �ַ����׵�ַ
 * @param       color       : �ַ�������ɫ
 * @retval      ��
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* �ж��ǲ��ǷǷ��ַ�! */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break;  /* �˳� */

        lcd_show_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}

 // ���� DMA ���
DMA_HandleTypeDef hdma_lcd;

void DMA_Init_LCD(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE();  // ʹ�� DMA2 ʱ��


    // ���� DMA ����
    hdma_lcd.Instance                 = DMA2_Stream0;
	hdma_lcd.Init.Request             = DMA_REQUEST_MEM2MEM;    // �洢�����洢��ģʽ
    hdma_lcd.Init.Direction           = DMA_MEMORY_TO_MEMORY;       // �ڴ浽�ڴ洫��
    hdma_lcd.Init.PeriphInc           = DMA_PINC_ENABLE;            // �����ַ��ͼ�λ�����������
    hdma_lcd.Init.MemInc              = DMA_MINC_DISABLE;           // LCD ��ַ������
    hdma_lcd.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    // �������ݿ�ȣ�16 λ
    hdma_lcd.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;    // �ڴ����ݿ�ȣ�16 λ
    hdma_lcd.Init.Mode                = DMA_NORMAL;                 // ��ͨģʽ
    hdma_lcd.Init.Priority            = DMA_PRIORITY_HIGH;          // �����ȼ�
	
//    hdma_lcd.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;        // �ر� FIFO
//    hdma_lcd.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;    // FIFO ��ֵ���ã���Ч��
//    hdma_lcd.Init.MemBurst            = DMA_MBURST_SINGLE;          // ���δ���
//    hdma_lcd.Init.PeriphBurst         = DMA_PBURST_SINGLE;          // ���δ���
	
	
	hdma_lcd.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;       // ���� FIFO
    hdma_lcd.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;   // FIFO ��ֵ����Ϊ��
    hdma_lcd.Init.MemBurst            = DMA_MBURST_INC8;           // �ڴ�ͻ�����䣺8������
    hdma_lcd.Init.PeriphBurst         = DMA_PBURST_SINGLE;         // ����ͻ�����䣺8�����ĵ���

    //DMAȱʡ����
    HAL_DMA_DeInit(&hdma_lcd);   

    // ��ʼ�� DMA
    HAL_DMA_Init(&hdma_lcd);

    // ֹͣ DMA �Խ��к�������
     HAL_DMA_Abort(&hdma_lcd);

    // ���� DMA ��������ж�
    __HAL_DMA_ENABLE_IT(&hdma_lcd, DMA_IT_TC);

    // ʹ�� NVIC �ж�
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 8, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}









#include "usb_middleware.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_benchmark.h"
#include "FreeRTOS.h"
#include "task.h"

/* USB相关 */
static USBD_HandleTypeDef hUsbDeviceFS;
static uint8_t usb_connected;

/* 函数实现 */
void usb_init(void)
{
    // 初始化USB硬件
    USB_HardwareInit();

    // 初始化USB设备
    USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
    USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
    USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);
    USBD_Start(&hUsbDeviceFS);
}

void usb_deinit(void)
{
    // 反初始化USB设备
    USBD_Stop(&hUsbDeviceFS);
    USBD_DeInit(&hUsbDeviceFS);
}

uint8_t usb_is_connected(void)
{
    // 检查USB连接状态
    return usb_connected;
}

void usb_send_data(uint8_t *data, uint16_t len)
{
    // 发送USB数据
    if (usb_connected)
    {
        USBD_CDC_SetTxBuffer(&hUsbDeviceFS, data, len);
        USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    }
}

void usb_receive_data(uint8_t *data, uint16_t len)
{
    // 接收USB数据
    if (usb_connected)
    {
        USBD_CDC_SetRxBuffer(&hUsbDeviceFS, data);
        USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    }
}

void USB_HardwareInit(void)
{
    // 初始化USB硬件
    // 这里需要根据具体的USB型号和硬件连接来实现
    // 包括GPIO初始化、USB配置等
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    // USB连接回调
    usb_connected = 1;
}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    // USB断开回调
    usb_connected = 0;
} 
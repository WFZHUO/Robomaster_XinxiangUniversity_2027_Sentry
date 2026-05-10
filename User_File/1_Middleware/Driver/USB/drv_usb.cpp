/**
 * @file drv_usb.cpp
 * @author WangFonzhuo
 * @brief USB通用接口
 * @version 1.0
 * @date 2026-05-09 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "drv_usb.h"
#include "usbd_cdc_if.h"
#include "sys_timestamp.h"
#include <string.h>

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// USB管理对象
Struct_USB_Manage_Object USB_Manage_Object;

// USB接收缓冲区
uint8_t USB_Rx_Buffer_0[USB_BUFFER_SIZE];
uint8_t USB_Rx_Buffer_1[USB_BUFFER_SIZE];

// USB设备句柄
extern USBD_HandleTypeDef hUsbDeviceHS;

// 全局初始化完成标志位
extern bool init_finished;

/* Function prototypes -------------------------------------------------------*/

static void USB_Reinit();

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 初始化USB
 *
 * @param Callback_Function 回调函数
 */
void USB_Init(USB_Callback Callback_Function)
{
    USB_Manage_Object.Callback_Function = Callback_Function;

    USB_Manage_Object.Rx_Buffer_0 = USB_Rx_Buffer_0;
    USB_Manage_Object.Rx_Buffer_1 = USB_Rx_Buffer_1;

    USB_Manage_Object.Rx_Buffer_Active = USB_Manage_Object.Rx_Buffer_0;

    USB_Reinit();
}

/**
 * @brief USB重新初始化接收
 */
static void USB_Reinit()
{
    if (hUsbDeviceHS.pClassData == nullptr)
    {
        return;
    }

    USBD_CDC_SetRxBuffer(&hUsbDeviceHS, USB_Manage_Object.Rx_Buffer_Active);
    USBD_CDC_ReceivePacket(&hUsbDeviceHS);
}

/**
 * @brief USB发送数据
 *
 * @param Data 被发送的数据指针
 * @param Length 数据长度
 * @return 发送状态
 */
uint8_t USB_Transmit_Data(uint8_t *Data, uint16_t Length)
{
    if (Data == nullptr || Length == 0)
    {
        return USBD_FAIL;
    }

    if (hUsbDeviceHS.pClassData == nullptr)
    {
        return USBD_FAIL;
    }

    return CDC_Transmit_HS(Data, Length);
}

/**
 * @brief USB接收回调转接函数
 *
 * @param Buffer 接收数据指针
 * @param Length 接收数据长度
 */
void USB_ReceiveCallback(uint8_t *Buffer, uint32_t Length)
{
    if (init_finished == false)
    {
        USB_Reinit();
        return;
    }

    if (Length > USB_BUFFER_SIZE)
    {
        Length = USB_BUFFER_SIZE;
    }

    // 自设双缓冲USB
    if (Buffer != USB_Manage_Object.Rx_Buffer_Active)
    {
        memcpy(USB_Manage_Object.Rx_Buffer_Active, Buffer, Length);
    }

    USB_Manage_Object.Rx_Buffer_Ready = USB_Manage_Object.Rx_Buffer_Active;

    if (USB_Manage_Object.Rx_Buffer_Active == USB_Manage_Object.Rx_Buffer_0)
    {
        USB_Manage_Object.Rx_Buffer_Active = USB_Manage_Object.Rx_Buffer_1;
    }
    else
    {
        USB_Manage_Object.Rx_Buffer_Active = USB_Manage_Object.Rx_Buffer_0;
    }

    USB_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

    USB_Reinit();

    if (USB_Manage_Object.Callback_Function != nullptr)
    {
        USB_Manage_Object.Callback_Function(USB_Manage_Object.Rx_Buffer_Ready, Length);
    }
}

/*----------------------------------------------------------------------------*/
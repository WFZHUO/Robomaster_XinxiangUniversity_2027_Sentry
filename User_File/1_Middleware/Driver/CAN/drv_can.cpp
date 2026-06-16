/**
 * @file drv_can.cpp
 * @author WangFonzhuo
 * @brief FDCAN通信初始化与配置流程
 * @version 1.0
 * @date 2026-05-20 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "drv_can.h"
#include "sys_timestamp.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// 全局初始化完成标志位
extern bool init_finished;

Struct_CAN_Manage_Object CAN1_Manage_Object = {nullptr};
Struct_CAN_Manage_Object CAN2_Manage_Object = {nullptr};
Struct_CAN_Manage_Object CAN3_Manage_Object = {nullptr};

/* Function prototypes -------------------------------------------------------*/

static uint32_t CAN_Length_To_DLC(uint16_t Length);
static void CAN_Filter_Mask_Config(FDCAN_HandleTypeDef *hfdcan);
static Struct_CAN_Manage_Object *CAN_Get_Manage_Object(FDCAN_HandleTypeDef *hfdcan);

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 字节长度转换为FDCAN DLC宏
 *
 * @param Length 字节长度
 * @return uint32_t FDCAN DLC宏
 */
static uint32_t CAN_Length_To_DLC(uint16_t Length)
{
    switch (Length)
    {
    case 0:
        return FDCAN_DLC_BYTES_0;
    case 1:
        return FDCAN_DLC_BYTES_1;
    case 2:
        return FDCAN_DLC_BYTES_2;
    case 3:
        return FDCAN_DLC_BYTES_3;
    case 4:
        return FDCAN_DLC_BYTES_4;
    case 5:
        return FDCAN_DLC_BYTES_5;
    case 6:
        return FDCAN_DLC_BYTES_6;
    case 7:
        return FDCAN_DLC_BYTES_7;
    case 8:
        return FDCAN_DLC_BYTES_8;
    default:
        return FDCAN_DLC_BYTES_0;
    }
}

/**
 * @brief 获取CAN管理对象
 *
 * @param hfdcan CAN编号
 * @return Struct_CAN_Manage_Object* CAN管理对象
 */
static Struct_CAN_Manage_Object *CAN_Get_Manage_Object(FDCAN_HandleTypeDef *hfdcan)
{
    if (hfdcan == nullptr)
    {
        return nullptr;
    }

    if (hfdcan->Instance == FDCAN1)
    {
        return &CAN1_Manage_Object;
    }
    else if (hfdcan->Instance == FDCAN2)
    {
        return &CAN2_Manage_Object;
    }
#ifdef FDCAN3
    else if (hfdcan->Instance == FDCAN3)
    {
        return &CAN3_Manage_Object;
    }
#endif

    return nullptr;
}

/**
 * @brief 配置CAN的过滤器
 *
 * @param hfdcan CAN编号
 */
static void CAN_Filter_Mask_Config(FDCAN_HandleTypeDef *hfdcan)
{
    FDCAN_FilterTypeDef can_filter_init_structure = {0};

    // 配置RXFIFO0全通滤波器
    can_filter_init_structure.IdType = FDCAN_STANDARD_ID;
    can_filter_init_structure.FilterIndex = 0;
    can_filter_init_structure.FilterType = FDCAN_FILTER_MASK;
    can_filter_init_structure.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    can_filter_init_structure.FilterID1 = 0x00000000;
    can_filter_init_structure.FilterID2 = 0x00000000;
    HAL_FDCAN_ConfigFilter(hfdcan, &can_filter_init_structure);

    // 全局滤波器: 拒绝未匹配标准帧、未匹配扩展帧、标准遥控帧、扩展遥控帧
    HAL_FDCAN_ConfigGlobalFilter(hfdcan,
                                  FDCAN_REJECT,
                                  FDCAN_REJECT,
                                  FDCAN_REJECT_REMOTE,
                                  FDCAN_REJECT_REMOTE);
}

/**
 * @brief 初始化CAN总线
 *
 * @param hfdcan CAN编号
 * @param Callback_Function 处理回调函数
 */
void CAN_Init(FDCAN_HandleTypeDef *hfdcan, CAN_Callback Callback_Function)
{
    Struct_CAN_Manage_Object *can_manage_object = CAN_Get_Manage_Object(hfdcan);

    if (can_manage_object == nullptr)
    {
        return;
    }

    can_manage_object->CAN_Handler = hfdcan;
    can_manage_object->Callback_Function = Callback_Function;

    CAN_Filter_Mask_Config(hfdcan);

    // Rx FIFO0新消息,ERROR_WARNING,ERROR_PASSIVE,BUS_OFF走Interrupt Line0
    HAL_FDCAN_ConfigInterruptLines(hfdcan,
                                FDCAN_IT_RX_FIFO0_NEW_MESSAGE |
                                FDCAN_IT_ERROR_WARNING |
                                FDCAN_IT_ERROR_PASSIVE |
                                FDCAN_IT_BUS_OFF,
                                FDCAN_INTERRUPT_LINE0);

    HAL_FDCAN_Start(hfdcan);

    // 启动CAN接收,ERROR_WARNING,ERROR_PASSIVE,BUS_OFF中断
    HAL_FDCAN_ActivateNotification(hfdcan,
                                    FDCAN_IT_RX_FIFO0_NEW_MESSAGE |
                                    FDCAN_IT_ERROR_WARNING |
                                    FDCAN_IT_ERROR_PASSIVE |
                                    FDCAN_IT_BUS_OFF,
                                    0);
}

/**
 * @brief 发送标准数据帧
 *
 * @param hfdcan CAN编号
 * @param ID 标准帧ID
 * @param Data 被发送的数据指针
 * @param Length 长度, Classic CAN范围0~8
 * @return uint8_t HAL执行状态
 */
uint8_t CAN_Transmit_Data(FDCAN_HandleTypeDef *hfdcan, uint16_t ID, uint8_t *Data, uint16_t Length)
{
    FDCAN_TxHeaderTypeDef tx_header = {0};

    if ((hfdcan == nullptr) || ((Data == nullptr) && (Length != 0U)))
    {
        return HAL_ERROR;
    }

    if ((ID > 0x7ffU) || (Length > 8U))
    {
        return HAL_ERROR;
    }

    if (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) == 0U)
    {
        return HAL_BUSY;
    }

    tx_header.Identifier = ID;
    tx_header.IdType = FDCAN_STANDARD_ID;
    tx_header.TxFrameType = FDCAN_DATA_FRAME;
    tx_header.DataLength = CAN_Length_To_DLC(Length);
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0;

    return HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &tx_header, Data);
}

/**
 * @brief CAN的TIM定时器中断发送回调函数
 */
void TIM_100us_CAN_PeriodElapsedCallback()
{
}

/**
 * @brief CAN的TIM定时器中断发送回调函数
 */
void TIM_1ms_CAN_PeriodElapsedCallback()
{
}

/**
 * @brief HAL库CAN接收FIFO0中断
 *
 * @param hfdcan CAN编号
 * @param RxFifo0ITs Rx FIFO0中断标志
 */
extern "C" void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    Struct_CAN_Manage_Object *can_manage_object = CAN_Get_Manage_Object(hfdcan);

    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0U)
    {
        return;
    }

    if (can_manage_object == nullptr)
    {
        return;
    }

    // 判断程序初始化完成
    if (!init_finished)
    {
        // 初始化未完成时也要清空FIFO, 防止FIFO满
        while (HAL_FDCAN_GetRxMessage(hfdcan,
                                      FDCAN_RX_FIFO0,
                                      &can_manage_object->Rx_Header,
                                      can_manage_object->Rx_Buffer) == HAL_OK)
        {
        }

        return;
    }

    while (HAL_FDCAN_GetRxMessage(hfdcan,
                                  FDCAN_RX_FIFO0,
                                  &can_manage_object->Rx_Header,
                                  can_manage_object->Rx_Buffer) == HAL_OK)
    {
        can_manage_object->Rx_Timestamp = SYS_Timestamp.Get_Current_Timestamp();

        if (can_manage_object->Callback_Function != nullptr)
        {
            can_manage_object->Callback_Function(can_manage_object->Rx_Header,
                                                 can_manage_object->Rx_Buffer);
        }
    }
}

/**
 * @brief HAL库CAN错误中断回调函数
 *
 * @param hfdcan CAN编号
 * @param ErrorStatusITs 错误中断标志
 */
extern "C" void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
    if ((ErrorStatusITs & FDCAN_IT_ERROR_WARNING) != 0U)
    {
        // CAN 错误警告
    }

    if ((ErrorStatusITs & FDCAN_IT_ERROR_PASSIVE) != 0U)
    {
        // CAN 进入 Error Passive
    }

    if ((ErrorStatusITs & FDCAN_IT_BUS_OFF) != 0U)
    {
        // CAN 进入 Bus-Off, 启动 Bus-Off 恢复流程
        CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);
    }
}

/*----------------------------------------------------------------------------*/

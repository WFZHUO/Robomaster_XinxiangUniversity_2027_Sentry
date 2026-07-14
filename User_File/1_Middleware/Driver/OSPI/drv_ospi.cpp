/**
 * @file drv_ospi.cpp
 * @author WangFonzhuo
 * @brief OSPI通用接口.
 *        OSPI 异步驱动. 命令、MDMA发送、MDMA接收和自动轮询的完成均通过中断回调通知；接口启动阶段 HAL 会同步检查并等待硬件 BUSY 清零，但正常状态下该等待会立即结束.
 * @version 1.0
 * @date 2026-07-13 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "drv_ospi.h"
#include "sys_timestamp.h"

#include <string.h>

/* Macros --------------------------------------------------------------------*/

#define OSPI_MDMA_BUFFER_SECTION __attribute__((section(".dma_buffer"), aligned(32)))
#define OSPI_DCACHE_LINE_SIZE    (32U)

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// OSPI管理对象
Struct_OSPI_Manage_Object OSPI1_Manage_Object = {};

#if defined(OCTOSPI2)
Struct_OSPI_Manage_Object OSPI2_Manage_Object = {};
#endif

// OSPI MDMA缓冲区, 单独放入DMA可访问区域
OSPI_MDMA_BUFFER_SECTION static uint8_t OSPI1_Tx_Buffer[OSPI_BUFFER_SIZE];
OSPI_MDMA_BUFFER_SECTION static uint8_t OSPI1_Rx_Buffer[OSPI_BUFFER_SIZE];

#if defined(OCTOSPI2)
OSPI_MDMA_BUFFER_SECTION static uint8_t OSPI2_Tx_Buffer[OSPI_BUFFER_SIZE];
OSPI_MDMA_BUFFER_SECTION static uint8_t OSPI2_Rx_Buffer[OSPI_BUFFER_SIZE];
#endif

/* Function prototypes -------------------------------------------------------*/

static Struct_OSPI_Manage_Object *OSPI_Get_Manage_Object(OSPI_HandleTypeDef *hospi);
static bool OSPI_Is_Busy(const Struct_OSPI_Manage_Object *ospi_manage_object);
static void OSPI_Clear_Transfer_State(Struct_OSPI_Manage_Object *ospi_manage_object);
static void OSPI_Complete_Operation(Struct_OSPI_Manage_Object *ospi_manage_object,
                                    uint8_t *Buffer,
                                    uint32_t Length,
                                    uint32_t Error_Code);
static void OSPI_Tx_DCache_Clean(uint8_t *Buffer, uint32_t Length);
static void OSPI_Rx_DCache_Invalidate(uint8_t *Buffer, uint32_t Length);

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 获取OSPI管理对象
 */
static Struct_OSPI_Manage_Object *OSPI_Get_Manage_Object(OSPI_HandleTypeDef *hospi)
{
    if (hospi == nullptr)
    {
        return nullptr;
    }

    if (hospi->Instance == OCTOSPI1)
    {
        return &OSPI1_Manage_Object;
    }

#if defined(OCTOSPI2)
    if (hospi->Instance == OCTOSPI2)
    {
        return &OSPI2_Manage_Object;
    }
#endif

    return nullptr;
}

/**
 * @brief 判断OSPI是否忙
 */
static bool OSPI_Is_Busy(const Struct_OSPI_Manage_Object *ospi_manage_object)
{
    if ((ospi_manage_object == nullptr) || (ospi_manage_object->OSPI_Handler == nullptr))
    {
        return true;
    }

    return ((ospi_manage_object->Operation != OSPI_OPERATION_IDLE) ||
            (HAL_OSPI_GetState(ospi_manage_object->OSPI_Handler) != HAL_OSPI_STATE_READY));
}

/**
 * @brief 清除一次传输状态
 */
static void OSPI_Clear_Transfer_State(Struct_OSPI_Manage_Object *ospi_manage_object)
{
    if (ospi_manage_object == nullptr)
    {
        return;
    }

    ospi_manage_object->Operation = OSPI_OPERATION_IDLE;
    ospi_manage_object->Tx_Buffer_Length = 0U;
    ospi_manage_object->Rx_Buffer_Length = 0U;
}

/**
 * @brief 完成当前操作并执行回调
 */
static void OSPI_Complete_Operation(Struct_OSPI_Manage_Object *ospi_manage_object,
                                    uint8_t *Buffer,
                                    uint32_t Length,
                                    uint32_t Error_Code)
{
    if (ospi_manage_object == nullptr)
    {
        return;
    }

    enum Enum_OSPI_Operation operation = ospi_manage_object->Operation;
    OSPI_Callback callback = ospi_manage_object->Callback_Function;

    ospi_manage_object->Timestamp = SYS_Timestamp.Get_Current_Timestamp();
    ospi_manage_object->Error_Code = Error_Code;
    OSPI_Clear_Transfer_State(ospi_manage_object);

    if (callback != nullptr)
    {
        callback(operation, Buffer, Length, Error_Code);
    }
}

/**
 * @brief 清理发送缓冲区DCache
 */
static void OSPI_Tx_DCache_Clean(uint8_t *Buffer, uint32_t Length)
{
#if (__DCACHE_PRESENT == 1U)
    if ((Buffer == nullptr) || (Length == 0U) || ((SCB->CCR & SCB_CCR_DC_Msk) == 0U))
    {
        return;
    }

    uintptr_t address = reinterpret_cast<uintptr_t>(Buffer);
    uintptr_t aligned_address = address & ~(static_cast<uintptr_t>(OSPI_DCACHE_LINE_SIZE) - 1U);
    uint32_t aligned_length = Length + static_cast<uint32_t>(address - aligned_address);
    aligned_length = (aligned_length + OSPI_DCACHE_LINE_SIZE - 1U) & ~(OSPI_DCACHE_LINE_SIZE - 1U);

    SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t *>(aligned_address),
                            static_cast<int32_t>(aligned_length));
#else
    (void) Buffer;
    (void) Length;
#endif
}

/**
 * @brief 失效接收缓冲区DCache
 */
static void OSPI_Rx_DCache_Invalidate(uint8_t *Buffer, uint32_t Length)
{
#if (__DCACHE_PRESENT == 1U)
    if ((Buffer == nullptr) || (Length == 0U) || ((SCB->CCR & SCB_CCR_DC_Msk) == 0U))
    {
        return;
    }

    uintptr_t address = reinterpret_cast<uintptr_t>(Buffer);
    uintptr_t aligned_address = address & ~(static_cast<uintptr_t>(OSPI_DCACHE_LINE_SIZE) - 1U);
    uint32_t aligned_length = Length + static_cast<uint32_t>(address - aligned_address);
    aligned_length = (aligned_length + OSPI_DCACHE_LINE_SIZE - 1U) & ~(OSPI_DCACHE_LINE_SIZE - 1U);

    SCB_InvalidateDCache_by_Addr(reinterpret_cast<uint32_t *>(aligned_address),
                                 static_cast<int32_t>(aligned_length));
#else
    (void) Buffer;
    (void) Length;
#endif
}

/**
 * @brief 初始化OSPI管理对象
 */
void OSPI_Init(OSPI_HandleTypeDef *hospi, OSPI_Callback Callback_Function)
{
    Struct_OSPI_Manage_Object *ospi_manage_object = OSPI_Get_Manage_Object(hospi);

    if (ospi_manage_object == nullptr)
    {
        return;
    }

    ospi_manage_object->OSPI_Handler = hospi;
    ospi_manage_object->Callback_Function = Callback_Function;

    if (hospi->Instance == OCTOSPI1)
    {
        ospi_manage_object->Tx_Buffer = OSPI1_Tx_Buffer;
        ospi_manage_object->Rx_Buffer = OSPI1_Rx_Buffer;
    }
#if defined(OCTOSPI2)
    else if (hospi->Instance == OCTOSPI2)
    {
        ospi_manage_object->Tx_Buffer = OSPI2_Tx_Buffer;
        ospi_manage_object->Rx_Buffer = OSPI2_Rx_Buffer;
    }
#endif

    ospi_manage_object->Timestamp = 0U;
    ospi_manage_object->Error_Code = HAL_OSPI_ERROR_NONE;
    OSPI_Clear_Transfer_State(ospi_manage_object);
}

/**
 * @brief 中断发送无数据阶段指令，正常情况下非阻塞，指令完成后通过回调通知
 */
uint8_t OSPI_Command(OSPI_HandleTypeDef *hospi,
                     const OSPI_RegularCmdTypeDef *Command)
{
    Struct_OSPI_Manage_Object *ospi_manage_object = OSPI_Get_Manage_Object(hospi);

    if ((ospi_manage_object == nullptr) ||
        (Command == nullptr) ||
        (Command->DataMode != HAL_OSPI_DATA_NONE))
    {
        return HAL_ERROR;
    }

    if (OSPI_Is_Busy(ospi_manage_object))
    {
        return HAL_BUSY;
    }

    OSPI_RegularCmdTypeDef command = *Command;

    ospi_manage_object->Operation = OSPI_OPERATION_COMMAND;
    ospi_manage_object->Error_Code = HAL_OSPI_ERROR_NONE;

    HAL_StatusTypeDef status = HAL_OSPI_Command_IT(hospi, &command);

    if (status != HAL_OK)
    {
        OSPI_Clear_Transfer_State(ospi_manage_object);
    }

    return static_cast<uint8_t>(status);
}

/**
 * @brief MDMA异步发送指令与数据，HAL_OSPI_Command仅配置命令，实际传输由MDMA完成
 */
uint8_t OSPI_Command_Transmit_Data(OSPI_HandleTypeDef *hospi,
                                   const OSPI_RegularCmdTypeDef *Command,
                                   const uint8_t *Data,
                                   uint32_t Length)
{
    Struct_OSPI_Manage_Object *ospi_manage_object = OSPI_Get_Manage_Object(hospi);

    if ((ospi_manage_object == nullptr) ||
        (Command == nullptr) ||
        (Data == nullptr) ||
        (Length == 0U) ||
        (Length > OSPI_BUFFER_SIZE) ||
        (Command->DataMode == HAL_OSPI_DATA_NONE) ||
        (hospi->hmdma == nullptr))
    {
        return HAL_ERROR;
    }

    if (OSPI_Is_Busy(ospi_manage_object))
    {
        return HAL_BUSY;
    }

    memcpy(ospi_manage_object->Tx_Buffer, Data, Length);
    OSPI_Tx_DCache_Clean(ospi_manage_object->Tx_Buffer, Length);

    OSPI_RegularCmdTypeDef command = *Command;
    command.NbData = Length;

    HAL_StatusTypeDef status = HAL_OSPI_Command(hospi,
                                                &command,
                                                HAL_OSPI_TIMEOUT_DEFAULT_VALUE);

    if (status != HAL_OK)
    {
        ospi_manage_object->Error_Code = HAL_OSPI_GetError(hospi);
        return static_cast<uint8_t>(status);
    }

    ospi_manage_object->Operation = OSPI_OPERATION_TRANSMIT;
    ospi_manage_object->Tx_Buffer_Length = Length;
    ospi_manage_object->Error_Code = HAL_OSPI_ERROR_NONE;

    status = HAL_OSPI_Transmit_DMA(hospi, ospi_manage_object->Tx_Buffer);

    if (status != HAL_OK)
    {
        OSPI_Clear_Transfer_State(ospi_manage_object);
    }

    return static_cast<uint8_t>(status);
}

/**
 * @brief MDMA异步发送指令并接收数据，HAL_OSPI_Command仅配置命令，实际传输由MDMA完成
 */
uint8_t OSPI_Command_Receive_Data(OSPI_HandleTypeDef *hospi,
                                  const OSPI_RegularCmdTypeDef *Command,
                                  uint32_t Length)
{
    Struct_OSPI_Manage_Object *ospi_manage_object = OSPI_Get_Manage_Object(hospi);

    if ((ospi_manage_object == nullptr) ||
        (Command == nullptr) ||
        (Length == 0U) ||
        (Length > OSPI_BUFFER_SIZE) ||
        (Command->DataMode == HAL_OSPI_DATA_NONE) ||
        (hospi->hmdma == nullptr))
    {
        return HAL_ERROR;
    }

    if (OSPI_Is_Busy(ospi_manage_object))
    {
        return HAL_BUSY;
    }

    OSPI_RegularCmdTypeDef command = *Command;
    command.NbData = Length;

    HAL_StatusTypeDef status = HAL_OSPI_Command(hospi,
                                                &command,
                                                HAL_OSPI_TIMEOUT_DEFAULT_VALUE);

    if (status != HAL_OK)
    {
        ospi_manage_object->Error_Code = HAL_OSPI_GetError(hospi);
        return static_cast<uint8_t>(status);
    }

    ospi_manage_object->Operation = OSPI_OPERATION_RECEIVE;
    ospi_manage_object->Rx_Buffer_Length = Length;
    ospi_manage_object->Error_Code = HAL_OSPI_ERROR_NONE;

    // DMA接收前先失效, 避免脏Cache行在DMA期间回写
    OSPI_Rx_DCache_Invalidate(ospi_manage_object->Rx_Buffer, Length);
    status = HAL_OSPI_Receive_DMA(hospi, ospi_manage_object->Rx_Buffer);

    if (status != HAL_OK)
    {
        OSPI_Clear_Transfer_State(ospi_manage_object);
    }

    return static_cast<uint8_t>(status);
}

/**
 * @brief 中断异步自动轮询，HAL_OSPI_Command仅配置命令，状态匹配后通过回调通知
 */
uint8_t OSPI_Auto_Polling(OSPI_HandleTypeDef *hospi,
                          const OSPI_RegularCmdTypeDef *Command,
                          const OSPI_AutoPollingTypeDef *Config)
{
    Struct_OSPI_Manage_Object *ospi_manage_object = OSPI_Get_Manage_Object(hospi);

    if ((ospi_manage_object == nullptr) ||
        (Command == nullptr) ||
        (Config == nullptr) ||
        (Command->DataMode == HAL_OSPI_DATA_NONE) ||
        (Command->NbData == 0U))
    {
        return HAL_ERROR;
    }

    if (OSPI_Is_Busy(ospi_manage_object))
    {
        return HAL_BUSY;
    }

    OSPI_RegularCmdTypeDef command = *Command;
    OSPI_AutoPollingTypeDef config = *Config;

    HAL_StatusTypeDef status = HAL_OSPI_Command(hospi,
                                                &command,
                                                HAL_OSPI_TIMEOUT_DEFAULT_VALUE);

    if (status != HAL_OK)
    {
        ospi_manage_object->Error_Code = HAL_OSPI_GetError(hospi);
        return static_cast<uint8_t>(status);
    }

    ospi_manage_object->Operation = OSPI_OPERATION_AUTO_POLLING;
    ospi_manage_object->Error_Code = HAL_OSPI_ERROR_NONE;

    status = HAL_OSPI_AutoPolling_IT(hospi, &config);

    if (status != HAL_OK)
    {
        OSPI_Clear_Transfer_State(ospi_manage_object);
    }

    return static_cast<uint8_t>(status);
}

/**
 * @brief HAL库OSPI指令完成回调函数
 */
extern "C" void HAL_OSPI_CmdCpltCallback(OSPI_HandleTypeDef *hospi)
{
    Struct_OSPI_Manage_Object *ospi_manage_object = OSPI_Get_Manage_Object(hospi);

    if ((ospi_manage_object == nullptr) ||
        (ospi_manage_object->Operation != OSPI_OPERATION_COMMAND))
    {
        return;
    }

    OSPI_Complete_Operation(ospi_manage_object,
                            nullptr,
                            0U,
                            HAL_OSPI_ERROR_NONE);
}

/**
 * @brief HAL库OSPI自动轮询完成回调函数
 */
extern "C" void HAL_OSPI_StatusMatchCallback(OSPI_HandleTypeDef *hospi)
{
    Struct_OSPI_Manage_Object *ospi_manage_object = OSPI_Get_Manage_Object(hospi);

    if ((ospi_manage_object == nullptr) ||
        (ospi_manage_object->Operation != OSPI_OPERATION_AUTO_POLLING))
    {
        return;
    }

    OSPI_Complete_Operation(ospi_manage_object,
                            nullptr,
                            0U,
                            HAL_OSPI_ERROR_NONE);
}

/**
 * @brief HAL库OSPI接收完成回调函数
 */
extern "C" void HAL_OSPI_RxCpltCallback(OSPI_HandleTypeDef *hospi)
{
    Struct_OSPI_Manage_Object *ospi_manage_object = OSPI_Get_Manage_Object(hospi);

    if ((ospi_manage_object == nullptr) ||
        (ospi_manage_object->Operation != OSPI_OPERATION_RECEIVE))
    {
        return;
    }

    uint32_t length = ospi_manage_object->Rx_Buffer_Length;
    OSPI_Rx_DCache_Invalidate(ospi_manage_object->Rx_Buffer, length);

    OSPI_Complete_Operation(ospi_manage_object,
                            ospi_manage_object->Rx_Buffer,
                            length,
                            HAL_OSPI_ERROR_NONE);
}

/**
 * @brief HAL库OSPI发送完成回调函数
 */
extern "C" void HAL_OSPI_TxCpltCallback(OSPI_HandleTypeDef *hospi)
{
    Struct_OSPI_Manage_Object *ospi_manage_object = OSPI_Get_Manage_Object(hospi);

    if ((ospi_manage_object == nullptr) ||
        (ospi_manage_object->Operation != OSPI_OPERATION_TRANSMIT))
    {
        return;
    }

    OSPI_Complete_Operation(ospi_manage_object,
                            ospi_manage_object->Tx_Buffer,
                            ospi_manage_object->Tx_Buffer_Length,
                            HAL_OSPI_ERROR_NONE);
}

/**
 * @brief HAL库OSPI错误回调函数
 */
extern "C" void HAL_OSPI_ErrorCallback(OSPI_HandleTypeDef *hospi)
{
    Struct_OSPI_Manage_Object *ospi_manage_object = OSPI_Get_Manage_Object(hospi);

    if (ospi_manage_object == nullptr)
    {
        return;
    }

    OSPI_Complete_Operation(ospi_manage_object,
                            nullptr,
                            0U,
                            HAL_OSPI_GetError(hospi));
}

/*----------------------------------------------------------------------------*/

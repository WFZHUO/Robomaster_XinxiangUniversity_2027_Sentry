/**
 * @file drv_spi.cpp
 * @author WangFonzhuo
 * @brief SPI通用接口
 * @version 1.0
 * @date 2026-07-11 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "drv_spi.h"
#include "sys_timestamp.h"

#include <string.h>

/* Macros --------------------------------------------------------------------*/

#define SPI_DMA_BUFFER_SECTION  __attribute__((section(".dma_buffer"), aligned(32)))
#define SPI_BDMA_BUFFER_SECTION __attribute__((section(".bdma_buffer"), aligned(32)))
#define SPI_DCACHE_LINE_SIZE   (32U)

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// SPI管理对象
Struct_SPI_Manage_Object SPI1_Manage_Object = {};
Struct_SPI_Manage_Object SPI2_Manage_Object = {};
Struct_SPI_Manage_Object SPI3_Manage_Object = {};
Struct_SPI_Manage_Object SPI4_Manage_Object = {};
Struct_SPI_Manage_Object SPI6_Manage_Object = {};

// SPI DMA缓冲区, 单独定义 DMA 专用缓冲区
SPI_DMA_BUFFER_SECTION uint8_t SPI1_Tx_Buffer[SPI_BUFFER_SIZE];
SPI_DMA_BUFFER_SECTION uint8_t SPI1_Rx_Buffer[SPI_BUFFER_SIZE];
SPI_DMA_BUFFER_SECTION uint8_t SPI2_Tx_Buffer[SPI_BUFFER_SIZE];
SPI_DMA_BUFFER_SECTION uint8_t SPI2_Rx_Buffer[SPI_BUFFER_SIZE];
SPI_DMA_BUFFER_SECTION uint8_t SPI3_Tx_Buffer[SPI_BUFFER_SIZE];
SPI_DMA_BUFFER_SECTION uint8_t SPI3_Rx_Buffer[SPI_BUFFER_SIZE];
SPI_DMA_BUFFER_SECTION uint8_t SPI4_Tx_Buffer[SPI_BUFFER_SIZE];
SPI_DMA_BUFFER_SECTION uint8_t SPI4_Rx_Buffer[SPI_BUFFER_SIZE];
SPI_BDMA_BUFFER_SECTION uint8_t SPI6_Tx_Buffer[SPI_BUFFER_SIZE];
SPI_BDMA_BUFFER_SECTION uint8_t SPI6_Rx_Buffer[SPI_BUFFER_SIZE];

/* Function prototypes -------------------------------------------------------*/

static Struct_SPI_Manage_Object *SPI_Get_Manage_Object(SPI_HandleTypeDef *hspi);
static void SPI_Assign_Buffer(Struct_SPI_Manage_Object *spi_manage_object, uint8_t *Tx_Buffer, uint8_t *Rx_Buffer);
static void SPI_Select(Struct_SPI_Manage_Object *spi_manage_object);
static void SPI_Deselect(Struct_SPI_Manage_Object *spi_manage_object);
static void SPI_Clear_Transfer_State(Struct_SPI_Manage_Object *spi_manage_object);
static void SPI_Tx_DCache_Clean(uint8_t *Buffer, uint16_t Length);
static void SPI_Rx_DCache_Invalidate(uint8_t *Buffer, uint16_t Length);

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 获取SPI管理对象
 *
 * @param hspi SPI编号
 * @return Struct_SPI_Manage_Object* SPI管理对象
 */
static Struct_SPI_Manage_Object *SPI_Get_Manage_Object(SPI_HandleTypeDef *hspi)
{
    if (hspi == nullptr)
    {
        return nullptr;
    }

    if (hspi->Instance == SPI1)
    {
        return &SPI1_Manage_Object;
    }
    else if (hspi->Instance == SPI2)
    {
        return &SPI2_Manage_Object;
    }
    else if (hspi->Instance == SPI3)
    {
        return &SPI3_Manage_Object;
    }
    else if (hspi->Instance == SPI4)
    {
        return &SPI4_Manage_Object;
    }
#ifdef SPI6
    else if (hspi->Instance == SPI6)
    {
        return &SPI6_Manage_Object;
    }
#endif

    return nullptr;
}

/**
 * @brief 绑定SPI DMA缓冲区
 *
 * @param spi_manage_object SPI管理对象
 * @param Tx_Buffer 发送缓冲区
 * @param Rx_Buffer 接收缓冲区
 */
static void SPI_Assign_Buffer(Struct_SPI_Manage_Object *spi_manage_object, uint8_t *Tx_Buffer, uint8_t *Rx_Buffer)
{
    if (spi_manage_object == nullptr)
    {
        return;
    }

    spi_manage_object->Tx_Buffer = Tx_Buffer;
    spi_manage_object->Rx_Buffer = Rx_Buffer;
}

/**
 * @brief 拉低/拉高片选到有效电平
 *
 * @param spi_manage_object SPI管理对象
 */
static void SPI_Select(Struct_SPI_Manage_Object *spi_manage_object)
{
    if ((spi_manage_object == nullptr) || (spi_manage_object->Activate_GPIOx == nullptr))
    {
        return;
    }

    HAL_GPIO_WritePin(spi_manage_object->Activate_GPIOx,
                      spi_manage_object->Activate_GPIO_Pin,
                      spi_manage_object->Activate_Level);
}

/**
 * @brief 释放片选到无效电平
 *
 * @param spi_manage_object SPI管理对象
 */
static void SPI_Deselect(Struct_SPI_Manage_Object *spi_manage_object)
{
    if ((spi_manage_object == nullptr) || (spi_manage_object->Activate_GPIOx == nullptr))
    {
        return;
    }

    HAL_GPIO_WritePin(spi_manage_object->Activate_GPIOx,
                      spi_manage_object->Activate_GPIO_Pin,
                      (spi_manage_object->Activate_Level == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/**
 * @brief 清除一次传输状态
 *
 * @param spi_manage_object SPI管理对象
 */
static void SPI_Clear_Transfer_State(Struct_SPI_Manage_Object *spi_manage_object)
{
    if (spi_manage_object == nullptr)
    {
        return;
    }

    spi_manage_object->Activate_GPIOx = nullptr;
    spi_manage_object->Activate_GPIO_Pin = 0U;
    spi_manage_object->Activate_Level = GPIO_PIN_RESET;
    spi_manage_object->Tx_Buffer_Length = 0U;
    spi_manage_object->Rx_Buffer_Length = 0U;
    spi_manage_object->Tx_Busy = false;
    spi_manage_object->Rx_Busy = false;
}

/**
 * @brief 清理发送缓冲区DCache
 *
 * @param Buffer 缓冲区
 * @param Length 长度
 */
static void SPI_Tx_DCache_Clean(uint8_t *Buffer, uint16_t Length)
{
#if (__DCACHE_PRESENT == 1U)
    if ((Buffer == nullptr) || (Length == 0U))
    {
        return;
    }

    // DCache 没有开启，则不需要清理
    if ((SCB->CCR & SCB_CCR_DC_Msk) == 0U)
    {
        return;
    }

    uint32_t address = reinterpret_cast<uint32_t>(Buffer);
    uint32_t aligned_address = address & ~(SPI_DCACHE_LINE_SIZE - 1U);
    uint32_t aligned_length = static_cast<uint32_t>(Length) + (address - aligned_address);
    aligned_length = (aligned_length + SPI_DCACHE_LINE_SIZE - 1U) & ~(SPI_DCACHE_LINE_SIZE - 1U);

    SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t *>(aligned_address),
                            static_cast<int32_t>(aligned_length));
#else
    (void)Buffer;
    (void)Length;
#endif
}

/**
 * @brief 失效接收缓冲区DCache
 *
 * @param Buffer 缓冲区
 * @param Length 长度
 */
static void SPI_Rx_DCache_Invalidate(uint8_t *Buffer, uint16_t Length)
{
#if (__DCACHE_PRESENT == 1U)
    if ((Buffer == nullptr) || (Length == 0U))
    {
        return;
    }

    // DCache 没有开启，则不需要失效
    if ((SCB->CCR & SCB_CCR_DC_Msk) == 0U)
    {
        return;
    }

    uint32_t address = reinterpret_cast<uint32_t>(Buffer);
    uint32_t aligned_address = address & ~(SPI_DCACHE_LINE_SIZE - 1U);
    uint32_t aligned_length = static_cast<uint32_t>(Length) + (address - aligned_address);
    aligned_length = (aligned_length + SPI_DCACHE_LINE_SIZE - 1U) & ~(SPI_DCACHE_LINE_SIZE - 1U);

    SCB_InvalidateDCache_by_Addr(reinterpret_cast<uint32_t *>(aligned_address),
                                 static_cast<int32_t>(aligned_length));
#else
    (void)Buffer;
    (void)Length;
#endif
}

/**
 * @brief 初始化SPI管理对象
 *
 * @param hspi SPI编号
 * @param Callback_Function 传输完成回调函数
 */
void SPI_Init(SPI_HandleTypeDef *hspi, SPI_Callback Callback_Function)
{
    Struct_SPI_Manage_Object *spi_manage_object = SPI_Get_Manage_Object(hspi);

    if (spi_manage_object == nullptr)
    {
        return;
    }

    spi_manage_object->SPI_Handler = hspi;
    spi_manage_object->Callback_Function = Callback_Function;
    SPI_Clear_Transfer_State(spi_manage_object);

    if (hspi->Instance == SPI1)
    {
        SPI_Assign_Buffer(spi_manage_object, SPI1_Tx_Buffer, SPI1_Rx_Buffer);
    }
    else if (hspi->Instance == SPI2)
    {
        SPI_Assign_Buffer(spi_manage_object, SPI2_Tx_Buffer, SPI2_Rx_Buffer);
    }
    else if (hspi->Instance == SPI3)
    {
        SPI_Assign_Buffer(spi_manage_object, SPI3_Tx_Buffer, SPI3_Rx_Buffer);
    }
    else if (hspi->Instance == SPI4)
    {
        SPI_Assign_Buffer(spi_manage_object, SPI4_Tx_Buffer, SPI4_Rx_Buffer);
    }
#ifdef SPI6
    else if (hspi->Instance == SPI6)
    {
        SPI_Assign_Buffer(spi_manage_object, SPI6_Tx_Buffer, SPI6_Rx_Buffer);
    }
#endif
}

/**
 * @brief SPI DMA发送数据
 *
 * @param hspi SPI编号
 * @param Data 被发送的数据指针
 * @param Length 发送长度
 * @param GPIOx 片选GPIO编组, 不使用片选则填nullptr
 * @param GPIO_Pin 片选GPIO引脚
 * @param Activate_Level 片选有效电平
 * @return uint8_t HAL执行状态
 */
uint8_t SPI_Transmit_Data(SPI_HandleTypeDef *hspi,
                          uint8_t *Data,
                          uint16_t Length,
                          GPIO_TypeDef *GPIOx,
                          uint16_t GPIO_Pin,
                          GPIO_PinState Activate_Level)
{
    Struct_SPI_Manage_Object *spi_manage_object = SPI_Get_Manage_Object(hspi);

    if ((spi_manage_object == nullptr) || (Data == nullptr) || (Length == 0U) || (Length > SPI_BUFFER_SIZE))
    {
        return HAL_ERROR;
    }

    if (spi_manage_object->Tx_Busy || spi_manage_object->Rx_Busy)
    {
        return HAL_BUSY;
    }

    memcpy(spi_manage_object->Tx_Buffer, Data, Length);

    spi_manage_object->Activate_GPIOx = GPIOx;
    spi_manage_object->Activate_GPIO_Pin = GPIO_Pin;
    spi_manage_object->Activate_Level = Activate_Level;
    spi_manage_object->Tx_Buffer_Length = Length;
    spi_manage_object->Rx_Buffer_Length = 0U;
    spi_manage_object->Tx_Busy = true;
    spi_manage_object->Rx_Busy = false;

    SPI_Tx_DCache_Clean(spi_manage_object->Tx_Buffer, Length);
    SPI_Select(spi_manage_object);

    HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(hspi, spi_manage_object->Tx_Buffer, Length);

    if (status != HAL_OK)
    {
        SPI_Deselect(spi_manage_object);
        SPI_Clear_Transfer_State(spi_manage_object);
    }

    return static_cast<uint8_t>(status);
}

/**
 * @brief SPI DMA全双工收发数据
 *
 * @param hspi SPI编号
 * @param Tx_Data 被发送的数据指针
 * @param Length 收发长度
 * @param GPIOx 片选GPIO编组, 不使用片选则填nullptr
 * @param GPIO_Pin 片选GPIO引脚
 * @param Activate_Level 片选有效电平
 * @return uint8_t HAL执行状态
 */
uint8_t SPI_Transmit_Receive_Data(SPI_HandleTypeDef *hspi,
                                  uint8_t *Tx_Data,
                                  uint16_t Length,
                                  GPIO_TypeDef *GPIOx,
                                  uint16_t GPIO_Pin,
                                  GPIO_PinState Activate_Level)
{
    Struct_SPI_Manage_Object *spi_manage_object = SPI_Get_Manage_Object(hspi);

    if ((spi_manage_object == nullptr) || (Tx_Data == nullptr) || (Length == 0U) || (Length > SPI_BUFFER_SIZE))
    {
        return HAL_ERROR;
    }

    if (spi_manage_object->Tx_Busy || spi_manage_object->Rx_Busy)
    {
        return HAL_BUSY;
    }

    memcpy(spi_manage_object->Tx_Buffer, Tx_Data, Length);

    spi_manage_object->Activate_GPIOx = GPIOx;
    spi_manage_object->Activate_GPIO_Pin = GPIO_Pin;
    spi_manage_object->Activate_Level = Activate_Level;
    spi_manage_object->Tx_Buffer_Length = Length;
    spi_manage_object->Rx_Buffer_Length = Length;
    spi_manage_object->Tx_Busy = true;
    spi_manage_object->Rx_Busy = true;

    SPI_Tx_DCache_Clean(spi_manage_object->Tx_Buffer, Length);
    SPI_Select(spi_manage_object);

    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(hspi,
                                                           spi_manage_object->Tx_Buffer,
                                                           spi_manage_object->Rx_Buffer,
                                                           Length);

    if (status != HAL_OK)
    {
        SPI_Deselect(spi_manage_object);
        SPI_Clear_Transfer_State(spi_manage_object);
    }

    return static_cast<uint8_t>(status);
}

/**
 * @brief HAL库SPI仅发送完成回调函数
 *
 * @param hspi SPI编号
 */
extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    Struct_SPI_Manage_Object *spi_manage_object = SPI_Get_Manage_Object(hspi);

    if (spi_manage_object == nullptr)
    {
        return;
    }

    SPI_Deselect(spi_manage_object);

    SPI_Clear_Transfer_State(spi_manage_object);

    if (spi_manage_object->Callback_Function != nullptr)
    {
        spi_manage_object->Callback_Function(spi_manage_object->Tx_Buffer,
                                             nullptr,
                                             spi_manage_object->Tx_Buffer_Length,
                                             0U);
    }
}

/**
 * @brief HAL库SPI全双工DMA完成回调函数
 *
 * @param hspi SPI编号
 */
extern "C" void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    Struct_SPI_Manage_Object *spi_manage_object = SPI_Get_Manage_Object(hspi);

    if (spi_manage_object == nullptr)
    {
        return;
    }

    spi_manage_object->Rx_Timestamp = SYS_Timestamp.Get_Current_Timestamp();

    SPI_Rx_DCache_Invalidate(spi_manage_object->Rx_Buffer, spi_manage_object->Rx_Buffer_Length);

    SPI_Deselect(spi_manage_object);

    SPI_Clear_Transfer_State(spi_manage_object);

    if (spi_manage_object->Callback_Function != nullptr)
    {
        spi_manage_object->Callback_Function(spi_manage_object->Tx_Buffer,
                                             spi_manage_object->Rx_Buffer,
                                             spi_manage_object->Tx_Buffer_Length,
                                             spi_manage_object->Rx_Buffer_Length);
    }
}

/**
 * @brief HAL库SPI错误回调函数
 *
 * @param hspi SPI编号
 */
extern "C" void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    Struct_SPI_Manage_Object *spi_manage_object = SPI_Get_Manage_Object(hspi);

    if (spi_manage_object == nullptr)
    {
        return;
    }

    SPI_Deselect(spi_manage_object);
    SPI_Clear_Transfer_State(spi_manage_object);
}

/*----------------------------------------------------------------------------*/

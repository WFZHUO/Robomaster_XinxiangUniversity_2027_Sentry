/**
 * @file dvc_serialplot.cpp
 * @author WangFonzhuo
 * @brief Serialplot串口绘图
 * @version 1.0
 * @date 2026-04-25 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "dvc_serialplot.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// Serialplot发送缓冲区, 单独定义 DMA 专用缓冲区
__attribute__((section(".dma_buffer"))) static uint8_t Serialplot_Tx_Buffer[UART_BUFFER_SIZE];

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 8位累加和校验
 */
static uint8_t Serialplot_Sum_8(const uint8_t *Address, uint32_t Length)
{
    uint8_t sum = 0;

    if (Address == nullptr)
    {
        return 0;
    }

    for (uint32_t i = 0; i < Length; i++)
    {
        sum += Address[i];
    }

    return sum;
}

/**
 * @brief 简易字符串转float, 解析形如 -12.34 的十进制数
 */
static float Serialplot_Parse_Float(const uint8_t *Data, uint16_t Start, uint16_t End)
{
    if (Data == nullptr || Start >= End)
    {
        return 0.0f;
    }

    float value = 0.0f;
    float decimal_factor = 0.1f;
    bool negative = false;
    bool decimal = false;

    uint16_t i = Start;

    if (Data[i] == '-')
    {
        negative = true;
        i++;
    }
    else if (Data[i] == '+')
    {
        i++;
    }

    for (; i < End; i++)
    {
        uint8_t ch = Data[i];

        if (ch == '.')
        {
            decimal = true;
            continue;
        }

        if (ch < '0' || ch > '9')
        {
            break;
        }

        if (!decimal)
        {
            value = value * 10.0f + static_cast<float>(ch - '0');
        }
        else
        {
            value += static_cast<float>(ch - '0') * decimal_factor;
            decimal_factor *= 0.1f;
        }
    }

    return negative ? -value : value;
}

/**
 * @brief 串口绘图初始化
 * @param huart UART句柄
 * @param __Checksum_8 是否开启8位累加和校验
 * @param __Rx_Variable_Assignment_Num 接收变量分配数量
 * @param __Rx_Variable_Assignment_List 接收变量分配列表
 * @param __Data_Type 数据类型
 * @param __Frame_Header 帧头
 */
void Class_Serialplot_UART::Init(const UART_HandleTypeDef *huart,
                                 const Enum_Serialplot_Checksum_8 &__Checksum_8,
                                 const uint8_t &__Rx_Variable_Assignment_Num,
                                 const char **__Rx_Variable_Assignment_List,
                                 const Enum_Serialplot_Data_Type &__Data_Type,
                                 const uint8_t &__Frame_Header)
{
    UART_Manage_Object = nullptr;

    if (huart == nullptr)
    {
        return;
    }

    if (huart->Instance == USART1)
    {
        UART_Manage_Object = &UART1_Manage_Object;
    }
    else if (huart->Instance == USART2)
    {
        UART_Manage_Object = &UART2_Manage_Object;
    }
    else if (huart->Instance == USART3)
    {
        UART_Manage_Object = &UART3_Manage_Object;
    }
    else if (huart->Instance == UART5)
    {
        UART_Manage_Object = &UART5_Manage_Object;
    }
    else if (huart->Instance == UART7)
    {
        UART_Manage_Object = &UART7_Manage_Object;
    }
    else if (huart->Instance == UART8)
    {
        UART_Manage_Object = &UART8_Manage_Object;
    }
    else if (huart->Instance == UART9)
    {
        UART_Manage_Object = &UART9_Manage_Object;
    }
    else if (huart->Instance == USART10)
    {
        UART_Manage_Object = &UART10_Manage_Object;
    }

    Checksum_8 = __Checksum_8;
    Rx_Variable_Num = __Rx_Variable_Assignment_Num;
    Rx_Variable_List = __Rx_Variable_Assignment_List;
    Tx_Data_Type = __Data_Type;
    Frame_Header = __Frame_Header;

    Tx_Buffer = Serialplot_Tx_Buffer;
    Tx_Buffer[0] = Frame_Header;
}

/**
 * @brief UART接收完成回调函数
 * @param Rx_Data 接收到的数据
 * @param Length 接收到的数据长度
 */
void Class_Serialplot_UART::UART_RxCpltCallback(const uint8_t *Rx_Data, uint16_t Length)
{
    if (Rx_Data == nullptr || Length == 0)
    {
        return;
    }

    Data_Process(Rx_Data, Length);
}

/**
 * @brief 1ms定时器周期发送函数
 */
void Class_Serialplot_UART::TIM_1ms_Write_PeriodElapsedCallback()
{
    if (UART_Manage_Object == nullptr || UART_Manage_Object->UART_Handler == nullptr || Tx_Buffer == nullptr)
    {
        return;
    }

    uint16_t type_size = Get_Data_Type_Size();

    if (type_size == 0)
    {
        return;
    }

    uint16_t data_length = 1 + Data_Number * type_size;

    if (Checksum_8 == Serialplot_Checksum_8_ENABLE)
    {
        data_length += 1;
    }

    if (data_length > UART_BUFFER_SIZE)
    {
        return;
    }

    Output();

    UART_Transmit_Data(UART_Manage_Object->UART_Handler, Tx_Buffer, data_length);
}

/**
 * @brief 数据处理函数
 * @param Rx_Data 接收到的数据
 * @param Length 数据长度
 */
void Class_Serialplot_UART::Data_Process(const uint8_t *Rx_Data, uint16_t Length)
{
    uint16_t value_start_index = 0;

    Variable_Index = Judge_Variable_Name(Rx_Data, Length, &value_start_index);

    if (Variable_Index < 0)
    {
        return;
    }

    Judge_Variable_Value(Rx_Data, Length, value_start_index);
}

/**
 * @brief 判断接收指令名称
 * @param Rx_Data 接收到的数据
 * @param Length 数据长度
 * @param value_start_index 值的起始索引
 * @return 指令编号
 */
int32_t Class_Serialplot_UART::Judge_Variable_Name(const uint8_t *Rx_Data, uint16_t Length, uint16_t *value_start_index)
{
    if (Rx_Data == nullptr || Length == 0 || value_start_index == nullptr || Rx_Variable_List == nullptr || Rx_Variable_Num == 0)
    {
        return -1;
    }

    uint16_t equal_index = 0;

    while (equal_index < Length && Rx_Data[equal_index] != '=' && Rx_Data[equal_index] != 0)
    {
        equal_index++;
    }

    if (equal_index >= Length || Rx_Data[equal_index] != '=')
    {
        return -1;
    }

    *value_start_index = equal_index + 1;

    char tmp_variable_name[SERIALPLOT_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH] = {0};
    uint16_t copy_len = equal_index;

    if (copy_len >= SERIALPLOT_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH)
    {
        copy_len = SERIALPLOT_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH - 1;
    }

    memcpy(tmp_variable_name, Rx_Data, copy_len);
    tmp_variable_name[copy_len] = '\0';

    for (uint8_t i = 0; i < Rx_Variable_Num; i++)
    {
        if (Rx_Variable_List[i] == nullptr)
        {
            continue;
        }

        if (strcmp(tmp_variable_name, Rx_Variable_List[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}

/**
 * @brief 判断接收指令值
 * @param Rx_Data 接收到的数据
 * @param Length 数据长度
 * @param value_start_index 值的起始索引
 */
void Class_Serialplot_UART::Judge_Variable_Value(const uint8_t *Rx_Data, uint16_t Length, uint16_t value_start_index)
{
    if (Rx_Data == nullptr || value_start_index >= Length)
    {
        Variable_Value = 0.0f;
        return;
    }

    uint16_t end_index = value_start_index;

    while (end_index < Length && Rx_Data[end_index] != '#' && Rx_Data[end_index] != 0)
    {
        end_index++;
    }

    Variable_Value = Serialplot_Parse_Float(Rx_Data, value_start_index, end_index);
}

/**
 * @brief 输出数据
 */
void Class_Serialplot_UART::Output()
{
    if (Tx_Buffer == nullptr)
    {
        return;
    }

    memset(Tx_Buffer, 0, UART_BUFFER_SIZE);

    Tx_Buffer[0] = Frame_Header;

    uint16_t type_size = Get_Data_Type_Size();
    uint16_t data_size = Data_Number * type_size;
    uint16_t total_size = 1 + data_size + ((Checksum_8 == Serialplot_Checksum_8_ENABLE) ? 1 : 0);

    if (type_size == 0 || total_size > UART_BUFFER_SIZE)
    {
        return;
    }

    for (uint8_t i = 0; i < Data_Number; i++)
    {
        if (Data[i] == nullptr)
        {
            continue;
        }

        memcpy(Tx_Buffer + 1 + i * type_size, Data[i], type_size);
    }

    if (Checksum_8 == Serialplot_Checksum_8_ENABLE)
    {
        Tx_Buffer[1 + data_size] = Serialplot_Sum_8(Tx_Buffer + 1, data_size);
    }
}

/**
 * @brief 获取数据类型大小
 * @return 数据类型大小
 */
uint16_t Class_Serialplot_UART::Get_Data_Type_Size() const
{
    switch (Tx_Data_Type)
    {
    case Serialplot_Data_Type_UINT8:
    case Serialplot_Data_Type_INT8:
        return sizeof(uint8_t);

    case Serialplot_Data_Type_UINT16:
    case Serialplot_Data_Type_INT16:
        return sizeof(uint16_t);

    case Serialplot_Data_Type_UINT32:
    case Serialplot_Data_Type_INT32:
    case Serialplot_Data_Type_FLOAT:
        return sizeof(uint32_t);

    case Serialplot_Data_Type_DOUBLE:
        return sizeof(uint64_t);

    default:
        return 0;
    }
}

/*----------------------------------------------------------------------------*/

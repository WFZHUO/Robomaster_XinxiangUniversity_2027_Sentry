/**
 * @file dvc_vofa.cpp
 * @author WangFonzhuo
 * @brief Vofa+绘图调参工具, 使用自定义协议, 带8位累加和校验
 *        单片机发送格式是：0xAB + float0 + float1 + ... + floatN + 0x7F800000 + sum8
 *        sum8 = 从 0xAB 到 0x7F800000 最后一个字节的 8 位累加和
 *        VOFA+ 端必须用自定义协议RMFloatSum8.dll 协议引擎解析
 * @version 1.0
 * @date 2026-05-12 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "dvc_vofa.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// Vofa+发送缓冲区, 放入DMA可访问区域
__attribute__((section(".dma_buffer"))) static uint8_t VofaSum8_UART_Tx_Buffer[VOFA_SUM8_TX_BUFFER_MAX_SIZE];
__attribute__((section(".dma_buffer"))) static uint8_t VofaSum8_USB_Tx_Buffer[VOFA_SUM8_TX_BUFFER_MAX_SIZE];

/* Function prototypes -------------------------------------------------------*/

static uint8_t VofaSum8_Sum_8(const uint8_t *Address, uint32_t Length);
static float VofaSum8_Parse_Float(const uint8_t *Data, uint16_t Start, uint16_t End);

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 8位累加和校验
 */
static uint8_t VofaSum8_Sum_8(const uint8_t *Address, uint32_t Length)
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
static float VofaSum8_Parse_Float(const uint8_t *Data, uint16_t Start, uint16_t End)
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
 * @brief Vofa+ UART初始化
 * @param huart UART句柄
 * @param __Rx_Variable_Assignment_Num 接收变量分配数量
 * @param __Rx_Variable_Assignment_List 接收变量分配列表
 * @param __Frame_Header 帧头
 * @param __Frame_Tail justfloat帧尾
 */
void Class_VofaSum8_UART::Init(const UART_HandleTypeDef *huart,
                               const uint8_t &__Rx_Variable_Assignment_Num,
                               const char **__Rx_Variable_Assignment_List,
                               const uint8_t &__Frame_Header,
                               const uint32_t &__Frame_Tail)
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

    Rx_Variable_Num = __Rx_Variable_Assignment_Num;
    Rx_Variable_List = __Rx_Variable_Assignment_List;
    Frame_Header = __Frame_Header;
    Frame_Tail = __Frame_Tail;

    Tx_Buffer = VofaSum8_UART_Tx_Buffer;
}

/**
 * @brief UART接收完成回调函数
 * @param Rx_Data 接收到的数据
 * @param Length 接收到的数据长度
 */
void Class_VofaSum8_UART::UART_RxCpltCallback(const uint8_t *Rx_Data, uint16_t Length)
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
void Class_VofaSum8_UART::TIM_1ms_Write_PeriodElapsedCallback()
{
    if (UART_Manage_Object == nullptr || UART_Manage_Object->UART_Handler == nullptr || Tx_Buffer == nullptr)
    {
        return;
    }

    //防止出现DMA正在读取上一帧, 但是该帧又被Output覆盖了数据的情况
    if (UART_Manage_Object->UART_Handler->gState != HAL_UART_STATE_READY)
    {
        return;
    }

    Output();

    UART_Transmit_Data(UART_Manage_Object->UART_Handler, Tx_Buffer, Get_Tx_Length());
}

/**
 * @brief 数据处理函数
 * @param Rx_Data 接收到的数据
 * @param Length 数据长度
 */
void Class_VofaSum8_UART::Data_Process(const uint8_t *Rx_Data, uint16_t Length)
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
int32_t Class_VofaSum8_UART::Judge_Variable_Name(const uint8_t *Rx_Data,
                                                 uint16_t Length,
                                                 uint16_t *value_start_index)
{
    if (Rx_Data == nullptr || Length == 0 || value_start_index == nullptr ||
        Rx_Variable_List == nullptr || Rx_Variable_Num == 0)
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

    char tmp_variable_name[VOFA_SUM8_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH] = {0};
    uint16_t copy_len = equal_index;

    if (copy_len >= VOFA_SUM8_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH)
    {
        copy_len = VOFA_SUM8_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH - 1;
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
void Class_VofaSum8_UART::Judge_Variable_Value(const uint8_t *Rx_Data,
                                               uint16_t Length,
                                               uint16_t value_start_index)
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

    Variable_Value = VofaSum8_Parse_Float(Rx_Data, value_start_index, end_index);
}

/**
 * @brief 输出数据
 */
void Class_VofaSum8_UART::Output()
{
    uint16_t tx_length = Get_Tx_Length();

    memset(Tx_Buffer, 0, tx_length);

    Tx_Buffer[0] = Frame_Header;

    for (uint8_t i = 0; i < Data_Number; i++)
    {
        if (Data[i] == nullptr)
        {
            continue;
        }

        memcpy(Tx_Buffer + 1 + i * sizeof(float), Data[i], sizeof(float));
    }

    uint16_t tail_index = 1 + Data_Number * sizeof(float);
    memcpy(Tx_Buffer + tail_index, &Frame_Tail, sizeof(uint32_t));

    Tx_Buffer[tx_length - 1] = VofaSum8_Sum_8(Tx_Buffer, tx_length - 1);
}

/**
 * @brief Vofa+ USB初始化
 * @param __Rx_Variable_Assignment_Num 接收变量分配数量
 * @param __Rx_Variable_Assignment_List 接收变量分配列表
 * @param __Frame_Header 帧头
 * @param __Frame_Tail justfloat帧尾
 */
void Class_VofaSum8_USB::Init(const uint8_t &__Rx_Variable_Assignment_Num,
                              const char **__Rx_Variable_Assignment_List,
                              const uint8_t &__Frame_Header,
                              const uint32_t &__Frame_Tail)
{
    Rx_Variable_Num = __Rx_Variable_Assignment_Num;
    Rx_Variable_List = __Rx_Variable_Assignment_List;
    Frame_Header = __Frame_Header;
    Frame_Tail = __Frame_Tail;

    Tx_Buffer = VofaSum8_USB_Tx_Buffer;
}

/**
 * @brief USB接收完成回调函数
 * @param Rx_Data 接收到的数据
 * @param Length 接收到的数据长度
 */
void Class_VofaSum8_USB::USB_RxCpltCallback(const uint8_t *Rx_Data, uint16_t Length)
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
void Class_VofaSum8_USB::TIM_1ms_Write_PeriodElapsedCallback()
{
    if (Tx_Buffer == nullptr)
    {
        return;
    }

    Output();

    USB_Transmit_Data(Tx_Buffer, Get_Tx_Length());
}

/**
 * @brief 数据处理函数
 * @param Rx_Data 接收到的数据
 * @param Length 数据长度
 */
void Class_VofaSum8_USB::Data_Process(const uint8_t *Rx_Data, uint16_t Length)
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
int32_t Class_VofaSum8_USB::Judge_Variable_Name(const uint8_t *Rx_Data,
                                                uint16_t Length,
                                                uint16_t *value_start_index)
{
    if (Rx_Data == nullptr || Length == 0 || value_start_index == nullptr ||
        Rx_Variable_List == nullptr || Rx_Variable_Num == 0)
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

    char tmp_variable_name[VOFA_SUM8_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH] = {0};
    uint16_t copy_len = equal_index;

    if (copy_len >= VOFA_SUM8_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH)
    {
        copy_len = VOFA_SUM8_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH - 1;
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
void Class_VofaSum8_USB::Judge_Variable_Value(const uint8_t *Rx_Data,
                                              uint16_t Length,
                                              uint16_t value_start_index)
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

    Variable_Value = VofaSum8_Parse_Float(Rx_Data, value_start_index, end_index);
}

/**
 * @brief 输出数据
 */
void Class_VofaSum8_USB::Output()
{
    uint16_t tx_length = Get_Tx_Length();

    memset(Tx_Buffer, 0, tx_length);

    Tx_Buffer[0] = Frame_Header;

    for (uint8_t i = 0; i < Data_Number; i++)
    {
        if (Data[i] == nullptr)
        {
            continue;
        }

        memcpy(Tx_Buffer + 1 + i * sizeof(float), Data[i], sizeof(float));
    }

    uint16_t tail_index = 1 + Data_Number * sizeof(float);
    memcpy(Tx_Buffer + tail_index, &Frame_Tail, sizeof(uint32_t));

    Tx_Buffer[tx_length - 1] = VofaSum8_Sum_8(Tx_Buffer, tx_length - 1);
}

/*----------------------------------------------------------------------------*/

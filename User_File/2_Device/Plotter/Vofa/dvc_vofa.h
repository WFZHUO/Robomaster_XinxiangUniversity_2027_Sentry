/**
 * @file dvc_vofa.h
 * @author WangFonzhuo
 * @brief Vofa+绘图调参工具, 使用自定义协议, 带8位累加和校验
 *        单片机发送格式是：0xAB + float0 + float1 + ... + floatN + 0x7F800000 + sum8
 *        sum8 = 从 0xAB 到 0x7F800000 最后一个字节的 8 位累加和
 *        VOFA+ 端必须用自定义协议RMFloatSum8.dll 协议引擎解析
 * @version 1.0
 * @date 2026-05-12 27赛季
 */

#ifndef DVC_VOFA_H
#define DVC_VOFA_H

/* Includes ------------------------------------------------------------------*/

#include "drv_uart.h"
#include "drv_usb.h"
#include <cstdarg>
#include <cstring>

/* Exported macros -----------------------------------------------------------*/

#define VOFA_SUM8_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH (100)
#define VOFA_SUM8_TX_DATA_MAX_NUM                   (24)

#define VOFA_SUM8_FRAME_HEADER                      (0xab)
#define VOFA_SUM8_FRAME_TAIL_JUSTFLOAT              (0x7f800000U)

#define VOFA_SUM8_TX_BUFFER_MAX_SIZE                    (1 + VOFA_SUM8_TX_DATA_MAX_NUM * sizeof(float) + sizeof(uint32_t) + 1)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief UART Vofa+绘图工具, 带8位累加和校验
 */
class Class_VofaSum8_UART
{
public:
    /**
     * @brief Vofa+ UART初始化
     * @param huart UART句柄
     * @param __Rx_Variable_Assignment_Num 接收变量分配数量
     * @param __Rx_Variable_Assignment_List 接收变量分配列表
     * @param __Frame_Header 帧头
     * @param __Frame_Tail justfloat帧尾
     */
    void Init(const UART_HandleTypeDef *huart,
              const uint8_t &__Rx_Variable_Assignment_Num = 0,
              const char **__Rx_Variable_Assignment_List = nullptr,
              const uint8_t &__Frame_Header = VOFA_SUM8_FRAME_HEADER,
              const uint32_t &__Frame_Tail = VOFA_SUM8_FRAME_TAIL_JUSTFLOAT);

    /**
     * @brief 获取当前接收指令在字典中的编号
     * @return 指令编号, -1表示未匹配
     */
    inline int32_t Get_Variable_Index() const;

    /**
     * @brief 获取当前接收指令的值
     * @return 指令值
     */
    inline float Get_Variable_Value() const;

    /**
     * @brief 绑定需要发送的变量地址
     * @param Number 变量数量
     * @param ... 变量地址列表, 变量类型应为float
     */
    inline void Set_Data(int Number, ...);

    /**
     * @brief UART接收完成回调函数, 需要在对应UART接收回调里调用
     * @param Rx_Data 接收到的数据
     * @param Length 接收到的数据长度
     */
    void UART_RxCpltCallback(const uint8_t *Rx_Data, uint16_t Length);

    /**
     * @brief 1ms定时器周期发送函数
     */
    void TIM_1ms_Write_PeriodElapsedCallback();

protected:
    // 绑定的UART管理对象
    Struct_UART_Manage_Object *UART_Manage_Object = nullptr;

    // 接收指令字典
    uint8_t Rx_Variable_Num = 0;
    const char **Rx_Variable_List = nullptr;

    // 帧头
    uint8_t Frame_Header = VOFA_SUM8_FRAME_HEADER;

    // justfloat帧尾
    uint32_t Frame_Tail = VOFA_SUM8_FRAME_TAIL_JUSTFLOAT;

    // 发送缓冲区
    uint8_t *Tx_Buffer = nullptr;

    // 需要绘图的变量地址
    const void *Data[VOFA_SUM8_TX_DATA_MAX_NUM] = {nullptr};

    // 当前发送通道数量
    uint8_t Data_Number = 0;

    // 当前接收指令在字典中的编号, -1表示未匹配
    int32_t Variable_Index = -1;

    // 当前接收指令的值
    float Variable_Value = 0.0f;

    // 数据处理函数
    void Data_Process(const uint8_t *Rx_Data, uint16_t Length);

    // 判断接收指令名称
    int32_t Judge_Variable_Name(const uint8_t *Rx_Data, uint16_t Length, uint16_t *value_start_index);

    // 判断接收指令值
    void Judge_Variable_Value(const uint8_t *Rx_Data, uint16_t Length, uint16_t value_start_index);

    // 输出数据
    void Output();

    // 获取发送数据长度
    inline uint16_t Get_Tx_Length() const;
};

/**
 * @brief USB Vofa+绘图工具, 带8位累加和校验
 */
class Class_VofaSum8_USB
{
public:
    /**
     * @brief Vofa+ USB初始化
     * @param __Rx_Variable_Assignment_Num 接收变量分配数量
     * @param __Rx_Variable_Assignment_List 接收变量分配列表
     * @param __Frame_Header 帧头
     * @param __Frame_Tail justfloat帧尾
     */
    void Init(const uint8_t &__Rx_Variable_Assignment_Num = 0,
              const char **__Rx_Variable_Assignment_List = nullptr,
              const uint8_t &__Frame_Header = VOFA_SUM8_FRAME_HEADER,
              const uint32_t &__Frame_Tail = VOFA_SUM8_FRAME_TAIL_JUSTFLOAT);

    /**
     * @brief 获取当前接收指令在字典中的编号
     * @return 指令编号, -1表示未匹配
     */
    inline int32_t Get_Variable_Index() const;

    /**
     * @brief 获取当前接收指令的值
     * @return 指令值
     */
    inline float Get_Variable_Value() const;

    /**
     * @brief 绑定需要发送的变量地址
     * @param Number 变量数量
     * @param ... 变量地址列表, 变量类型应为float
     */
    inline void Set_Data(int Number, ...);

    /**
     * @brief USB接收完成回调函数, 需要在USB接收回调里调用
     * @param Rx_Data 接收到的数据
     * @param Length 接收到的数据长度
     */
    void USB_RxCpltCallback(const uint8_t *Rx_Data, uint16_t Length);

    /**
     * @brief 1ms定时器周期发送函数
     */
    void TIM_1ms_Write_PeriodElapsedCallback();

protected:
    // 接收指令字典
    uint8_t Rx_Variable_Num = 0;
    const char **Rx_Variable_List = nullptr;

    // 帧头
    uint8_t Frame_Header = VOFA_SUM8_FRAME_HEADER;

    // justfloat帧尾
    uint32_t Frame_Tail = VOFA_SUM8_FRAME_TAIL_JUSTFLOAT;

    // 发送缓冲区
    uint8_t *Tx_Buffer = nullptr;

    // 需要绘图的变量地址
    const void *Data[VOFA_SUM8_TX_DATA_MAX_NUM] = {nullptr};

    // 当前发送通道数量
    uint8_t Data_Number = 0;

    // 当前接收指令在字典中的编号, -1表示未匹配
    int32_t Variable_Index = -1;

    // 当前接收指令的值
    float Variable_Value = 0.0f;

    // 数据处理函数
    void Data_Process(const uint8_t *Rx_Data, uint16_t Length);

    // 判断接收指令名称
    int32_t Judge_Variable_Name(const uint8_t *Rx_Data, uint16_t Length, uint16_t *value_start_index);

    // 判断接收指令值
    void Judge_Variable_Value(const uint8_t *Rx_Data, uint16_t Length, uint16_t value_start_index);

    // 输出数据
    void Output();

    // 获取发送数据长度
    inline uint16_t Get_Tx_Length() const;
};

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 获取当前接收指令在字典中的编号
 * @return 指令编号, -1表示未匹配
 */
inline int32_t Class_VofaSum8_UART::Get_Variable_Index() const
{
    return Variable_Index;
}

/**
 * @brief 获取当前接收指令的值
 * @return 指令值
 */
inline float Class_VofaSum8_UART::Get_Variable_Value() const
{
    return Variable_Value;
}

/**
 * @brief 获取发送数据长度
 * @return 数据长度
 */
inline uint16_t Class_VofaSum8_UART::Get_Tx_Length() const
{
    return 1 + Data_Number * sizeof(float) + sizeof(uint32_t) + 1;
}

/**
 * @brief 绑定需要发送的变量地址
 * @param Number 变量数量
 * @param ... 变量地址列表, 变量类型应为float
 * @note 这里保存的是变量地址, 不是变量当前值。
 *       后续每次调用TIM_1ms_Write_PeriodElapsedCallback时, 才会读取这些地址里的最新值。
 */
inline void Class_VofaSum8_UART::Set_Data(int Number, ...)
{
    if (Number < 0)
    {
        Number = 0;
    }
    else if (Number > VOFA_SUM8_TX_DATA_MAX_NUM)
    {
        Number = VOFA_SUM8_TX_DATA_MAX_NUM;
    }

    va_list data_ptr;
    va_start(data_ptr, Number);

    for (int i = 0; i < Number; i++)
    {
        Data[i] = va_arg(data_ptr, const void *);
    }

    va_end(data_ptr);

    Data_Number = static_cast<uint8_t>(Number);
}

/**
 * @brief 获取当前接收指令在字典中的编号
 * @return 指令编号, -1表示未匹配
 */
inline int32_t Class_VofaSum8_USB::Get_Variable_Index() const
{
    return Variable_Index;
}

/**
 * @brief 获取当前接收指令的值
 * @return 指令值
 */
inline float Class_VofaSum8_USB::Get_Variable_Value() const
{
    return Variable_Value;
}

/**
 * @brief 获取发送数据长度
 * @return 数据长度
 */
inline uint16_t Class_VofaSum8_USB::Get_Tx_Length() const
{
    return 1 + Data_Number * sizeof(float) + sizeof(uint32_t) + 1;
}

/**
 * @brief 绑定需要发送的变量地址
 * @param Number 变量数量
 * @param ... 变量地址列表, 变量类型应为float
 * @note 这里保存的是变量地址, 不是变量当前值。
 *       后续每次调用TIM_1ms_Write_PeriodElapsedCallback时, 才会读取这些地址里的最新值。
 */
inline void Class_VofaSum8_USB::Set_Data(int Number, ...)
{
    if (Number < 0)
    {
        Number = 0;
    }
    else if (Number > VOFA_SUM8_TX_DATA_MAX_NUM)
    {
        Number = VOFA_SUM8_TX_DATA_MAX_NUM;
    }

    va_list data_ptr;
    va_start(data_ptr, Number);

    for (int i = 0; i < Number; i++)
    {
        Data[i] = va_arg(data_ptr, const void *);
    }

    va_end(data_ptr);

    Data_Number = static_cast<uint8_t>(Number);
}

#endif /* DVC_VOFA_H */

/*----------------------------------------------------------------------------*/

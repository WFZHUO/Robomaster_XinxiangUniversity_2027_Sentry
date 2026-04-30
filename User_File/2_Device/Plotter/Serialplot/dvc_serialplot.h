/**
 * @file dvc_serialplot.h
 * @author WangFonzhuo
 * @brief Serialplot串口绘图
 * @version 1.0
 * @date 2026-04-25 27赛季
 */

#ifndef DVC_SERIALPLOT_H
#define DVC_SERIALPLOT_H

/* Includes ------------------------------------------------------------------*/

#include "drv_uart.h"
#include <cstdarg>
#include <cstring>

/* Exported macros -----------------------------------------------------------*/

#define SERIALPLOT_RX_VARIABLE_ASSIGNMENT_MAX_LENGTH (100)
#define SERIALPLOT_TX_DATA_MAX_NUM                   (24)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 是否开启8位累加和校验
 */
enum Enum_Serialplot_Checksum_8
{
    Serialplot_Checksum_8_DISABLE = 0,
    Serialplot_Checksum_8_ENABLE,
};

/**
 * @brief 串口绘图传输数据类型
 */
enum Enum_Serialplot_Data_Type
{
    Serialplot_Data_Type_UINT8 = 0,
    Serialplot_Data_Type_UINT16,
    Serialplot_Data_Type_UINT32,
    Serialplot_Data_Type_INT8,
    Serialplot_Data_Type_INT16,
    Serialplot_Data_Type_INT32,
    Serialplot_Data_Type_FLOAT,
    Serialplot_Data_Type_DOUBLE,
};

/**
 * @brief UART串口绘图工具, 最多支持24个通道
 */
class Class_Serialplot_UART
{
public:
    /**
     * @brief 串口绘图初始化
     */
    void Init(const UART_HandleTypeDef *huart,
              const Enum_Serialplot_Checksum_8 &__Checksum_8 = Serialplot_Checksum_8_ENABLE,
              const uint8_t &__Rx_Variable_Assignment_Num = 0,
              const char **__Rx_Variable_Assignment_List = nullptr,
              const Enum_Serialplot_Data_Type &__Data_Type = Serialplot_Data_Type_FLOAT,
              const uint8_t &__Frame_Header = 0xab);

    /**
     * @brief 获取当前接收指令在字典中的编号
     * @return 指令编号
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
     * @param ... 变量地址列表
     */
    inline void Set_Data(int Number, ...);

    /**
     * @brief UART接收完成回调函数, 需要在对应的UART接收中断回调里调用
     * @param Rx_Data 接收到的数据
     * @param Length 接收到的数据长度
     */
    void UART_RxCpltCallback(const uint8_t *Rx_Data, uint16_t Length);

    /**
     * @brief 1ms定时器周期发送函数, 需要在对应的1ms定时器回调里调用
     */
    void TIM_1ms_Write_PeriodElapsedCallback();

protected:
    // 绑定的UART管理对象
    Struct_UART_Manage_Object *UART_Manage_Object = nullptr;

    // 是否开启8位累加和校验
    Enum_Serialplot_Checksum_8 Checksum_8 = Serialplot_Checksum_8_ENABLE;

    // 接收指令字典
    uint8_t Rx_Variable_Num = 0;
    const char **Rx_Variable_List = nullptr;

    // 发送数据类型
    Enum_Serialplot_Data_Type Tx_Data_Type = Serialplot_Data_Type_FLOAT;

    // 数据包帧头
    uint8_t Frame_Header = 0xab;

    // 发送缓冲区
    uint8_t *Tx_Buffer = nullptr;

    // 需要绘图的变量地址
    const void *Data[SERIALPLOT_TX_DATA_MAX_NUM] = {nullptr};

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

    // 计算校验和
    void Output();

    // 输出数据
    uint16_t Get_Data_Type_Size() const;
};

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 获取当前接收指令在字典中的编号
 * @return 指令编号
 */
inline int32_t Class_Serialplot_UART::Get_Variable_Index() const
{
    return Variable_Index;
}

/**
 * @brief 获取当前接收指令的值
 * @return 指令值
 */
inline float Class_Serialplot_UART::Get_Variable_Value() const
{
    return Variable_Value;
}

/**
 * @brief 绑定需要发送的变量地址
 * @note 这里保存的是变量地址, 不是变量当前值。
 *       后续每次调用TIM_1ms_Write_PeriodElapsedCallback时, 才会读取这些地址里的最新值。
 */
inline void Class_Serialplot_UART::Set_Data(int Number, ...)
{
    if (Number < 0)
    {
        Number = 0;
    }
    else if (Number > SERIALPLOT_TX_DATA_MAX_NUM)
    {
        Number = SERIALPLOT_TX_DATA_MAX_NUM;
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

#endif /* DVC_SERIALPLOT_H */

/*----------------------------------------------------------------------------*/

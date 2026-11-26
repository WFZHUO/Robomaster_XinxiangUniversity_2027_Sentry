/**
 * @file dvc_dr16.h
 * @author WangFongzhuo
 * @brief DR16遥控器DBUS协议解析与在线状态管理
 * @version 1.0
 * @date 2026-11-22 27赛季
 */

#ifndef DVC_DR16_H
#define DVC_DR16_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

// DR16遥控器DBUS协议数据帧长度, 单位: 字节
#define DR16_RX_FRAME_LENGTH (18U)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 遥控器在线状态
 */
enum Enum_DR16_Status
{
    DR16_Status_DISABLE = 0,
    DR16_Status_ENABLE,
};

/**
 * @brief 拨杆状态, TRIG表示本帧发生了位置变化
 */
enum Enum_DR16_Switch_Status
{
    DR16_Switch_Status_UNDEFINED = 0,
    DR16_Switch_Status_UP,
    DR16_Switch_Status_TRIG_UP_MIDDLE,  // 刚从上档拨到中档
    DR16_Switch_Status_TRIG_MIDDLE_UP,  // 刚从中档拨到上档
    DR16_Switch_Status_MIDDLE,
    DR16_Switch_Status_TRIG_MIDDLE_DOWN,
    DR16_Switch_Status_TRIG_DOWN_MIDDLE,
    DR16_Switch_Status_DOWN,
};

/**
 * @brief 鼠标或键盘按键状态
 */
enum Enum_DR16_Key_Status
{
    DR16_Key_Status_FREE = 0,
    DR16_Key_Status_TRIG_FREE_PRESSED,
    DR16_Key_Status_TRIG_PRESSED_FREE,
    DR16_Key_Status_PRESSED,
};

/**
 * @brief 键盘按键编号, 与DBUS键盘位图对应
 */
enum Enum_DR16_Keyboard_Key
{
    DR16_Key_W = 0,
    DR16_Key_S,
    DR16_Key_A,
    DR16_Key_D,
    DR16_Key_SHIFT,
    DR16_Key_CTRL,
    DR16_Key_Q,
    DR16_Key_E,
    DR16_Key_R,
    DR16_Key_F,
    DR16_Key_G,
    DR16_Key_Z,
    DR16_Key_X,
    DR16_Key_C,
    DR16_Key_V,
    DR16_Key_B,
};

/**
 * @brief 解码后的DBUS源数据
 */
struct Struct_DR16_UART_Data
{
    uint16_t Channel_0;
    uint16_t Channel_1;
    uint16_t Channel_2;
    uint16_t Channel_3;
    uint16_t Channel_Yaw;
    uint8_t Switch_1;
    uint8_t Switch_2;
    int16_t Mouse_X;
    int16_t Mouse_Y;
    int16_t Mouse_Z;
    uint8_t Mouse_Left_Key;
    uint8_t Mouse_Right_Key;
    uint16_t Keyboard_Key;
};

/**
 * @brief 处理后的遥控器数据, 摇杆和拨轮归一化至[-1, 1]
 */
struct Struct_DR16_Data
{
    float Right_X;
    float Right_Y;
    float Left_X;
    float Left_Y;
    float Yaw;
    Enum_DR16_Switch_Status Left_Switch;
    Enum_DR16_Switch_Status Right_Switch;
    float Mouse_X;
    float Mouse_Y;
    float Mouse_Z;
    Enum_DR16_Key_Status Mouse_Left_Key;
    Enum_DR16_Key_Status Mouse_Right_Key;
    Enum_DR16_Key_Status Keyboard_Key[16];
};

/**
 * @brief DR16遥控器
 * @note 只有Get_Status()==DR16_Status_ENABLE时才允许上层使用控制数据
 */
class Class_DR16
{
public:
    void Init();

    inline Enum_DR16_Status Get_Status() const;
    inline float Get_Right_X() const;
    inline float Get_Right_Y() const;
    inline float Get_Left_X() const;
    inline float Get_Left_Y() const;
    inline float Get_Yaw() const;
    inline Enum_DR16_Switch_Status Get_Left_Switch() const;
    inline Enum_DR16_Switch_Status Get_Right_Switch() const;
    inline float Get_Mouse_X() const;
    inline float Get_Mouse_Y() const;
    inline float Get_Mouse_Z() const;
    inline Enum_DR16_Key_Status Get_Mouse_Left_Key() const;
    inline Enum_DR16_Key_Status Get_Mouse_Right_Key() const;
    inline Enum_DR16_Key_Status Get_Keyboard_Key(Enum_DR16_Keyboard_Key Key) const;

    /**
     * @brief UART接收完成回调, 传入drv_uart实际收到的缓冲区
     */
    void UART_RxCpltCallback(uint8_t *Rx_Data, uint16_t Length);

    /**
     * @brief 每1ms调用一次, 连续50ms无有效帧即掉线
     */
    void TIM_1ms_Alive_PeriodElapsedCallback();

protected:
    // 摇杆/拨轮的物理范围为364~1684, 中值1024
    const float ROCKER_OFFSET = 1024.0f;
    const float ROCKER_NUM = 660.0f;

    // 从上一帧有效数据开始的失联计时, 单位ms
    volatile uint16_t Offline_Time = 50U;

    volatile Enum_DR16_Status DR16_Status = DR16_Status_DISABLE;
    Struct_DR16_UART_Data Pre_UART_Rx_Data = {};
    Struct_DR16_Data Data = {};

    bool Data_Process(const uint8_t *Rx_Data, Struct_DR16_UART_Data &Raw_Data);
    Enum_DR16_Switch_Status Judge_Switch(uint8_t Status, uint8_t Pre_Status, bool Online);
    Enum_DR16_Key_Status Judge_Key(uint8_t Status, uint8_t Pre_Status, bool Online);
    void Reset_Data_To_Safe_State();
};

/* Exported function definitions ---------------------------------------------*/

inline Enum_DR16_Status Class_DR16::Get_Status() const { return DR16_Status; }
inline float Class_DR16::Get_Right_X() const { return Data.Right_X; }
inline float Class_DR16::Get_Right_Y() const { return Data.Right_Y; }
inline float Class_DR16::Get_Left_X() const { return Data.Left_X; }
inline float Class_DR16::Get_Left_Y() const { return Data.Left_Y; }
inline float Class_DR16::Get_Yaw() const { return Data.Yaw; }
inline Enum_DR16_Switch_Status Class_DR16::Get_Left_Switch() const { return Data.Left_Switch; }
inline Enum_DR16_Switch_Status Class_DR16::Get_Right_Switch() const { return Data.Right_Switch; }
inline float Class_DR16::Get_Mouse_X() const { return Data.Mouse_X; }
inline float Class_DR16::Get_Mouse_Y() const { return Data.Mouse_Y; }
inline float Class_DR16::Get_Mouse_Z() const { return Data.Mouse_Z; }
inline Enum_DR16_Key_Status Class_DR16::Get_Mouse_Left_Key() const { return Data.Mouse_Left_Key; }
inline Enum_DR16_Key_Status Class_DR16::Get_Mouse_Right_Key() const { return Data.Mouse_Right_Key; }
inline Enum_DR16_Key_Status Class_DR16::Get_Keyboard_Key(Enum_DR16_Keyboard_Key Key) const { return Data.Keyboard_Key[Key]; }

#endif /* DVC_DR16_H */

/*----------------------------------------------------------------------------*/

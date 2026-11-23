/**
 * @file dvc_dr16.cpp
 * @author WangFongzhuo
 * @brief DR16遥控器DBUS协议解析与在线状态管理
 * @version 1.0
 * @date 2026-11-22 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "dvc_dr16.h"

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 初始化遥控器
 */
void Class_DR16::Init()
{
    Offline_Time = 50U;
    Reset_Data_To_Safe_State();
}

/**
 * @brief 解析并校验18字节DBUS数据
 * @note 校验摇杆、拨轮、拨杆和鼠标按键后才允许整帧更新
 */
bool Class_DR16::Data_Process(const uint8_t *Rx_Data, Struct_DR16_UART_Data &Raw_Data)
{
    Raw_Data.Channel_0 = (Rx_Data[0] | (Rx_Data[1] << 8)) & 0x07FFU;
    Raw_Data.Channel_1 = ((Rx_Data[1] >> 3) | (Rx_Data[2] << 5)) & 0x07FFU;
    Raw_Data.Channel_2 = ((Rx_Data[2] >> 6) | (Rx_Data[3] << 2) | (Rx_Data[4] << 10)) & 0x07FFU;
    Raw_Data.Channel_3 = ((Rx_Data[4] >> 1) | (Rx_Data[5] << 7)) & 0x07FFU;
    Raw_Data.Channel_Yaw = (Rx_Data[16] | (Rx_Data[17] << 8)) & 0x07FFU;

    Raw_Data.Switch_1 = (Rx_Data[5] >> 6) & 0x03U;
    Raw_Data.Switch_2 = (Rx_Data[5] >> 4) & 0x03U;

    Raw_Data.Mouse_X = static_cast<int16_t>(Rx_Data[6] | (Rx_Data[7] << 8));
    Raw_Data.Mouse_Y = static_cast<int16_t>(Rx_Data[8] | (Rx_Data[9] << 8));
    Raw_Data.Mouse_Z = static_cast<int16_t>(Rx_Data[10] | (Rx_Data[11] << 8));
    Raw_Data.Mouse_Left_Key = Rx_Data[12];
    Raw_Data.Mouse_Right_Key = Rx_Data[13];
    Raw_Data.Keyboard_Key = Rx_Data[14] | (Rx_Data[15] << 8);

    if (Raw_Data.Channel_0 < 364U || Raw_Data.Channel_0 > 1684U ||
        Raw_Data.Channel_1 < 364U || Raw_Data.Channel_1 > 1684U ||
        Raw_Data.Channel_2 < 364U || Raw_Data.Channel_2 > 1684U ||
        Raw_Data.Channel_3 < 364U || Raw_Data.Channel_3 > 1684U ||
        Raw_Data.Channel_Yaw < 364U || Raw_Data.Channel_Yaw > 1684U)
    {
        return false;
    }

    if (Raw_Data.Switch_1 < 1U || Raw_Data.Switch_1 > 3U ||
        Raw_Data.Switch_2 < 1U || Raw_Data.Switch_2 > 3U ||
        Raw_Data.Mouse_Left_Key > 1U || Raw_Data.Mouse_Right_Key > 1U)
    {
        return false;
    }

    return true;
}

/**
 * @brief 判断拨杆位置和切换沿
 */
Enum_DR16_Switch_Status Class_DR16::Judge_Switch(uint8_t Status, uint8_t Pre_Status, bool Online)
{
    if (Online)
    {
        if (Pre_Status == 1U && Status == 3U)
        {
            return DR16_Switch_Status_TRIG_UP_MIDDLE;
        }
        if (Pre_Status == 3U && Status == 1U)
        {
            return DR16_Switch_Status_TRIG_MIDDLE_UP;
        }
        if (Pre_Status == 3U && Status == 2U)
        {
            return DR16_Switch_Status_TRIG_MIDDLE_DOWN;
        }
        if (Pre_Status == 2U && Status == 3U)
        {
            return DR16_Switch_Status_TRIG_DOWN_MIDDLE;
        }
        // 快速跨越中间位置时, 采用接近目标位置的触发事件
        if (Pre_Status == 1U && Status == 2U)
        {
            return DR16_Switch_Status_TRIG_MIDDLE_DOWN;
        }
        if (Pre_Status == 2U && Status == 1U)
        {
            return DR16_Switch_Status_TRIG_MIDDLE_UP;
        }
    }

    if (Status == 1U)
    {
        return DR16_Switch_Status_UP;
    }
    if (Status == 2U)
    {
        return DR16_Switch_Status_DOWN;
    }
    else
    {
        return DR16_Switch_Status_MIDDLE;
    }
}

/**
 * @brief 判断按键按下与释放沿
 */
Enum_DR16_Key_Status Class_DR16::Judge_Key(uint8_t Status, uint8_t Pre_Status, bool Online)
{
    if (Online && Status != Pre_Status)
    {
        return Status ? DR16_Key_Status_TRIG_FREE_PRESSED : DR16_Key_Status_TRIG_PRESSED_FREE;
    }

    return Status ? DR16_Key_Status_PRESSED : DR16_Key_Status_FREE;
}

/**
 * @brief UART通信接收完成回调函数
 */
void Class_DR16::UART_RxCpltCallback(uint8_t *Rx_Data, uint16_t Length)
{
    Struct_DR16_UART_Data raw_data = {};

    if (Length != DR16_RX_FRAME_LENGTH || !Data_Process(Rx_Data, raw_data))
    {
        // 无效帧立即使控制数据失效, 不让旧指令继续生效
        Offline_Time = 50U;
        Reset_Data_To_Safe_State();
        return;
    }

    bool online = (DR16_Status == DR16_Status_ENABLE);
    Struct_DR16_Data temp_data = {};

    temp_data.Right_X = (raw_data.Channel_0 - ROCKER_OFFSET) / ROCKER_NUM;
    temp_data.Right_Y = (raw_data.Channel_1 - ROCKER_OFFSET) / ROCKER_NUM;
    temp_data.Left_X = (raw_data.Channel_2 - ROCKER_OFFSET) / ROCKER_NUM;
    temp_data.Left_Y = (raw_data.Channel_3 - ROCKER_OFFSET) / ROCKER_NUM;
    temp_data.Yaw = (raw_data.Channel_Yaw - ROCKER_OFFSET) / ROCKER_NUM;

    temp_data.Left_Switch = Judge_Switch(raw_data.Switch_1, Pre_UART_Rx_Data.Switch_1, online);
    temp_data.Right_Switch = Judge_Switch(raw_data.Switch_2, Pre_UART_Rx_Data.Switch_2, online);

    temp_data.Mouse_X = raw_data.Mouse_X / 32768.0f;
    temp_data.Mouse_Y = raw_data.Mouse_Y / 32768.0f;
    temp_data.Mouse_Z = raw_data.Mouse_Z / 32768.0f;
    temp_data.Mouse_Left_Key = Judge_Key(raw_data.Mouse_Left_Key, Pre_UART_Rx_Data.Mouse_Left_Key, online);
    temp_data.Mouse_Right_Key = Judge_Key(raw_data.Mouse_Right_Key, Pre_UART_Rx_Data.Mouse_Right_Key, online);

    for (uint8_t i = 0; i < 16U; i++)
    {
        uint8_t key = (raw_data.Keyboard_Key >> i) & 0x01U;
        uint8_t pre_key = (Pre_UART_Rx_Data.Keyboard_Key >> i) & 0x01U;
        temp_data.Keyboard_Key[i] = Judge_Key(key, pre_key, online);
    }

    // 数据全部校验并计算完成后, 再提交对外可见的状态
    Data = temp_data;
    Pre_UART_Rx_Data = raw_data;
    Offline_Time = 0U;
    DR16_Status = DR16_Status_ENABLE;
}

/**
 * @brief 每1ms更新一次失联计时, 50ms未收到有效帧则掉线
 */
void Class_DR16::TIM_1ms_Alive_PeriodElapsedCallback()
{
    if (Offline_Time < 50U)
    {
        Offline_Time++;
    }

    if (Offline_Time >= 50U && DR16_Status == DR16_Status_ENABLE)
    {
        Reset_Data_To_Safe_State();
    }
}

/**
 * @brief 清空失联后的控制输入
 */
void Class_DR16::Reset_Data_To_Safe_State()
{
    DR16_Status = DR16_Status_DISABLE;
    Data = {};
    Pre_UART_Rx_Data = {};
}

/*----------------------------------------------------------------------------*/

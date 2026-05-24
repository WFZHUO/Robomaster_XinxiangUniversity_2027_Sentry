/**
 * @file dvc_motor_dji.cpp
 * @author WangFonzhuo
 * @brief DJI电机的配置与操作
 * @version 1.0
 * @date 2025-12-28 26赛季定稿
 * @date 2026-05-22 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "dvc_motor_dji.h"
#include "alg_basic.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// 电机控制报文发送缓冲区
uint8_t CAN1_0x1fe_Tx_Data[8];
uint8_t CAN1_0x1ff_Tx_Data[8];
uint8_t CAN1_0x200_Tx_Data[8];
uint8_t CAN1_0x2fe_Tx_Data[8];

uint8_t CAN2_0x1fe_Tx_Data[8];
uint8_t CAN2_0x1ff_Tx_Data[8];
uint8_t CAN2_0x200_Tx_Data[8];
uint8_t CAN2_0x2fe_Tx_Data[8];

uint8_t CAN3_0x1fe_Tx_Data[8];
uint8_t CAN3_0x1ff_Tx_Data[8];
uint8_t CAN3_0x200_Tx_Data[8];
uint8_t CAN3_0x2fe_Tx_Data[8];

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief 分配CAN发送缓冲区
 *
 * @param hfdcan CAN编号
 * @param __CAN_Rx_ID 电机反馈报文ID枚举
 * @param __Distinction 电机型号区分枚举
 * @return uint8_t* 缓冲区指针
 */
static uint8_t *allocate_tx_data(FDCAN_HandleTypeDef *hfdcan, Enum_Motor_DJI_ID __CAN_Rx_ID, Enum_Motor_DJI_DISTINCTION __Distinction);

/**
 * @brief 估计功率值
 *
 * @param Power_Model 功率模型
 * @param Current 电流值
 * @param Omega 角速度值
 * @return float 估计功率值
 */
static float power_calculate(const Struct_Motor_DJI_Power_Model *Power_Model, float Current, float Omega);

/* Function definitions ------------------------------------------------------*/

/**
 * @brief DJI电机CAN发送回调函数
 * @param hfdcan CAN编号
 */
void Motor_DJI_CAN_Tx_PeriodElapsedCallback(FDCAN_HandleTypeDef *hfdcan)
{
    if (hfdcan == nullptr)
    {
        return;
    }

    if (hfdcan->Instance == FDCAN1)
    {
        CAN_Transmit_Data(hfdcan, 0x1FE, CAN1_0x1fe_Tx_Data, 8U);
        CAN_Transmit_Data(hfdcan, 0x1FF, CAN1_0x1ff_Tx_Data, 8U);
        CAN_Transmit_Data(hfdcan, 0x200, CAN1_0x200_Tx_Data, 8U);
        CAN_Transmit_Data(hfdcan, 0x2FE, CAN1_0x2fe_Tx_Data, 8U);
    }
#ifdef FDCAN2
    else if (hfdcan->Instance == FDCAN2)
    {
        CAN_Transmit_Data(hfdcan, 0x1FE, CAN2_0x1fe_Tx_Data, 8U);
        CAN_Transmit_Data(hfdcan, 0x1FF, CAN2_0x1ff_Tx_Data, 8U);
        CAN_Transmit_Data(hfdcan, 0x200, CAN2_0x200_Tx_Data, 8U);
        CAN_Transmit_Data(hfdcan, 0x2FE, CAN2_0x2fe_Tx_Data, 8U);
    }
#endif
#ifdef FDCAN3
    else if (hfdcan->Instance == FDCAN3)
    {
        CAN_Transmit_Data(hfdcan, 0x1FE, CAN3_0x1fe_Tx_Data, 8U);
        CAN_Transmit_Data(hfdcan, 0x1FF, CAN3_0x1ff_Tx_Data, 8U);
        CAN_Transmit_Data(hfdcan, 0x200, CAN3_0x200_Tx_Data, 8U);
        CAN_Transmit_Data(hfdcan, 0x2FE, CAN3_0x2fe_Tx_Data, 8U);
    }
#endif
    else
    {
        return;
    }
}

/**
 * @brief 初始化GM6020电机
 *
 * @param hfdcan CAN编号
 * @param __CAN_Rx_ID 电机反馈报文ID枚举
 * @param __Control_Method 电机控制方式
 * @param __Encoder_Offset 编码器零点偏移
 */
void Class_Motor_DJI_GM6020::Init(FDCAN_HandleTypeDef *hfdcan, Enum_Motor_DJI_ID __CAN_Rx_ID,
                              Enum_Motor_DJI_Control_Method __Control_Method,
                              int32_t __Encoder_Offset)
{
    if (hfdcan == nullptr)
    {
        return;
    }

    if (hfdcan->Instance == FDCAN1)
    {
        CAN_Manage_Object = &CAN1_Manage_Object;
    }
#ifdef FDCAN2
    else if (hfdcan->Instance == FDCAN2)
    {
        CAN_Manage_Object = &CAN2_Manage_Object;
    }
#endif
#ifdef FDCAN3
    else if (hfdcan->Instance == FDCAN3)
    {
        CAN_Manage_Object = &CAN3_Manage_Object;
    }
#endif
    else
    {
        return;
    }

    CAN_Rx_ID = __CAN_Rx_ID;
    Tx_Data = allocate_tx_data(hfdcan, __CAN_Rx_ID, Motor_DJI_GM6020);

    Encoder_Offset = __Encoder_Offset;
    Control_Method = __Control_Method;
}

/**
 * @brief CAN通信接收回调函数
 *
 * @param Rx_Data 接收数据缓冲区
 */
void Class_Motor_DJI_GM6020::CAN_RxCpltCallback(uint8_t *Rx_Data)
{
    Flag += 1U;
    Data_Process(Rx_Data);
}

/**
 * @brief TIM定时器中断定期检测电机是否存活
 */
void Class_Motor_DJI_GM6020::TIM_100ms_Alive_PeriodElapsedCallback()
{
    // 判断该时间段内是否接收过电机数据
    if (Flag == Pre_Flag)
    {
        // 电机断开连接
        Motor_Status = Motor_DJI_Status_DISABLE;
    }
    else
    {
        // 电机保持连接
        Motor_Status = Motor_DJI_Status_ENABLE;
    }
    Pre_Flag = Flag;
}

/**
 * @brief TIM定时器中断计算回调函数
 */
void Class_Motor_DJI_GM6020::TIM_Calculate_PeriodElapsedCallback()
{
    if (Motor_Status == Motor_DJI_Status_DISABLE)
    {
        Clear();
        return;
    }

    PID_Calculate();

    Out = (Target_Current + Feedforward_Current) * CURRENT_TO_OUT;
    Basic_Math_Constrain(&Out, -THEORETICAL_OUTPUT_CURRENT_MAX, THEORETICAL_OUTPUT_CURRENT_MAX);

    Output();
}

/**
 * @brief 数据处理过程
 * @param Rx_Data_Buffer 接收数据缓冲区
 */
void Class_Motor_DJI_GM6020::Data_Process(uint8_t *Rx_Data_Buffer)
{
    uint16_t tmp_encoder;
    int16_t tmp_omega;
    int16_t tmp_current;
    int32_t delta_encoder;

    if (Rx_Data_Buffer == nullptr)
    {
        return;
    }

    Struct_Motor_DJI_CAN_Rx_Data *tmp_buffer = (Struct_Motor_DJI_CAN_Rx_Data *) Rx_Data_Buffer;

    // 处理大小端
    Basic_Math_Endian_Reverse_16((void *) &tmp_buffer->Encoder, (void *) &tmp_encoder);
    Basic_Math_Endian_Reverse_16((void *) &tmp_buffer->Omega, (void *) &tmp_omega);
    Basic_Math_Endian_Reverse_16((void *) &tmp_buffer->Current, (void *) &tmp_current);

    if (Rx_Data.Encoder_Init_Flag == false)
    {
        // 第一帧只建立编码器基准, 防止Pre_Encoder默认0导致误判跨圈
        Rx_Data.Encoder_Init_Flag = true;
        Rx_Data.Pre_Encoder = tmp_encoder;
        Rx_Data.Now_Encoder = tmp_encoder;
        Rx_Data.Total_Round = 0;
        Rx_Data.Total_Encoder = (int64_t)tmp_encoder + (int64_t)Encoder_Offset;
    }
    else
    {
        delta_encoder = (int32_t)tmp_encoder - (int32_t)Rx_Data.Pre_Encoder;

        if (delta_encoder < -(int32_t)(ENCODER_NUM_PER_ROUND / 2U))
        {
            Rx_Data.Total_Round++;
        }
        else if (delta_encoder > (int32_t)(ENCODER_NUM_PER_ROUND / 2U))
        {
            Rx_Data.Total_Round--;
        }

        Rx_Data.Now_Encoder = tmp_encoder;
        Rx_Data.Total_Encoder = (int64_t)Rx_Data.Total_Round * (int64_t)ENCODER_NUM_PER_ROUND +
                                (int64_t)tmp_encoder + (int64_t)Encoder_Offset;
        Rx_Data.Pre_Encoder = tmp_encoder;
    }

    Rx_Data.Now_Angle = (float)Rx_Data.Total_Encoder / (float)ENCODER_NUM_PER_ROUND * 2.0f * 3.14159265358979323846f;
    Rx_Data.Now_Omega = (float)tmp_omega * BASIC_MATH_RPM_TO_RADPS;
    Rx_Data.Now_Current = (float)tmp_current / CURRENT_TO_OUT;
    Rx_Data.Now_Temperature = (float)Rx_Data_Buffer[6];
    Rx_Data.Now_Power = power_calculate(&POWER_MODEL, Rx_Data.Now_Current, Rx_Data.Now_Omega);
}

/**
 * @brief 计算PID
 */
void Class_Motor_DJI_GM6020::PID_Calculate()
{
    // 是否使用外部反馈
    float angle_feedback = External_Angle_Flag ? Now_External_Angle : Rx_Data.Now_Angle;
    float omega_feedback = External_Omega_Flag ? Now_External_Omega : Rx_Data.Now_Omega;

    // 重置外部速度反馈标志，等待下一次设置
    External_Omega_Flag = false;
    External_Angle_Flag = false;

    switch (Control_Method)
    {
        case (Motor_DJI_Control_Method_CURRENT):
        {
            break;
        }
        case (Motor_DJI_Control_Method_OMEGA):
        {
            PID_Omega.Set_Target(Target_Omega + Feedforward_Omega);
            PID_Omega.Set_Now(omega_feedback);
            PID_Omega.TIM_Adjust_PeriodElapsedCallback();

            Target_Current = PID_Omega.Get_Out();

            break;
        }
        case (Motor_DJI_Control_Method_ANGLE):
        {
            PID_Angle.Set_Target(Target_Angle);
            PID_Angle.Set_Now(angle_feedback);
            PID_Angle.TIM_Adjust_PeriodElapsedCallback();

            Target_Omega = PID_Angle.Get_Out();

            PID_Omega.Set_Target(Target_Omega + Feedforward_Omega);
            PID_Omega.Set_Now(omega_feedback);
            PID_Omega.TIM_Adjust_PeriodElapsedCallback();

            Target_Current = PID_Omega.Get_Out();

            break;
        }
        default:
        {
            Target_Current = 0.0f;

            break;
        }
    }
}

/**
 * @brief 电机数据输出到CAN总线发送缓冲区
 *
 */
void Class_Motor_DJI_GM6020::Output()
{
    int16_t tmp_out;

    if (Tx_Data == nullptr)
    {
        return;
    }

    tmp_out = (int16_t)Out;

    Tx_Data[0] = (uint8_t)((uint16_t)tmp_out >> 8U);
    Tx_Data[1] = (uint8_t)((uint16_t)tmp_out);
}

/**
 * @brief 重置
 */
void Class_Motor_DJI_GM6020::Clear()
{
    Rx_Data.Encoder_Init_Flag = false;

    PID_Angle.Clear();
    PID_Omega.Clear();

    Out = 0.0f;

    Rx_Data = {};

    Target_Angle = 0.0f;
    Target_Omega = 0.0f;
    Target_Current = 0.0f;
    Feedforward_Omega = 0.0f;
    Feedforward_Current = 0.0f;

    if (Tx_Data != nullptr)
    {
        Tx_Data[0] = 0U;
        Tx_Data[1] = 0U;
    }
}

/**
 * @brief 初始化C610无刷电调
 *
 * @param hfdcan CAN编号
 * @param __CAN_Rx_ID 电机反馈报文ID枚举
 * @param __Control_Method 电机控制方式
 * @param __Gearbox_Rate 减速箱减速比
 */
void Class_Motor_DJI_C610::Init(FDCAN_HandleTypeDef *hfdcan, Enum_Motor_DJI_ID __CAN_Rx_ID,
                         Enum_Motor_DJI_Control_Method __Control_Method,
                         float __Gearbox_Rate)
{
    if (hfdcan == nullptr)
    {
        return;
    }

    if (hfdcan->Instance == FDCAN1)
    {
        CAN_Manage_Object = &CAN1_Manage_Object;
    }
#ifdef FDCAN2
    else if (hfdcan->Instance == FDCAN2)
    {
        CAN_Manage_Object = &CAN2_Manage_Object;
    }
#endif
#ifdef FDCAN3
    else if (hfdcan->Instance == FDCAN3)
    {
        CAN_Manage_Object = &CAN3_Manage_Object;
    }
#endif
    else
    {
        return;
    }

    CAN_Rx_ID = __CAN_Rx_ID;
    Tx_Data = allocate_tx_data(hfdcan, __CAN_Rx_ID, Motor_DJI_OTHER);

    Control_Method = __Control_Method;

    if (__Gearbox_Rate > 1.0e-6f)
    {
        Gearbox_Rate = __Gearbox_Rate;
    }
}

/**
 * @brief CAN通信接收回调函数
 * @param Rx_Data 接收数据缓冲区
 */
void Class_Motor_DJI_C610::CAN_RxCpltCallback(uint8_t *Rx_Data)
{
    Flag += 1U;
    Data_Process(Rx_Data);
}

/**
 * @brief TIM定时器中断定期检测电机是否存活
 */
void Class_Motor_DJI_C610::TIM_100ms_Alive_PeriodElapsedCallback()
{
    // 判断该时间段内是否接收过电机数据
    if (Flag == Pre_Flag)
    {
        // 电机断开连接
        Motor_Status = Motor_DJI_Status_DISABLE;
    }
    else
    {
        // 电机保持连接
        Motor_Status = Motor_DJI_Status_ENABLE;
    }
    Pre_Flag = Flag;
}

/**
 * @brief TIM定时器中断计算回调函数
 */
void Class_Motor_DJI_C610::TIM_Calculate_PeriodElapsedCallback()
{
    if (Motor_Status == Motor_DJI_Status_DISABLE)
    {
        Clear();
        return;
    }

    PID_Calculate();

    Out = (Target_Current + Feedforward_Current) * CURRENT_TO_OUT;
    Basic_Math_Constrain(&Out, -THEORETICAL_OUTPUT_CURRENT_MAX, THEORETICAL_OUTPUT_CURRENT_MAX);

    Output();
}

/**
 * @brief 数据处理过程
 * @param Rx_Data_Buffer 接收数据缓冲区
 */
void Class_Motor_DJI_C610::Data_Process(uint8_t *Rx_Data_Buffer)
{
    uint16_t tmp_encoder;
    int16_t tmp_omega;
    int16_t tmp_current;
    int32_t delta_encoder;

    if (Rx_Data_Buffer == nullptr)
    {
        return;
    }

    Struct_Motor_DJI_CAN_Rx_Data *tmp_buffer = (Struct_Motor_DJI_CAN_Rx_Data *) Rx_Data_Buffer;

    // 处理大小端
    Basic_Math_Endian_Reverse_16((void *) &tmp_buffer->Encoder, (void *) &tmp_encoder);
    Basic_Math_Endian_Reverse_16((void *) &tmp_buffer->Omega, (void *) &tmp_omega);
    Basic_Math_Endian_Reverse_16((void *) &tmp_buffer->Current, (void *) &tmp_current);

    if (Rx_Data.Encoder_Init_Flag == false)
    {
        // 第一帧只建立相对编码器基准, 防止Pre_Encoder默认0导致误判跨圈, 同时上电当前位置视为零点
        Rx_Data.Encoder_Init_Flag = true;
        Rx_Data.Pre_Encoder = tmp_encoder;
        Rx_Data.Now_Encoder = tmp_encoder;
        Rx_Data.Total_Round = 0;
        Rx_Data.Total_Encoder = 0;
    }
    else
    {
        delta_encoder = (int32_t)tmp_encoder - (int32_t)Rx_Data.Pre_Encoder;

        if (delta_encoder < -(int32_t)(ENCODER_NUM_PER_ROUND / 2U))
        {
            delta_encoder += (int32_t)ENCODER_NUM_PER_ROUND;
            Rx_Data.Total_Round++;
        }
        else if (delta_encoder > (int32_t)(ENCODER_NUM_PER_ROUND / 2U))
        {
            delta_encoder -= (int32_t)ENCODER_NUM_PER_ROUND;
            Rx_Data.Total_Round--;
        }

        Rx_Data.Total_Encoder += (int64_t)delta_encoder;
        Rx_Data.Now_Encoder = tmp_encoder;
        Rx_Data.Pre_Encoder = tmp_encoder;
    }

    Rx_Data.Now_Angle = (float)Rx_Data.Total_Encoder / (float)ENCODER_NUM_PER_ROUND * 2.0f * 3.14159265358979323846f / Gearbox_Rate;
    Rx_Data.Now_Omega = (float)tmp_omega * BASIC_MATH_RPM_TO_RADPS / Gearbox_Rate;
    Rx_Data.Now_Current = (float)tmp_current / CURRENT_TO_OUT;
    Rx_Data.Now_Temperature = (float)Rx_Data_Buffer[6];
    Rx_Data.Now_Power = power_calculate(&POWER_MODEL, Rx_Data.Now_Current, Rx_Data.Now_Omega);
}

/**
 * @brief 计算PID
 */
void Class_Motor_DJI_C610::PID_Calculate()
{
    // 是否使用外部反馈
    float angle_feedback = External_Angle_Flag ? Now_External_Angle : Rx_Data.Now_Angle;
    float omega_feedback = External_Omega_Flag ? Now_External_Omega : Rx_Data.Now_Omega;

    // 重置外部速度反馈标志，等待下一次设置
    External_Omega_Flag = false;
    External_Angle_Flag = false;

    switch (Control_Method)
    {
        case (Motor_DJI_Control_Method_CURRENT):
        {
            break;
        }
        case (Motor_DJI_Control_Method_OMEGA):
        {
            PID_Omega.Set_Target(Target_Omega + Feedforward_Omega);
            PID_Omega.Set_Now(omega_feedback);
            PID_Omega.TIM_Adjust_PeriodElapsedCallback();

            Target_Current = PID_Omega.Get_Out();

            break;
        }
        case (Motor_DJI_Control_Method_ANGLE):
        {
            PID_Angle.Set_Target(Target_Angle);
            PID_Angle.Set_Now(angle_feedback);
            PID_Angle.TIM_Adjust_PeriodElapsedCallback();

            Target_Omega = PID_Angle.Get_Out();

            PID_Omega.Set_Target(Target_Omega + Feedforward_Omega);
            PID_Omega.Set_Now(omega_feedback);
            PID_Omega.TIM_Adjust_PeriodElapsedCallback();

            Target_Current = PID_Omega.Get_Out();

            break;
        }
        default:
        {
            Target_Current = 0.0f;

            break;
        }
    }
}

/**
 * @brief 电机数据输出到CAN总线发送缓冲区
 */
void Class_Motor_DJI_C610::Output()
{
    int16_t tmp_out;

    if (Tx_Data == nullptr)
    {
        return;
    }

    tmp_out = (int16_t)Out;

    Tx_Data[0] = (uint8_t)((uint16_t)tmp_out >> 8U);
    Tx_Data[1] = (uint8_t)((uint16_t)tmp_out);
}

/**
 * @brief 重置
 */
void Class_Motor_DJI_C610::Clear()
{
    Rx_Data.Encoder_Init_Flag = false;

    PID_Angle.Clear();
    PID_Omega.Clear();

    Out = 0.0f;

    Rx_Data = {};

    Target_Angle = 0.0f;
    Target_Omega = 0.0f;
    Target_Current = 0.0f;
    Feedforward_Omega = 0.0f;
    Feedforward_Current = 0.0f;

    if (Tx_Data != nullptr)
    {
        Tx_Data[0] = 0U;
        Tx_Data[1] = 0U;
    }
}


/**
 * @brief 初始化C620无刷电调
 *
 * @param hfdcan CAN编号
 * @param __CAN_Rx_ID 电机反馈报文ID枚举
 * @param __Control_Method 电机控制方式
 * @param __Gearbox_Rate 减速箱减速比
 */
void Class_Motor_DJI_C620::Init(FDCAN_HandleTypeDef *hfdcan, Enum_Motor_DJI_ID __CAN_Rx_ID,
                         Enum_Motor_DJI_Control_Method __Control_Method,
                         float __Gearbox_Rate)
{
    if (hfdcan == nullptr)
    {
        return;
    }

    if (hfdcan->Instance == FDCAN1)
    {
        CAN_Manage_Object = &CAN1_Manage_Object;
    }
#ifdef FDCAN2
    else if (hfdcan->Instance == FDCAN2)
    {
        CAN_Manage_Object = &CAN2_Manage_Object;
    }
#endif
#ifdef FDCAN3
    else if (hfdcan->Instance == FDCAN3)
    {
        CAN_Manage_Object = &CAN3_Manage_Object;
    }
#endif
    else
    {
        return;
    }

    CAN_Rx_ID = __CAN_Rx_ID;
    Tx_Data = allocate_tx_data(hfdcan, __CAN_Rx_ID, Motor_DJI_OTHER);

    Control_Method = __Control_Method;

    if (__Gearbox_Rate > 1.0e-6f)
    {
        Gearbox_Rate = __Gearbox_Rate;
    }
}

/**
 * @brief CAN通信接收回调函数
 * @param Rx_Data 接收数据缓冲区
 */
void Class_Motor_DJI_C620::CAN_RxCpltCallback(uint8_t *Rx_Data)
{
    Flag += 1U;
    Data_Process(Rx_Data);
}

/**
 * @brief TIM定时器中断定期检测电机是否存活
 */
void Class_Motor_DJI_C620::TIM_100ms_Alive_PeriodElapsedCallback()
{
    // 判断该时间段内是否接收过电机数据
    if (Flag == Pre_Flag)
    {
        // 电机断开连接
        Motor_Status = Motor_DJI_Status_DISABLE;
    }
    else
    {
        // 电机保持连接
        Motor_Status = Motor_DJI_Status_ENABLE;
    }
    Pre_Flag = Flag;
}

/**
 * @brief TIM定时器中断计算回调函数
 */
void Class_Motor_DJI_C620::TIM_Calculate_PeriodElapsedCallback()
{
    if (Motor_Status == Motor_DJI_Status_DISABLE)
    {
        Clear();
        return;
    }

    PID_Calculate();

    Out = (Target_Current + Feedforward_Current) * CURRENT_TO_OUT;
    Basic_Math_Constrain(&Out, -THEORETICAL_OUTPUT_CURRENT_MAX, THEORETICAL_OUTPUT_CURRENT_MAX);

    Output();
}

/**
 * @brief 数据处理过程
 * @param Rx_Data_Buffer 接收数据缓冲区
 */
void Class_Motor_DJI_C620::Data_Process(uint8_t *Rx_Data_Buffer)
{
    uint16_t tmp_encoder;
    int16_t tmp_omega;
    int16_t tmp_current;
    int32_t delta_encoder;

    if (Rx_Data_Buffer == nullptr)
    {
        return;
    }

    Struct_Motor_DJI_CAN_Rx_Data *tmp_buffer = (Struct_Motor_DJI_CAN_Rx_Data *) Rx_Data_Buffer;

    // 处理大小端
    Basic_Math_Endian_Reverse_16((void *) &tmp_buffer->Encoder, (void *) &tmp_encoder);
    Basic_Math_Endian_Reverse_16((void *) &tmp_buffer->Omega, (void *) &tmp_omega);
    Basic_Math_Endian_Reverse_16((void *) &tmp_buffer->Current, (void *) &tmp_current);

    if (Rx_Data.Encoder_Init_Flag == false)
    {
        // 第一帧只建立相对编码器基准, 防止Pre_Encoder默认0导致误判跨圈, 同时上电当前位置视为零点
        Rx_Data.Encoder_Init_Flag = true;
        Rx_Data.Pre_Encoder = tmp_encoder;
        Rx_Data.Now_Encoder = tmp_encoder;
        Rx_Data.Total_Round = 0;
        Rx_Data.Total_Encoder = 0;
    }
    else
    {
        delta_encoder = (int32_t)tmp_encoder - (int32_t)Rx_Data.Pre_Encoder;

        if (delta_encoder < -(int32_t)(ENCODER_NUM_PER_ROUND / 2U))
        {
            delta_encoder += (int32_t)ENCODER_NUM_PER_ROUND;
            Rx_Data.Total_Round++;
        }
        else if (delta_encoder > (int32_t)(ENCODER_NUM_PER_ROUND / 2U))
        {
            delta_encoder -= (int32_t)ENCODER_NUM_PER_ROUND;
            Rx_Data.Total_Round--;
        }

        Rx_Data.Total_Encoder += (int64_t)delta_encoder;
        Rx_Data.Now_Encoder = tmp_encoder;
        Rx_Data.Pre_Encoder = tmp_encoder;
    }

    Rx_Data.Now_Angle = (float)Rx_Data.Total_Encoder / (float)ENCODER_NUM_PER_ROUND * 2.0f * 3.14159265358979323846f / Gearbox_Rate;
    Rx_Data.Now_Omega = (float)tmp_omega * BASIC_MATH_RPM_TO_RADPS / Gearbox_Rate;
    Rx_Data.Now_Current = (float)tmp_current / CURRENT_TO_OUT;
    Rx_Data.Now_Temperature = (float)Rx_Data_Buffer[6];
    Rx_Data.Now_Power = power_calculate(&POWER_MODEL, Rx_Data.Now_Current, Rx_Data.Now_Omega);
}

/**
 * @brief 计算PID
 */
void Class_Motor_DJI_C620::PID_Calculate()
{
    // 是否使用外部反馈
    float angle_feedback = External_Angle_Flag ? Now_External_Angle : Rx_Data.Now_Angle;
    float omega_feedback = External_Omega_Flag ? Now_External_Omega : Rx_Data.Now_Omega;

    // 重置外部速度反馈标志，等待下一次设置
    External_Omega_Flag = false;
    External_Angle_Flag = false;

    switch (Control_Method)
    {
        case (Motor_DJI_Control_Method_CURRENT):
        {
            break;
        }
        case (Motor_DJI_Control_Method_OMEGA):
        {
            PID_Omega.Set_Target(Target_Omega + Feedforward_Omega);
            PID_Omega.Set_Now(omega_feedback);
            PID_Omega.TIM_Adjust_PeriodElapsedCallback();

            Target_Current = PID_Omega.Get_Out();

            break;
        }
        case (Motor_DJI_Control_Method_ANGLE):
        {
            PID_Angle.Set_Target(Target_Angle);
            PID_Angle.Set_Now(angle_feedback);
            PID_Angle.TIM_Adjust_PeriodElapsedCallback();

            Target_Omega = PID_Angle.Get_Out();

            PID_Omega.Set_Target(Target_Omega + Feedforward_Omega);
            PID_Omega.Set_Now(omega_feedback);
            PID_Omega.TIM_Adjust_PeriodElapsedCallback();

            Target_Current = PID_Omega.Get_Out();

            break;
        }
        default:
        {
            Target_Current = 0.0f;

            break;
        }
    }
}

/**
 * @brief 电机数据输出到CAN总线发送缓冲区
 */
void Class_Motor_DJI_C620::Output()
{
    int16_t tmp_out;

    if (Tx_Data == nullptr)
    {
        return;
    }

    tmp_out = (int16_t)Out;

    Tx_Data[0] = (uint8_t)((uint16_t)tmp_out >> 8U);
    Tx_Data[1] = (uint8_t)((uint16_t)tmp_out);
}

/**
 * @brief 重置
 */
void Class_Motor_DJI_C620::Clear()
{
    Rx_Data.Encoder_Init_Flag = false;

    PID_Angle.Clear();
    PID_Omega.Clear();

    Out = 0.0f;

    Rx_Data = {};

    Target_Angle = 0.0f;
    Target_Omega = 0.0f;
    Target_Current = 0.0f;
    Feedforward_Omega = 0.0f;
    Feedforward_Current = 0.0f;

    if (Tx_Data != nullptr)
    {
        Tx_Data[0] = 0U;
        Tx_Data[1] = 0U;
    }
}

/**
 * @brief 分配CAN发送缓冲区
 * @param hfdcan CAN编号
 * @param __CAN_Rx_ID 电机反馈报文ID枚举
 * @param __Distinction 电机型号区分枚举
 * @return uint8_t* 缓冲区指针
 */
static uint8_t *allocate_tx_data(FDCAN_HandleTypeDef *hfdcan, Enum_Motor_DJI_ID __CAN_Rx_ID,
                                 Enum_Motor_DJI_DISTINCTION __Distinction)
{
    uint8_t *tx_data_0x1fe = nullptr;
    uint8_t *tx_data_0x1ff = nullptr;
    uint8_t *tx_data_0x200 = nullptr;
    uint8_t *tx_data_0x2fe = nullptr;

    uint8_t *tmp_tx_data = nullptr;
    uint8_t byte_offset = 0U;

    if (hfdcan == nullptr)
    {
        return (nullptr);
    }

    if (hfdcan->Instance == FDCAN1)
    {
        tx_data_0x1fe = CAN1_0x1fe_Tx_Data;
        tx_data_0x1ff = CAN1_0x1ff_Tx_Data;
        tx_data_0x200 = CAN1_0x200_Tx_Data;
        tx_data_0x2fe = CAN1_0x2fe_Tx_Data;
    }
#ifdef FDCAN2
    else if (hfdcan->Instance == FDCAN2)
    {
        tx_data_0x1fe = CAN2_0x1fe_Tx_Data;
        tx_data_0x1ff = CAN2_0x1ff_Tx_Data;
        tx_data_0x200 = CAN2_0x200_Tx_Data;
        tx_data_0x2fe = CAN2_0x2fe_Tx_Data;
    }
#endif
#ifdef FDCAN3
    else if (hfdcan->Instance == FDCAN3)
    {
        tx_data_0x1fe = CAN3_0x1fe_Tx_Data;
        tx_data_0x1ff = CAN3_0x1ff_Tx_Data;
        tx_data_0x200 = CAN3_0x200_Tx_Data;
        tx_data_0x2fe = CAN3_0x2fe_Tx_Data;
    }
#endif
    else
    {
        return (nullptr);
    }

    if ((__CAN_Rx_ID >= Motor_DJI_ID_0x201) && (__CAN_Rx_ID <= Motor_DJI_ID_0x204))
    {
        if (__Distinction == Motor_DJI_GM6020)
        {
            return (nullptr);
        }        

        tmp_tx_data = tx_data_0x200;
        byte_offset = (uint8_t)((__CAN_Rx_ID - Motor_DJI_ID_0x201) * 2U);
    }
    else if ((__CAN_Rx_ID >= Motor_DJI_ID_0x205) && (__CAN_Rx_ID <= Motor_DJI_ID_0x208))
    {
        if (__Distinction == Motor_DJI_GM6020)
        {
            tmp_tx_data = tx_data_0x1fe;
        }
        else
        {
            tmp_tx_data = tx_data_0x1ff;
        }
        byte_offset = (uint8_t)((__CAN_Rx_ID - Motor_DJI_ID_0x205) * 2U);
    }
    else if ((__CAN_Rx_ID >= Motor_DJI_ID_0x209) && (__CAN_Rx_ID <= Motor_DJI_ID_0x20B))
    {
        if (__Distinction != Motor_DJI_GM6020)
        {
            return (nullptr);
        }        
        tmp_tx_data = tx_data_0x2fe;
        byte_offset = (uint8_t)((__CAN_Rx_ID - Motor_DJI_ID_0x209) * 2U);
    }
    else
    {
        return (nullptr);
    }

    return (&tmp_tx_data[byte_offset]);
}

/**
 * @brief 估计功率值
 * @param Power_Model 功率模型
 * @param Current 电流值
 * @param Omega 角速度值
 * @return float 估计功率值
 */
static float power_calculate(const Struct_Motor_DJI_Power_Model *Power_Model, float Current, float Omega)
{
    if (Power_Model == nullptr)
    {
        return (0.0f);
    }

    return (Power_Model->K_0 * Current * Omega +
            Power_Model->K_1 * Omega * Omega +
            Power_Model->K_2 * Current * Current +
            Power_Model->A);
}

/*----------------------------------------------------------------------------*/

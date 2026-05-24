/**
 * @file dvc_motor_dji.h
 * @author WangFonzhuo
 * @brief DJI电机的配置与操作
 * @version 1.0
 * @date 2025-12-28 26赛季定稿
 * @date 2026-05-22 27赛季
 */

#ifndef DVC_MOTOR_DJI_H
#define DVC_MOTOR_DJI_H

/* Includes ------------------------------------------------------------------*/

#include "alg_pid.h"
#include "drv_can.h"

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 电机状态
 */
enum Enum_Motor_DJI_Status
{
    Motor_DJI_Status_DISABLE = 0,
    Motor_DJI_Status_ENABLE,
};

/**
 * @brief 电机的反馈报文ID
 */
enum Enum_Motor_DJI_ID
{
    Motor_DJI_ID_UNDEFINED = 0,
    Motor_DJI_ID_0x201 = 0x201,
    Motor_DJI_ID_0x202 = 0x202,
    Motor_DJI_ID_0x203 = 0x203,
    Motor_DJI_ID_0x204 = 0x204,
    Motor_DJI_ID_0x205 = 0x205,
    Motor_DJI_ID_0x206 = 0x206,
    Motor_DJI_ID_0x207 = 0x207,
    Motor_DJI_ID_0x208 = 0x208,
    Motor_DJI_ID_0x209 = 0x209,
    Motor_DJI_ID_0x20A = 0x20A,
    Motor_DJI_ID_0x20B = 0x20B,
};

/**
 * @brief 区分该电机是GM6020还是其他型号, 以便分配不同的CAN发送缓存
 */
enum Enum_Motor_DJI_DISTINCTION
{
    Motor_DJI_GM6020 = 0,
    Motor_DJI_OTHER,
};

/**
 * @brief 电机控制方式
 */
enum Enum_Motor_DJI_Control_Method
{
    Motor_DJI_Control_Method_CURRENT = 0,
    Motor_DJI_Control_Method_OMEGA,
    Motor_DJI_Control_Method_ANGLE,
};

/**
 * @brief DJI电机CAN反馈源数据
 */
struct Struct_Motor_DJI_CAN_Rx_Data
{
    uint16_t Encoder;
    int16_t Omega;
    int16_t Current;
    uint8_t Temperature;
    uint8_t Reserved;
} __attribute__((packed));

/**
 * @brief 电机经过处理的数据
 */
struct Struct_Motor_DJI_Rx_Data
{
    float Now_Angle;
    float Now_Omega;
    float Now_Current;
    float Now_Temperature;
    float Now_Power;
    uint16_t Now_Encoder;
    uint16_t Pre_Encoder;
    int64_t Total_Encoder;
    int32_t Total_Round;
    bool Encoder_Init_Flag;
};

/**
 * @brief 电机功率模型
 * @note Power = K_0 * Current * Omega + K_1 * Omega^2 + K_2 * Current^2 + A
 */
struct Struct_Motor_DJI_Power_Model
{
    float K_0;
    float K_1;
    float K_2;
    float A;
};

/**
 * @brief GM6020无刷电机
 */
class Class_Motor_DJI_GM6020
{
public:
    // PID角度环控制
    Class_PID PID_Angle;

    // PID角速度环控制
    Class_PID PID_Omega;

    /**
     * @brief 初始化GM6020电机

     * @param hfdcan CAN编号
     * @param __CAN_Rx_ID 电机反馈报文ID枚举
     * @param __Control_Method 电机控制方式
     * @param __Encoder_Offset 编码器零点偏移
     */
    void Init(FDCAN_HandleTypeDef *hfdcan, Enum_Motor_DJI_ID __CAN_Rx_ID,
              Enum_Motor_DJI_Control_Method __Control_Method = Motor_DJI_Control_Method_ANGLE,
              int32_t __Encoder_Offset = 0);

    /**
     * @brief 获取电机状态
     */
    inline Enum_Motor_DJI_Status Get_Status() const;

    /**
     * @brief 获取转矩常数, N*m/A
     */
    inline float Get_CURRENT_TO_TORQUE() const;

    /**
     * @brief 获取当前角度, rad
     */
    inline float Get_Now_Angle() const;

    /**
     * @brief 获取当前角速度, rad/s
     */
    inline float Get_Now_Omega() const;

    /**
     * @brief 获取当前电流, A
     */
    inline float Get_Now_Current() const;

    /**
     * @brief 获取当前温度, 摄氏度
     */
    inline float Get_Now_Temperature() const;

    /**
     * @brief 获取当前功率, W
     */
    inline float Get_Now_Power() const;

    /**
     * @brief 获取当前编码器值
     */
    inline uint16_t Get_Now_Encoder() const;

    /**
     * @brief 获取累计编码器值
     */
    inline int64_t Get_Total_Encoder() const;

    /**
     * @brief 获取累计圈数
     */
    inline int32_t Get_Total_Round() const;

    /**
     * @brief 获取CAN反馈报文ID
     */
    inline Enum_Motor_DJI_ID Get_CAN_Rx_ID() const;

    /**
     * @brief 获取编码器零点偏移
     */
    inline int32_t Get_Encoder_Offset() const;

    /**
     * @brief 获取控制方式
     */
    inline Enum_Motor_DJI_Control_Method Get_Control_Method() const;

    /**
     * @brief 获取目标角度, rad
     */
    inline float Get_Target_Angle() const;

    /**
     * @brief 获取目标角速度, rad/s
     */
    inline float Get_Target_Omega() const;

    /**
     * @brief 获取目标电流, A
     */
    inline float Get_Target_Current() const;

    /**
     * @brief 获取前馈角速度, rad/s
     */
    inline float Get_Feedforward_Omega() const;

    /**
     * @brief 获取前馈电流, A
     */
    inline float Get_Feedforward_Current() const;

    /**
     * @brief 获取当前外部输入角度, rad
     */
    inline float Get_External_Angle() const;

    /**
     * @brief 获取当前外部输入角速度, rad/s
     */
    inline float Get_External_Omega() const;

    /**
     * @brief 获取当前输出值
     */
    inline float Get_Out() const;

    /**
     * @brief 设置控制方式
     */
    inline void Set_Control_Method(Enum_Motor_DJI_Control_Method __Control_Method);

    /**
     * @brief 设置目标角度值, rad
     */
    inline void Set_Target_Angle(float __Target_Angle);

    /**
     * @brief 设置目标角速度值, rad/s
     */
    inline void Set_Target_Omega(float __Target_Omega);

    /**
     * @brief 设置目标电流值, A
     */
    inline void Set_Target_Current(float __Target_Current);

    /**
     * @brief 设置角速度前馈值, rad/s
     */
    inline void Set_Feedforward_Omega(float __Feedforward_Omega);

    /**
     * @brief 设置电流前馈值, A
     */
    inline void Set_Feedforward_Current(float __Feedforward_Current);

    /**
     * @brief 设置外置角度值反馈, rad
     */
    inline void Set_Now_External_Angle(float __Now_External_Angle);

    /**
     * @brief 设置外置角速度值反馈, rad/s
     */
    inline void Set_Now_External_Omega(float __Now_External_Omega);

    /**
     * @brief CAN通信接收回调函数
     *
     * @param Rx_Data 接收数据缓冲区
     */
    void CAN_RxCpltCallback(uint8_t *Rx_Data);

    /**
     * @brief TIM定时器中断定期检测电机是否存活
     */
    void TIM_100ms_Alive_PeriodElapsedCallback();

    /**
     * @brief TIM定时器中断计算回调函数
     */
    void TIM_Calculate_PeriodElapsedCallback();

protected:
    // 绑定的CAN管理对象
    Struct_CAN_Manage_Object *CAN_Manage_Object = nullptr;
    // 电机反馈给单片机的报文ID
    Enum_Motor_DJI_ID CAN_Rx_ID = Motor_DJI_ID_UNDEFINED;
    // 发送缓存区
    uint8_t *Tx_Data = nullptr;
    // 编码器偏移
    int32_t Encoder_Offset = 0;

    // 一圈编码器刻度
    const uint16_t ENCODER_NUM_PER_ROUND = 8192U;
    // 电流到输出的转换系数
    const float CURRENT_TO_OUT = 16384.0f / 3.0f;
    // 理论最大电流输出值
    const float THEORETICAL_OUTPUT_CURRENT_MAX = 16384.0f;
    // 电流扭矩常数, N*m/A
    const float CURRENT_TO_TORQUE = 0.741f;
    // 功率计算模型
    const Struct_Motor_DJI_Power_Model POWER_MODEL = {0.0f, 0.0f, 0.0f, 0.0f};

    // 当前时刻的电机接收flag
    uint32_t Flag = 0;
    // 前一时刻的电机接收flag
    uint32_t Pre_Flag = 0;
    // 当前时刻的电机输出值
    float Out = 0.0f;

    // 电机状态
    Enum_Motor_DJI_Status Motor_Status = Motor_DJI_Status_DISABLE;
    // 电机对外接口信息
    Struct_Motor_DJI_Rx_Data Rx_Data = {};

    // 电机上层控制方式
    Enum_Motor_DJI_Control_Method Control_Method = Motor_DJI_Control_Method_ANGLE;

    // 目标角度, rad
    float Target_Angle = 0.0f;
    // 目标角速度, rad/s
    float Target_Omega = 0.0f;
    // 目标电流, A
    float Target_Current = 0.0f;

    // 前馈速度, rad/s
    float Feedforward_Omega = 0.0f;
    // 前馈电流, A
    float Feedforward_Current = 0.0f;

    // 当前外部输入角度, rad
    float Now_External_Angle = 0.0f;
    // 当前外部输入角速度, rad/s
    float Now_External_Omega = 0.0f;
    // 外部输入角度标志
    bool External_Angle_Flag = false;
    // 外部输入角速度标志
    bool External_Omega_Flag = false;

    // 数据处理过程
    void Data_Process(uint8_t *Rx_Data_Buffer);

    // PID计算
    void PID_Calculate();

    // 电机数据输出到CAN总线发送缓冲区
    void Output();

    // 重置
    void Clear();
};

/**
 * @brief C610无刷电调
 */
class Class_Motor_DJI_C610
{
public:
    // PID角度环控制
    Class_PID PID_Angle;

    // PID角速度环控制
    Class_PID PID_Omega;

    /**
     * @brief 初始化C610无刷电调
     *
     * @param hfdcan CAN编号
     * @param __CAN_Rx_ID 电机反馈报文ID枚举
     * @param __Control_Method 电机控制方式
     * @param __Gearbox_Rate 减速箱减速比
     */
    void Init(FDCAN_HandleTypeDef *hfdcan, Enum_Motor_DJI_ID __CAN_Rx_ID,
              Enum_Motor_DJI_Control_Method __Control_Method = Motor_DJI_Control_Method_ANGLE,
              float __Gearbox_Rate = 36.0f);

    /**
     * @brief 获取电机状态
     */
    inline Enum_Motor_DJI_Status Get_Status() const;

    /**
     * @brief 获取减速箱减速比
     */
    inline float Get_Gearbox_Rate() const;

    /**
     * @brief 获取减速前转矩常数, N*m/A
     */
    inline float Get_CURRENT_TO_TORQUE() const;

    /**
     * @brief 获取当前角度, rad
     */
    inline float Get_Now_Angle() const;

    /**
     * @brief 获取当前角速度, rad/s
     */
    inline float Get_Now_Omega() const;

    /**
     * @brief 获取当前电流, A
     */
    inline float Get_Now_Current() const;

    /**
     * @brief 获取当前温度, 摄氏度
     */
    inline float Get_Now_Temperature() const;

    /**
     * @brief 获取当前功率, W
     */
    inline float Get_Now_Power() const;

    /**
     * @brief 获取当前编码器值
     */
    inline uint16_t Get_Now_Encoder() const;

    /**
     * @brief 获取累计编码器值
     */
    inline int64_t Get_Total_Encoder() const;

    /**
     * @brief 获取累计圈数
     */
    inline int32_t Get_Total_Round() const;

    /**
     * @brief 获取CAN反馈报文ID
     */
    inline Enum_Motor_DJI_ID Get_CAN_Rx_ID() const;

    /**
     * @brief 获取控制方式
     */
    inline Enum_Motor_DJI_Control_Method Get_Control_Method() const;

    /**
     * @brief 获取目标角度, rad
     */
    inline float Get_Target_Angle() const;

    /**
     * @brief 获取目标角速度, rad/s
     */
    inline float Get_Target_Omega() const;

    /**
     * @brief 获取目标电流, A
     */
    inline float Get_Target_Current() const;

    /**
     * @brief 获取前馈角速度, rad/s
     */
    inline float Get_Feedforward_Omega() const;

    /**
     * @brief 获取前馈电流, A
     */
    inline float Get_Feedforward_Current() const;

    /**
     * @brief 获取当前外部输入角度, rad
     */
    inline float Get_External_Angle() const;

    /**
     * @brief 获取当前外部输入角速度, rad/s
     */
    inline float Get_External_Omega() const;

    /**
     * @brief 获取当前输出值
     */
    inline float Get_Out() const;

    /**
     * @brief 设置控制方式
     */
    inline void Set_Control_Method(Enum_Motor_DJI_Control_Method __Control_Method);

    /**
     * @brief 设置减速箱减速比
     */
    inline void Set_Gearbox_Rate(float __Gearbox_Rate);

    /**
     * @brief 设置目标角度值, rad
     */
    inline void Set_Target_Angle(float __Target_Angle);

    /**
     * @brief 设置目标角速度值, rad/s
     */
    inline void Set_Target_Omega(float __Target_Omega);

    /**
     * @brief 设置目标电流值, A
     */
    inline void Set_Target_Current(float __Target_Current);

    /**
     * @brief 设置角速度前馈值, rad/s
     */
    inline void Set_Feedforward_Omega(float __Feedforward_Omega);

    /**
     * @brief 设置电流前馈值, A
     */
    inline void Set_Feedforward_Current(float __Feedforward_Current);

    /**
     * @brief 设置外置角度值反馈, rad
     */
    inline void Set_Now_External_Angle(float __Now_External_Angle);

    /**
     * @brief 设置外置角速度值反馈, rad/s
     */
    inline void Set_Now_External_Omega(float __Now_External_Omega);

    /**
     * @brief CAN通信接收回调函数
     * @param Rx_Data 接收数据缓冲区
     */
    void CAN_RxCpltCallback(uint8_t *Rx_Data);

    /**
     * @brief TIM定时器中断定期检测电机是否存活
     */
    void TIM_100ms_Alive_PeriodElapsedCallback();

    /**
     * @brief TIM定时器中断计算回调函数
     */
    void TIM_Calculate_PeriodElapsedCallback();

protected:
    // 绑定的CAN管理对象
    Struct_CAN_Manage_Object *CAN_Manage_Object = nullptr;
    // 电机反馈给单片机的报文ID
    Enum_Motor_DJI_ID CAN_Rx_ID = Motor_DJI_ID_UNDEFINED;
    // 发送缓存区
    uint8_t *Tx_Data = nullptr;

    // 一圈编码器刻度
    const uint16_t ENCODER_NUM_PER_ROUND = 8192U;
    // 电流到输出的转换系数
    const float CURRENT_TO_OUT = 10000.0f / 10.0f;
    // 理论最大电流输出值
    const float THEORETICAL_OUTPUT_CURRENT_MAX = 10000.0f;
    // 减速前, 电流扭矩常数, N*m/A
    const float CURRENT_TO_TORQUE = 0.18f / 36.0f;
    // 功率计算模型
    const Struct_Motor_DJI_Power_Model POWER_MODEL = {0.0f, 0.0f, 0.0f, 0.0f};
    // 减速箱减速比
    float Gearbox_Rate = 36.0f;

    // 当前时刻的电机接收flag
    uint32_t Flag = 0;
    // 前一时刻的电机接收flag
    uint32_t Pre_Flag = 0;
    // 当前时刻的电机输出值
    float Out = 0.0f;

    // 电机状态
    Enum_Motor_DJI_Status Motor_Status = Motor_DJI_Status_DISABLE;
    // 电机对外接口信息
    Struct_Motor_DJI_Rx_Data Rx_Data = {};

    // 电机上层控制方式
    Enum_Motor_DJI_Control_Method Control_Method = Motor_DJI_Control_Method_ANGLE;

    // 目标角度, rad
    float Target_Angle = 0.0f;
    // 目标角速度, rad/s
    float Target_Omega = 0.0f;
    // 目标电流, A
    float Target_Current = 0.0f;

    // 前馈速度, rad/s
    float Feedforward_Omega = 0.0f;
    // 前馈电流, A
    float Feedforward_Current = 0.0f;

    // 当前外部输入角度, rad
    float Now_External_Angle = 0.0f;
    // 当前外部输入角速度, rad/s
    float Now_External_Omega = 0.0f;
    // 外部输入角度标志
    bool External_Angle_Flag = false;
    // 外部输入角速度标志
    bool External_Omega_Flag = false;

    // 数据处理过程
    void Data_Process(uint8_t *Rx_Data_Buffer);

    // PID计算
    void PID_Calculate();

    // 电机数据输出到CAN总线发送缓冲区
    void Output();

    // 重置
    void Clear();
};

/**
 * @brief C620无刷电调
 */
class Class_Motor_DJI_C620
{
public:
    // PID角度环控制
    Class_PID PID_Angle;

    // PID角速度环控制
    Class_PID PID_Omega;

    /**
     * @brief 初始化C620无刷电调
     *
     * @param hfdcan CAN编号
     * @param __CAN_Rx_ID 电机反馈报文ID枚举
     * @param __Control_Method 电机控制方式
     * @param __Gearbox_Rate 减速箱减速比
     */
    void Init(FDCAN_HandleTypeDef *hfdcan, Enum_Motor_DJI_ID __CAN_Rx_ID,
              Enum_Motor_DJI_Control_Method __Control_Method = Motor_DJI_Control_Method_OMEGA,
              float __Gearbox_Rate = 3591.0f / 187.0f);

    /**
     * @brief 获取电机状态
     */
    inline Enum_Motor_DJI_Status Get_Status() const;

    /**
     * @brief 获取减速箱减速比
     */
    inline float Get_Gearbox_Rate() const;

    /**
     * @brief 获取减速前转矩常数, N*m/A
     */
    inline float Get_CURRENT_TO_TORQUE() const;

    /**
     * @brief 获取当前角度, rad
     */
    inline float Get_Now_Angle() const;

    /**
     * @brief 获取当前角速度, rad/s
     */
    inline float Get_Now_Omega() const;

    /**
     * @brief 获取当前电流, A
     */
    inline float Get_Now_Current() const;

    /**
     * @brief 获取当前温度, 摄氏度
     */
    inline float Get_Now_Temperature() const;

    /**
     * @brief 获取当前功率, W
     */
    inline float Get_Now_Power() const;

    /**
     * @brief 获取当前编码器值
     */
    inline uint16_t Get_Now_Encoder() const;

    /**
     * @brief 获取累计编码器值
     */
    inline int64_t Get_Total_Encoder() const;

    /**
     * @brief 获取累计圈数
     */
    inline int32_t Get_Total_Round() const;

    /**
     * @brief 获取CAN反馈报文ID
     */
    inline Enum_Motor_DJI_ID Get_CAN_Rx_ID() const;

    /**
     * @brief 获取控制方式
     */
    inline Enum_Motor_DJI_Control_Method Get_Control_Method() const;

    /**
     * @brief 获取目标角度, rad
     */
    inline float Get_Target_Angle() const;

    /**
     * @brief 获取目标角速度, rad/s
     */
    inline float Get_Target_Omega() const;

    /**
     * @brief 获取目标电流, A
     */
    inline float Get_Target_Current() const;

    /**
     * @brief 获取前馈角速度, rad/s
     */
    inline float Get_Feedforward_Omega() const;

    /**
     * @brief 获取前馈电流, A
     */
    inline float Get_Feedforward_Current() const;

    /**
     * @brief 获取当前外部输入角度, rad
     */
    inline float Get_External_Angle() const;

    /**
     * @brief 获取当前外部输入角速度, rad/s
     */
    inline float Get_External_Omega() const;

    /**
     * @brief 获取当前输出值
     */
    inline float Get_Out() const;

    /**
     * @brief 设置控制方式
     */
    inline void Set_Control_Method(Enum_Motor_DJI_Control_Method __Control_Method);

    /**
     * @brief 设置减速箱减速比
     */
    inline void Set_Gearbox_Rate(float __Gearbox_Rate);

    /**
     * @brief 设置目标角度值, rad
     */
    inline void Set_Target_Angle(float __Target_Angle);

    /**
     * @brief 设置目标角速度值, rad/s
     */
    inline void Set_Target_Omega(float __Target_Omega);

    /**
     * @brief 设置目标电流值, A
     */
    inline void Set_Target_Current(float __Target_Current);

    /**
     * @brief 设置角速度前馈值, rad/s
     */
    inline void Set_Feedforward_Omega(float __Feedforward_Omega);

    /**
     * @brief 设置电流前馈值, A
     */
    inline void Set_Feedforward_Current(float __Feedforward_Current);

    /**
     * @brief 设置外置角度值反馈, rad
     */
    inline void Set_Now_External_Angle(float __Now_External_Angle);

    /**
     * @brief 设置外置角速度值反馈, rad/s
     */
    inline void Set_Now_External_Omega(float __Now_External_Omega);

    /**
     * @brief CAN通信接收回调函数
     * @param Rx_Data 接收数据缓冲区
     */
    void CAN_RxCpltCallback(uint8_t *Rx_Data);

    /**
     * @brief TIM定时器中断定期检测电机是否存活
     */
    void TIM_100ms_Alive_PeriodElapsedCallback();

    /**
     * @brief TIM定时器中断计算回调函数
     */
    void TIM_Calculate_PeriodElapsedCallback();

protected:
    // 绑定的CAN管理对象
    Struct_CAN_Manage_Object *CAN_Manage_Object = nullptr;
    // 电机反馈给单片机的报文ID
    Enum_Motor_DJI_ID CAN_Rx_ID = Motor_DJI_ID_UNDEFINED;
    // 发送缓存区
    uint8_t *Tx_Data = nullptr;

    // 一圈编码器刻度
    const uint16_t ENCODER_NUM_PER_ROUND = 8192U;
    // 电流到输出的转换系数
    const float CURRENT_TO_OUT = 16384.0f / 20.0f;
    // 理论最大电流输出值
    const float THEORETICAL_OUTPUT_CURRENT_MAX = 16384.0f;
    // 减速前, 电流扭矩常数, N*m/A
    const float CURRENT_TO_TORQUE = 0.3f / (3591.0f / 187.0f);
    // 功率计算模型
    const Struct_Motor_DJI_Power_Model POWER_MODEL = {0.0f, 0.0f, 0.0f, 0.0f};
    // 减速箱减速比
    float Gearbox_Rate = 3591.0f / 187.0f;

    // 当前时刻的电机接收flag
    uint32_t Flag = 0;
    // 前一时刻的电机接收flag
    uint32_t Pre_Flag = 0;
    // 当前时刻的电机输出值
    float Out = 0.0f;

    // 电机状态
    Enum_Motor_DJI_Status Motor_Status = Motor_DJI_Status_DISABLE;
    // 电机对外接口信息
    Struct_Motor_DJI_Rx_Data Rx_Data = {};

    // 电机上层控制方式
    Enum_Motor_DJI_Control_Method Control_Method = Motor_DJI_Control_Method_OMEGA;

    // 目标角度, rad
    float Target_Angle = 0.0f;
    // 目标角速度, rad/s
    float Target_Omega = 0.0f;
    // 目标电流, A
    float Target_Current = 0.0f;

    // 前馈速度, rad/s
    float Feedforward_Omega = 0.0f;
    // 前馈电流, A
    float Feedforward_Current = 0.0f;

    // 当前外部输入角度, rad
    float Now_External_Angle = 0.0f;
    // 当前外部输入角速度, rad/s
    float Now_External_Omega = 0.0f;
    // 外部输入角度标志
    bool External_Angle_Flag = false;
    // 外部输入角速度标志
    bool External_Omega_Flag = false;

    // 数据处理过程
    void Data_Process(uint8_t *Rx_Data_Buffer);

    // PID计算
    void PID_Calculate();

    // 电机数据输出到CAN总线发送缓冲区
    void Output();

    // 重置
    void Clear();
};


/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief DJI电机CAN发送回调函数
 * @param hfdcan CAN编号
 */
void Motor_DJI_CAN_Tx_PeriodElapsedCallback(FDCAN_HandleTypeDef *hfdcan);

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 获取电机状态
 *
 * @return Enum_Motor_DJI_Status 电机状态
 */
inline Enum_Motor_DJI_Status Class_Motor_DJI_GM6020::Get_Status() const
{
    return (Motor_Status);
}

/**
 * @brief 获取转矩常数, N*m/A
 *
 * @return float 转矩常数
 */
inline float Class_Motor_DJI_GM6020::Get_CURRENT_TO_TORQUE() const
{
    return (CURRENT_TO_TORQUE);
}

/**
 * @brief 获取当前角度, rad
 *
 * @return float 当前角度
 */
inline float Class_Motor_DJI_GM6020::Get_Now_Angle() const
{
    return (Rx_Data.Now_Angle);
}

/**
 * @brief 获取当前角速度, rad/s
 *
 * @return float 当前角速度
 */
inline float Class_Motor_DJI_GM6020::Get_Now_Omega() const
{
    return (Rx_Data.Now_Omega);
}

/**
 * @brief 获取当前电流, A
 *
 * @return float 当前电流
 */
inline float Class_Motor_DJI_GM6020::Get_Now_Current() const
{
    return (Rx_Data.Now_Current);
}

/**
 * @brief 获取当前温度, 摄氏度
 *
 * @return float 当前温度
 */
inline float Class_Motor_DJI_GM6020::Get_Now_Temperature() const
{
    return (Rx_Data.Now_Temperature);
}

/**
 * @brief 获取当前功率, W
 *
 * @return float 当前功率
 */
inline float Class_Motor_DJI_GM6020::Get_Now_Power() const
{
    return (Rx_Data.Now_Power);
}

/**
 * @brief 获取当前编码器值
 *
 * @return uint16_t 当前编码器值
 */
inline uint16_t Class_Motor_DJI_GM6020::Get_Now_Encoder() const
{
    return (Rx_Data.Now_Encoder);
}

/**
 * @brief 获取累计编码器值
 *
 * @return int64_t 累计编码器值
 */
inline int64_t Class_Motor_DJI_GM6020::Get_Total_Encoder() const
{
    return (Rx_Data.Total_Encoder);
}

/**
 * @brief 获取累计圈数
 *
 * @return int32_t 累计圈数
 */
inline int32_t Class_Motor_DJI_GM6020::Get_Total_Round() const
{
    return (Rx_Data.Total_Round);
}

/**
 * @brief 获取CAN反馈报文ID
 *
 * @return Enum_Motor_DJI_ID CAN反馈报文ID
 */
inline Enum_Motor_DJI_ID Class_Motor_DJI_GM6020::Get_CAN_Rx_ID() const
{
    return (CAN_Rx_ID);
}

/**
 * @brief 获取编码器零点偏移
 *
 * @return int32_t 编码器零点偏移
 */
inline int32_t Class_Motor_DJI_GM6020::Get_Encoder_Offset() const
{
    return (Encoder_Offset);
}

/**
 * @brief 获取控制方式
 *
 * @return Enum_Motor_DJI_Control_Method 控制方式
 */
inline Enum_Motor_DJI_Control_Method Class_Motor_DJI_GM6020::Get_Control_Method() const
{
    return (Control_Method);
}

/**
 * @brief 获取目标角度, rad
 *
 * @return float 目标角度
 */
inline float Class_Motor_DJI_GM6020::Get_Target_Angle() const
{
    return (Target_Angle);
}

/**
 * @brief 获取目标角速度, rad/s
 *
 * @return float 目标角速度
 */
inline float Class_Motor_DJI_GM6020::Get_Target_Omega() const
{
    return (Target_Omega);
}

/**
 * @brief 获取目标电流, A
 *
 * @return float 目标电流
 */
inline float Class_Motor_DJI_GM6020::Get_Target_Current() const
{
    return (Target_Current);
}

/**
 * @brief 获取前馈角速度, rad/s
 *
 * @return float 前馈角速度
 */
inline float Class_Motor_DJI_GM6020::Get_Feedforward_Omega() const
{
    return (Feedforward_Omega);
}

/**
 * @brief 获取前馈电流, A
 *
 * @return float 前馈电流
 */
inline float Class_Motor_DJI_GM6020::Get_Feedforward_Current() const
{
    return (Feedforward_Current);
}

/**
 * @brief 获取当前外部输入角度, rad
 *
 * @return float 当前外部输入角度
 */
inline float Class_Motor_DJI_GM6020::Get_External_Angle() const
{
    return (Now_External_Angle);
}

/**
 * @brief 获取当前外部输入角速度, rad/s
 *
 * @return float 当前外部输入角速度
 */
inline float Class_Motor_DJI_GM6020::Get_External_Omega() const
{
    return (Now_External_Omega);
}

/**
 * @brief 获取当前输出值
 *
 * @return float 当前输出值
 */
inline float Class_Motor_DJI_GM6020::Get_Out() const
{
    return (Out);
}

/**
 * @brief 设置控制方式
 *
 * @param __Control_Method 控制方式
 */
inline void Class_Motor_DJI_GM6020::Set_Control_Method(Enum_Motor_DJI_Control_Method __Control_Method)
{
    Control_Method = __Control_Method;
}

/**
 * @brief 设置目标角度值, rad
 *
 * @param __Target_Angle 目标角度值
 */
inline void Class_Motor_DJI_GM6020::Set_Target_Angle(float __Target_Angle)
{
    Target_Angle = __Target_Angle;
}

/**
 * @brief 设定目标的速度, rad/s
 *
 * @param __Target_Omega 目标的速度, rad/s
 */
inline void Class_Motor_DJI_GM6020::Set_Target_Omega(float __Target_Omega)
{
    Target_Omega = __Target_Omega;
}

/**
 * @brief 设定目标的电流, A
 *
 * @param __Target_Current 目标的电流, A
 */
inline void Class_Motor_DJI_GM6020::Set_Target_Current(float __Target_Current)
{
    Target_Current = __Target_Current;
}

/**
 * @brief 设定前馈的速度, rad/s
 *
 * @param __Feedforward_Omega 前馈的速度, rad/s
 */
inline void Class_Motor_DJI_GM6020::Set_Feedforward_Omega(float __Feedforward_Omega)
{
    Feedforward_Omega = __Feedforward_Omega;
}

/**
 * @brief 设定前馈的电流, A
 *
 * @param __Feedforward_Current 前馈的电流, A
 */
inline void Class_Motor_DJI_GM6020::Set_Feedforward_Current(float __Feedforward_Current)
{
    Feedforward_Current = __Feedforward_Current;
}

/**
 * @brief 设定当前外部输入的角度值, rad
 *
 * @param __Now_External_Angle 当前外部输入的角度值, rad
 */
inline void Class_Motor_DJI_GM6020::Set_Now_External_Angle(float __Now_External_Angle)
{
    Now_External_Angle = __Now_External_Angle;
    External_Angle_Flag = true;
}

/**
 * @brief 设定当前外部输入的角速度值, rad/s
 *
 * @param __Now_External_Omega 当前外部输入的角速度值, rad/s
 */
inline void Class_Motor_DJI_GM6020::Set_Now_External_Omega(float __Now_External_Omega)
{
    Now_External_Omega = __Now_External_Omega;
    External_Omega_Flag = true;
}

/**
 * @brief 获取电机状态
 *
 * @return Enum_Motor_DJI_Status 电机状态
 */
inline Enum_Motor_DJI_Status Class_Motor_DJI_C610::Get_Status() const
{
    return (Motor_Status);
}

/**
 * @brief 获取减速箱减速比
 *
 * @return float 减速箱减速比
 */
inline float Class_Motor_DJI_C610::Get_Gearbox_Rate() const
{
    return (Gearbox_Rate);
}

/**
 * @brief 获取减速前转矩常数, N*m/A
 *
 * @return float 转矩常数
 */
inline float Class_Motor_DJI_C610::Get_CURRENT_TO_TORQUE() const
{
    return (CURRENT_TO_TORQUE);
}

/**
 * @brief 获取当前角度, rad
 *
 * @return float 当前角度
 */
inline float Class_Motor_DJI_C610::Get_Now_Angle() const
{
    return (Rx_Data.Now_Angle);
}

/**
 * @brief 获取当前角速度, rad/s
 *
 * @return float 当前角速度
 */
inline float Class_Motor_DJI_C610::Get_Now_Omega() const
{
    return (Rx_Data.Now_Omega);
}

/**
 * @brief 获取当前电流, A
 *
 * @return float 当前电流
 */
inline float Class_Motor_DJI_C610::Get_Now_Current() const
{
    return (Rx_Data.Now_Current);
}

/**
 * @brief 获取当前温度, 摄氏度
 *
 * @return float 当前温度
 */
inline float Class_Motor_DJI_C610::Get_Now_Temperature() const
{
    return (Rx_Data.Now_Temperature);
}

/**
 * @brief 获取当前功率, W
 *
 * @return float 当前功率
 */
inline float Class_Motor_DJI_C610::Get_Now_Power() const
{
    return (Rx_Data.Now_Power);
}

/**
 * @brief 获取当前编码器值
 *
 * @return uint16_t 当前编码器值
 */
inline uint16_t Class_Motor_DJI_C610::Get_Now_Encoder() const
{
    return (Rx_Data.Now_Encoder);
}

/**
 * @brief 获取累计编码器值
 *
 * @return int64_t 累计编码器值
 */
inline int64_t Class_Motor_DJI_C610::Get_Total_Encoder() const
{
    return (Rx_Data.Total_Encoder);
}

/**
 * @brief 获取累计圈数
 *
 * @return int32_t 累计圈数
 */
inline int32_t Class_Motor_DJI_C610::Get_Total_Round() const
{
    return (Rx_Data.Total_Round);
}

/**
 * @brief 获取CAN反馈报文ID
 *
 * @return Enum_Motor_DJI_ID CAN反馈报文ID
 */
inline Enum_Motor_DJI_ID Class_Motor_DJI_C610::Get_CAN_Rx_ID() const
{
    return (CAN_Rx_ID);
}

/**
 * @brief 获取控制方式
 *
 * @return Enum_Motor_DJI_Control_Method 控制方式
 */
inline Enum_Motor_DJI_Control_Method Class_Motor_DJI_C610::Get_Control_Method() const
{
    return (Control_Method);
}

/**
 * @brief 获取目标角度, rad
 *
 * @return float 目标角度
 */
inline float Class_Motor_DJI_C610::Get_Target_Angle() const
{
    return (Target_Angle);
}

/**
 * @brief 获取目标角速度, rad/s
 *
 * @return float 目标角速度
 */
inline float Class_Motor_DJI_C610::Get_Target_Omega() const
{
    return (Target_Omega);
}

/**
 * @brief 获取目标电流, A
 *
 * @return float 目标电流
 */
inline float Class_Motor_DJI_C610::Get_Target_Current() const
{
    return (Target_Current);
}

/**
 * @brief 获取前馈角速度, rad/s
 *
 * @return float 前馈角速度
 */
inline float Class_Motor_DJI_C610::Get_Feedforward_Omega() const
{
    return (Feedforward_Omega);
}

/**
 * @brief 获取前馈电流, A
 *
 * @return float 前馈电流
 */
inline float Class_Motor_DJI_C610::Get_Feedforward_Current() const
{
    return (Feedforward_Current);
}

/**
 * @brief 获取当前外部输入角度, rad
 *
 * @return float 当前外部输入角度
 */
inline float Class_Motor_DJI_C610::Get_External_Angle() const
{
    return (Now_External_Angle);
}

/**
 * @brief 获取当前外部输入角速度, rad/s
 *
 * @return float 当前外部输入角速度
 */
inline float Class_Motor_DJI_C610::Get_External_Omega() const
{
    return (Now_External_Omega);
}

/**
 * @brief 获取当前输出值
 *
 * @return float 当前输出值
 */
inline float Class_Motor_DJI_C610::Get_Out() const
{
    return (Out);
}

/**
 * @brief 设置控制方式
 *
 * @param __Control_Method 控制方式
 */
inline void Class_Motor_DJI_C610::Set_Control_Method(Enum_Motor_DJI_Control_Method __Control_Method)
{
    Control_Method = __Control_Method;
}

/**
 * @brief 设置减速箱减速比
 *
 * @param __Gearbox_Rate 减速箱减速比
 */
inline void Class_Motor_DJI_C610::Set_Gearbox_Rate(float __Gearbox_Rate)
{
    if (__Gearbox_Rate > 1.0e-6f)
    {
        Gearbox_Rate = __Gearbox_Rate;
    }
}

/**
 * @brief 设置目标角度值, rad
 *
 * @param __Target_Angle 目标角度值
 */
inline void Class_Motor_DJI_C610::Set_Target_Angle(float __Target_Angle)
{
    Target_Angle = __Target_Angle;
}

/**
 * @brief 设定目标的速度, rad/s
 *
 * @param __Target_Omega 目标的速度, rad/s
 */
inline void Class_Motor_DJI_C610::Set_Target_Omega(float __Target_Omega)
{
    Target_Omega = __Target_Omega;
}

/**
 * @brief 设定目标的电流, A
 *
 * @param __Target_Current 目标的电流, A
 */
inline void Class_Motor_DJI_C610::Set_Target_Current(float __Target_Current)
{
    Target_Current = __Target_Current;
}

/**
 * @brief 设定前馈的速度, rad/s
 *
 * @param __Feedforward_Omega 前馈的速度, rad/s
 */
inline void Class_Motor_DJI_C610::Set_Feedforward_Omega(float __Feedforward_Omega)
{
    Feedforward_Omega = __Feedforward_Omega;
}

/**
 * @brief 设定前馈的电流, A
 *
 * @param __Feedforward_Current 前馈的电流, A
 */
inline void Class_Motor_DJI_C610::Set_Feedforward_Current(float __Feedforward_Current)
{
    Feedforward_Current = __Feedforward_Current;
}

/**
 * @brief 设定当前外部输入的角度值, rad
 *
 * @param __Now_External_Angle 当前外部输入的角度值, rad
 */
inline void Class_Motor_DJI_C610::Set_Now_External_Angle(float __Now_External_Angle)
{
    Now_External_Angle = __Now_External_Angle;
    External_Angle_Flag = true;
}

/**
 * @brief 设定当前外部输入的角速度值, rad/s
 *
 * @param __Now_External_Omega 当前外部输入的角速度值, rad/s
 */
inline void Class_Motor_DJI_C610::Set_Now_External_Omega(float __Now_External_Omega)
{
    Now_External_Omega = __Now_External_Omega;
    External_Omega_Flag = true;
}

/**
 * @brief 获取电机状态
 *
 * @return Enum_Motor_DJI_Status 电机状态
 */
inline Enum_Motor_DJI_Status Class_Motor_DJI_C620::Get_Status() const
{
    return (Motor_Status);
}

/**
 * @brief 获取减速箱减速比
 *
 * @return float 减速箱减速比
 */
inline float Class_Motor_DJI_C620::Get_Gearbox_Rate() const
{
    return (Gearbox_Rate);
}

/**
 * @brief 获取减速前转矩常数, N*m/A
 *
 * @return float 转矩常数
 */
inline float Class_Motor_DJI_C620::Get_CURRENT_TO_TORQUE() const
{
    return (CURRENT_TO_TORQUE);
}

/**
 * @brief 获取当前角度, rad
 *
 * @return float 当前角度
 */
inline float Class_Motor_DJI_C620::Get_Now_Angle() const
{
    return (Rx_Data.Now_Angle);
}

/**
 * @brief 获取当前角速度, rad/s
 *
 * @return float 当前角速度
 */
inline float Class_Motor_DJI_C620::Get_Now_Omega() const
{
    return (Rx_Data.Now_Omega);
}

/**
 * @brief 获取当前电流, A
 *
 * @return float 当前电流
 */
inline float Class_Motor_DJI_C620::Get_Now_Current() const
{
    return (Rx_Data.Now_Current);
}

/**
 * @brief 获取当前温度, 摄氏度
 *
 * @return float 当前温度
 */
inline float Class_Motor_DJI_C620::Get_Now_Temperature() const
{
    return (Rx_Data.Now_Temperature);
}

/**
 * @brief 获取当前功率, W
 *
 * @return float 当前功率
 */
inline float Class_Motor_DJI_C620::Get_Now_Power() const
{
    return (Rx_Data.Now_Power);
}

/**
 * @brief 获取当前编码器值
 *
 * @return uint16_t 当前编码器值
 */
inline uint16_t Class_Motor_DJI_C620::Get_Now_Encoder() const
{
    return (Rx_Data.Now_Encoder);
}

/**
 * @brief 获取累计编码器值
 *
 * @return int64_t 累计编码器值
 */
inline int64_t Class_Motor_DJI_C620::Get_Total_Encoder() const
{
    return (Rx_Data.Total_Encoder);
}

/**
 * @brief 获取累计圈数
 *
 * @return int32_t 累计圈数
 */
inline int32_t Class_Motor_DJI_C620::Get_Total_Round() const
{
    return (Rx_Data.Total_Round);
}

/**
 * @brief 获取CAN反馈报文ID
 *
 * @return Enum_Motor_DJI_ID CAN反馈报文ID
 */
inline Enum_Motor_DJI_ID Class_Motor_DJI_C620::Get_CAN_Rx_ID() const
{
    return (CAN_Rx_ID);
}

/**
 * @brief 获取控制方式
 *
 * @return Enum_Motor_DJI_Control_Method 控制方式
 */
inline Enum_Motor_DJI_Control_Method Class_Motor_DJI_C620::Get_Control_Method() const
{
    return (Control_Method);
}

/**
 * @brief 获取目标角度, rad
 *
 * @return float 目标角度
 */
inline float Class_Motor_DJI_C620::Get_Target_Angle() const
{
    return (Target_Angle);
}

/**
 * @brief 获取目标角速度, rad/s
 *
 * @return float 目标角速度
 */
inline float Class_Motor_DJI_C620::Get_Target_Omega() const
{
    return (Target_Omega);
}

/**
 * @brief 获取目标电流, A
 *
 * @return float 目标电流
 */
inline float Class_Motor_DJI_C620::Get_Target_Current() const
{
    return (Target_Current);
}

/**
 * @brief 获取前馈角速度, rad/s
 *
 * @return float 前馈角速度
 */
inline float Class_Motor_DJI_C620::Get_Feedforward_Omega() const
{
    return (Feedforward_Omega);
}

/**
 * @brief 获取前馈电流, A
 *
 * @return float 前馈电流
 */
inline float Class_Motor_DJI_C620::Get_Feedforward_Current() const
{
    return (Feedforward_Current);
}

/**
 * @brief 获取当前外部输入角度, rad
 *
 * @return float 当前外部输入角度
 */
inline float Class_Motor_DJI_C620::Get_External_Angle() const
{
    return (Now_External_Angle);
}

/**
 * @brief 获取当前外部输入角速度, rad/s
 *
 * @return float 当前外部输入角速度
 */
inline float Class_Motor_DJI_C620::Get_External_Omega() const
{
    return (Now_External_Omega);
}

/**
 * @brief 获取当前输出值
 *
 * @return float 当前输出值
 */
inline float Class_Motor_DJI_C620::Get_Out() const
{
    return (Out);
}

/**
 * @brief 设置控制方式
 *
 * @param __Control_Method 控制方式
 */
inline void Class_Motor_DJI_C620::Set_Control_Method(Enum_Motor_DJI_Control_Method __Control_Method)
{
    Control_Method = __Control_Method;
}

/**
 * @brief 设置减速箱减速比
 *
 * @param __Gearbox_Rate 减速箱减速比
 */
inline void Class_Motor_DJI_C620::Set_Gearbox_Rate(float __Gearbox_Rate)
{
    if (__Gearbox_Rate > 1.0e-6f)
    {
        Gearbox_Rate = __Gearbox_Rate;
    }
}

/**
 * @brief 设置目标角度值, rad
 *
 * @param __Target_Angle 目标角度值
 */
inline void Class_Motor_DJI_C620::Set_Target_Angle(float __Target_Angle)
{
    Target_Angle = __Target_Angle;
}

/**
 * @brief 设定目标的速度, rad/s
 *
 * @param __Target_Omega 目标的速度, rad/s
 */
inline void Class_Motor_DJI_C620::Set_Target_Omega(float __Target_Omega)
{
    Target_Omega = __Target_Omega;
}

/**
 * @brief 设定目标的电流, A
 *
 * @param __Target_Current 目标的电流, A
 */
inline void Class_Motor_DJI_C620::Set_Target_Current(float __Target_Current)
{
    Target_Current = __Target_Current;
}

/**
 * @brief 设定前馈的速度, rad/s
 *
 * @param __Feedforward_Omega 前馈的速度, rad/s
 */ 
inline void Class_Motor_DJI_C620::Set_Feedforward_Omega(float __Feedforward_Omega)
{
    Feedforward_Omega = __Feedforward_Omega;
}

/**
 * @brief 设定前馈的电流, A
 *
 * @param __Feedforward_Current 前馈的电流, A
 */
inline void Class_Motor_DJI_C620::Set_Feedforward_Current(float __Feedforward_Current)
{
    Feedforward_Current = __Feedforward_Current;
}

/**
 * @brief 设定当前外部输入的角度值, rad
 *
 * @param __Now_External_Angle 当前外部输入的角度值, rad
 */
inline void Class_Motor_DJI_C620::Set_Now_External_Angle(float __Now_External_Angle)
{
    Now_External_Angle = __Now_External_Angle;
    External_Angle_Flag = true;
}

/**
 * @brief 设定当前外部输入的角速度值, rad/s
 *
 * @param __Now_External_Omega 当前外部输入的角速度值, rad/s
 */
inline void Class_Motor_DJI_C620::Set_Now_External_Omega(float __Now_External_Omega)
{
    Now_External_Omega = __Now_External_Omega;
    External_Omega_Flag = true;
}

#endif /* DVC_MOTOR_DJI_H */

/*----------------------------------------------------------------------------*/

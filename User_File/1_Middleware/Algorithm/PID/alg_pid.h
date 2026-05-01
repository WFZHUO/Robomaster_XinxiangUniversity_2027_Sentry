/**
 * @file alg_pid.h
 * @author WangFonzhuo
 * @brief PID通用算法
 * @version 1.0
 * @date 2026-05-01 27赛季
 */

#ifndef ALG_PID_H
#define ALG_PID_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief PID方向, 取决于控制量与被控制量成正相关还是负相关
 */
typedef enum
{
    PID_DIRECT = 0,
    PID_REVERSE,
} Enum_PID_Direction;

/**
 * @brief 微分先行
 */
typedef enum
{
    PID_D_First_DISABLE = 0,
    PID_D_First_ENABLE,
} Enum_PID_D_First;

/**
 * @brief 零位积分泄放, 目标与实际均为0时释放积分
 */
typedef enum
{
    PID_ZPIB_DISABLE = 0,
    PID_ZPIB_ENABLE,
} Enum_PID_Zero_Position_Integral_Bleeding;

/**
 * @brief PID控制器
 */
class Class_PID
{
public:
    /**
     * @brief 初始化PID控制器
     *
     * @param __K_P P值
     * @param __K_I I值
     * @param __K_D D值
     * @param __K_F 目标变化前馈
     * @param __I_Out_Max 积分项输出限幅, 0为不限制
     * @param __D_Out_Max 微分项输出限幅, 0为不限制
     * @param __Out_Max 总输出限幅, 0为不限制
     * @param __D_T 控制周期, 单位s
     * @param __Dead_Zone 死区, 0为不设置死区
     * @param __I_Variable_Speed_A 变速积分定速段阈值
     * @param __I_Variable_Speed_B 变速积分变速区间,范围：0<__I_Variable_Speed_A<__I_Variable_Speed_B，当__I_Variable_Speed_A = __I_Variable_Speed_B = 0时，代表不变速积分.
     * @param __I_Separate_Threshold 积分分离阈值, 0为不限制
     * @param __D_First 微分先行开关
     * @param __Direction PID方向
     * @param __D_Filter_Alpha D项低通滤波系数, 0~1, 0为不滤波
     * @param __Zero_Position_Integral_Bleeding 零位积分泄放开关
     */
    void Init(float __K_P, float __K_I, float __K_D, float __K_F = 0.0f,
              float __I_Out_Max = 0.0f, float __D_Out_Max = 0.0f, float __Out_Max = 0.0f,
              float __D_T = 0.001f,
              float __Dead_Zone = 0.0f,
              float __I_Variable_Speed_A = 0.0f, float __I_Variable_Speed_B = 0.0f,
              float __I_Separate_Threshold = 0.0f,
              Enum_PID_D_First __D_First = PID_D_First_DISABLE,
              Enum_PID_Direction __Direction = PID_DIRECT,
              float __D_Filter_Alpha = 0.0f,
              Enum_PID_Zero_Position_Integral_Bleeding __Zero_Position_Integral_Bleeding = PID_ZPIB_DISABLE);

    /**
     * @brief 获取积分误差
     */
    inline float Get_Integral_Error();

    /**
     * @brief 获取PID输出值
     */
    inline float Get_Out();

    /**
     * @brief 获取P输出值
     */
    inline float Get_P_Out();

    /**
     * @brief 获取I输出值
     */
    inline float Get_I_Out();

    /**
     * @brief 获取D输出值
     */
    inline float Get_D_Out();

    /**
     * @brief 获取F输出值
     */
    inline float Get_F_Out();

    /**
     * @brief 获取当前误差值
     */
    inline float Get_Error();

    /**
     * @brief 设定P
     */
    inline void Set_K_P(float __K_P);

    /**
     * @brief 设定I
     */
    inline void Set_K_I(float __K_I);

    /**
     * @brief 设定D
     */
    inline void Set_K_D(float __K_D);

    /**
     * @brief 设定前馈
     */
    inline void Set_K_F(float __K_F);

    /**
     * @brief 设定积分项输出限幅, 0为不限制
     */
    inline void Set_I_Out_Max(float __I_Out_Max);

    /**
     * @brief 设定D项输出限幅, 0为不限制
     */
    inline void Set_D_Out_Max(float __D_Out_Max);

    /**
     * @brief 设定总输出限幅, 0为不限制
     */
    inline void Set_Out_Max(float __Out_Max);

    /**
     * @brief 设定变速积分定速段阈值, 0为不限制
     */
    inline void Set_I_Variable_Speed_A(float __I_Variable_Speed_A);

    /**
     * @brief 设定变速积分变速区间, 0为不限制
     */
    inline void Set_I_Variable_Speed_B(float __I_Variable_Speed_B);

    /**
     * @brief 设定积分分离阈值, 0为不限制
     */
    inline void Set_I_Separate_Threshold(float __I_Separate_Threshold);

    /**
     * @brief 设定目标值
     */
    inline void Set_Target(float __Target);

    /**
     * @brief 设定当前值
     */
    inline void Set_Now(float __Now);

    /**
     * @brief 设定积分误差, 一般用于积分清零
     */
    inline void Set_Integral_Error(float __Integral_Error);

    /**
     * @brief 设置D项滤波系数
     */
    inline void Set_D_Filter_Alpha(float __D_Filter_Alpha);

    /**
     * @brief 设置零位积分泄放标志位
     */
    inline void Set_Zero_Position_Integral_Bleeding(Enum_PID_Zero_Position_Integral_Bleeding __Zero_Position_Integral_Bleeding);

    /**
     * @brief PID定时调整回调函数
     */
    void TIM_Adjust_PeriodElapsedCallback();

protected:
    // PID参数
    float K_P = 0.0f;
    float K_I = 0.0f;
    float K_D = 0.0f;
    float K_F = 0.0f;

    // PID限幅
    float I_Out_Max = 0.0f;
    float D_Out_Max = 0.0f;
    float Out_Max = 0.0f;

    // PID功能配置
    float D_T = 0.001f;
    float Dead_Zone = 0.0f;
    float I_Variable_Speed_A = 0.0f;
    float I_Variable_Speed_B = 0.0f;
    float I_Separate_Threshold = 0.0f;
    float D_Filter_Alpha = 0.0f;
    Enum_PID_D_First D_First = PID_D_First_DISABLE;
    Enum_PID_Direction Direction = PID_DIRECT;
    Enum_PID_Zero_Position_Integral_Bleeding Zero_Position_Integral_Bleeding = PID_ZPIB_DISABLE;

    // PID输入量
    float Target = 0.0f;
    float Now = 0.0f;

    // PID历史量
    float Pre_Now = 0.0f;
    float Pre_Target = 0.0f;
    float Pre_Out = 0.0f;
    float Pre_Error = 0.0f;

    // PID输出量
    float Out = 0.0f;
    float P_Out = 0.0f;
    float I_Out = 0.0f;
    float D_Out = 0.0f;
    float F_Out = 0.0f;
    float Error = 0.0f;

    // PID积分和滤波量
    float Integral_Error = 0.0f;
    float Filtered_D_Out = 0.0f;

    // 浮点绝对值
    inline float Abs_Float(float Value);

    // 浮点数限幅
    inline void Constrain_Float(float *Value, float Min, float Max);
};

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 获取积分误差
 */
inline float Class_PID::Get_Integral_Error()
{
    return (Integral_Error);
}

/**
 * @brief 获取PID输出值
 */
inline float Class_PID::Get_Out()
{
    return (Out);
}

/**
 * @brief 获取P输出值
 */
inline float Class_PID::Get_P_Out()
{
    return (P_Out);
}

/**
 * @brief 获取I输出值
 */
inline float Class_PID::Get_I_Out()
{
    return (I_Out);
}

/**
 * @brief 获取D输出值
 */
inline float Class_PID::Get_D_Out()
{
    return (D_Out);
}

/**
 * @brief 获取F输出值
 */
inline float Class_PID::Get_F_Out()
{
    return (F_Out);
}

/**
 * @brief 获取当前误差值
 */
inline float Class_PID::Get_Error()
{
    return (Error);
}

/**
 * @brief 设定PID的P
 */
inline void Class_PID::Set_K_P(float __K_P)
{
    K_P = __K_P;
}

/**
 * @brief 设定PID的I
 */
inline void Class_PID::Set_K_I(float __K_I)
{
    K_I = __K_I;
}

/**
 * @brief 设定PID的D
 */
inline void Class_PID::Set_K_D(float __K_D)
{
    K_D = __K_D;
}

/**
 * @brief 设定前馈
 */
inline void Class_PID::Set_K_F(float __K_F)
{
    K_F = __K_F;
}

/**
 * @brief 设定积分项输出限幅, 0为不限制
 */
inline void Class_PID::Set_I_Out_Max(float __I_Out_Max)
{
    I_Out_Max = __I_Out_Max;
}

/**
 * @brief 设定D项输出限幅, 0为不限制
 */
inline void Class_PID::Set_D_Out_Max(float __D_Out_Max)
{
    D_Out_Max = __D_Out_Max;
}

/**
 * @brief 设定总输出限幅, 0为不限制
 */
inline void Class_PID::Set_Out_Max(float __Out_Max)
{
    Out_Max = __Out_Max;
}

/**
 * @brief 设定变速积分定速段阈值, 0为不限制
 */
inline void Class_PID::Set_I_Variable_Speed_A(float __I_Variable_Speed_A)
{
    I_Variable_Speed_A = __I_Variable_Speed_A;
}

/**
 * @brief 设定变速积分变速区间, 0为不限制
 */
inline void Class_PID::Set_I_Variable_Speed_B(float __I_Variable_Speed_B)
{
    I_Variable_Speed_B = __I_Variable_Speed_B;
}

/**
 * @brief 设定积分分离阈值, 0为不限制
 */
inline void Class_PID::Set_I_Separate_Threshold(float __I_Separate_Threshold)
{
    I_Separate_Threshold = __I_Separate_Threshold;
}

/**
 * @brief 设定目标值
 */
inline void Class_PID::Set_Target(float __Target)
{
    Target = __Target;
}

/**
 * @brief 设定当前值
 */
inline void Class_PID::Set_Now(float __Now)
{
    Now = __Now;
}

/**
 * @brief 设定积分误差, 一般用于积分清零
 */
inline void Class_PID::Set_Integral_Error(float __Integral_Error)
{
    Integral_Error = __Integral_Error;
}

/**
 * @brief 设置D项滤波系数
 */
inline void Class_PID::Set_D_Filter_Alpha(float __D_Filter_Alpha)
{
    D_Filter_Alpha = __D_Filter_Alpha;
    Constrain_Float(&D_Filter_Alpha, 0.0f, 1.0f);
}

/**
 * @brief 设置零位积分泄放标志位
 */
inline void Class_PID::Set_Zero_Position_Integral_Bleeding(Enum_PID_Zero_Position_Integral_Bleeding __Zero_Position_Integral_Bleeding)
{
    Zero_Position_Integral_Bleeding = __Zero_Position_Integral_Bleeding;
}

/**
 * @brief 浮点绝对值
 */
inline float Class_PID::Abs_Float(float Value)
{
    if (Value < 0.0f)
    {
        return (-Value);
    }

    return (Value);
}

/**
 * @brief 浮点数限幅
 */
inline void Class_PID::Constrain_Float(float *Value, float Min, float Max)
{
    if (*Value < Min)
    {
        *Value = Min;
    }
    else if (*Value > Max)
    {
        *Value = Max;
    }
}

#endif /* ALG_PID_H */

/*----------------------------------------------------------------------------*/

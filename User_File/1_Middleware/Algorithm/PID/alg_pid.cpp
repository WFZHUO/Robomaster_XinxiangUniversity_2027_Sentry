/**
 * @file alg_pid.cpp
 * @author WangFonzhuo
 * @brief PID通用算法
 * @version 1.0
 * @date 2026-05-01 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "alg_pid.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 初始化PID控制器
 */
void Class_PID::Init(float __K_P, float __K_I, float __K_D, float __K_F,
                     float __I_Out_Max, float __D_Out_Max, float __Out_Max,
                     float __D_T,
                     float __Dead_Zone,
                     float __I_Variable_Speed_A, float __I_Variable_Speed_B,
                     float __I_Separate_Threshold,
                     Enum_PID_D_First __D_First,
                     Enum_PID_Direction __Direction,
                     float __D_Filter_Alpha,
                     Enum_PID_Zero_Position_Integral_Bleeding __Zero_Position_Integral_Bleeding)
{
    K_P = __K_P;
    K_I = __K_I;
    K_D = __K_D;
    K_F = __K_F;

    I_Out_Max = __I_Out_Max;
    D_Out_Max = __D_Out_Max;
    Out_Max = __Out_Max;

    if (__D_T <= 0.0f)
    {
        D_T = 0.001f;
    }
    else
    {
        D_T = __D_T;
    }

    Dead_Zone = __Dead_Zone;
    I_Variable_Speed_A = __I_Variable_Speed_A;
    I_Variable_Speed_B = __I_Variable_Speed_B;
    I_Separate_Threshold = __I_Separate_Threshold;
    D_Filter_Alpha = __D_Filter_Alpha;
    Constrain_Float(&D_Filter_Alpha, 0.0f, 1.0f);

    D_First = __D_First;
    Direction = __Direction;
    Zero_Position_Integral_Bleeding = __Zero_Position_Integral_Bleeding;
}

/**
 * @brief PID定时调整回调函数
 */
void Class_PID::TIM_Adjust_PeriodElapsedCallback()
{
    // 添加除零保护
    if (D_T <= 0.0f)
    {
        D_T = 0.001f;
    }

    P_Out = 0.0f;
    I_Out = 0.0f;
    D_Out = 0.0f;
    F_Out = 0.0f;
    Error = 0.0f;

    float abs_error;
    float speed_ratio;  // 线性变速积分

    Error = Target - Now;

    //根据方向调整误差符号
    if (Direction == PID_REVERSE)
    {
        Error = -Error;
    }

    abs_error = Abs_Float(Error);

    //判断死区
    if (abs_error < Dead_Zone)
    {
        Error = 0.0f;
        abs_error = 0.0f;
    }

    //计算p项

    P_Out = K_P * Error;

    //计算i项

    if ((I_Variable_Speed_A == 0.0f) && (I_Variable_Speed_B == 0.0f))
    {
        //非变速积分
        speed_ratio = 1.0f;
    }
    else
    {
        //变速积分
        if (abs_error <= I_Variable_Speed_A)
        {
            //误差小于A，正常积分
            speed_ratio = 1.0f;
        }
        else if (abs_error < (I_Variable_Speed_A + I_Variable_Speed_B))
        {
            //误差在A到A+B之间，线性递减
            speed_ratio = 1.0f - (abs_error - I_Variable_Speed_A) / I_Variable_Speed_B;
        }
        else
        {
            //误差大于A+B，停止积分
            speed_ratio = 0.0f;
        }
    }

    //零位积分泄放
    bool tmp_zpib_status = ((Abs_Float(Target) < Dead_Zone) &&
                            (abs_error < Dead_Zone) &&
                            (Zero_Position_Integral_Bleeding == PID_ZPIB_ENABLE));

    if (tmp_zpib_status == false)
    {
        //积分分离
        if (I_Separate_Threshold == 0.0f)
        {
            //没有积分分离
            Integral_Error += speed_ratio * D_T * Error;

            //如果开启积分限幅，那么在对积分项输出限幅的同时对积分误差也进行限幅，防止积分误差一直增大
            if ((K_I > 0.0f) && (I_Out_Max != 0.0f))
            {
                float tmp_integral_error_max = I_Out_Max / K_I;
                Constrain_Float(&Integral_Error, -tmp_integral_error_max, tmp_integral_error_max);
            }

            I_Out = K_I * Integral_Error;
            //积分限幅
            if (I_Out_Max != 0.0f)
            {
                Constrain_Float(&I_Out, -I_Out_Max, I_Out_Max);
            }
        }
        else
        {
            //积分分离使能
            if (abs_error < I_Separate_Threshold)
            {
                Integral_Error += speed_ratio * D_T * Error;

                //如果开启积分限幅，那么在对积分项输出限幅的同时对积分误差也进行限幅，防止积分误差一直增大
                if ((K_I > 0.0f) && (I_Out_Max != 0.0f))
                {
                    float tmp_integral_error_max = I_Out_Max / K_I;
                    Constrain_Float(&Integral_Error, -tmp_integral_error_max, tmp_integral_error_max);
                }

                I_Out = K_I * Integral_Error;
                //积分限幅
                if (I_Out_Max != 0.0f)
                {
                    Constrain_Float(&I_Out, -I_Out_Max, I_Out_Max);
                }
            }
            else
            {
                Integral_Error = 0.0f;
                I_Out = 0.0f;
            }
        }
    }
    else  // 为了防止静摩擦力的存在，当电机停止时，残留的积分项输出的力不足以克服积分项让电机旋转，但是残留积分却消耗功率,但是这个功能也不能盲目开，比如Pitch轴电机为了克服重力，就算速度为0，也要保持一个小的积分项输出，否则会导致电机旋转不稳定
    {
        //在死区内，不累积新的积分
        //缓慢释放已有积分        
        if (Abs_Float(Integral_Error) > 0.0001f)
        {
            //每周期衰减5%
            Integral_Error *= 0.95f;
        }
    }

    //计算d项

    float tmp_d_raw = 0.0f;

    if (D_First == PID_D_First_DISABLE)
    {
        // 没有微分先行
        tmp_d_raw = K_D * (Error - Pre_Error) / D_T;
    }
    else
    {
        // 微分先行, 对测量值微分, 避免目标阶跃导致D项冲击
        if (Direction == PID_DIRECT)
        {
            tmp_d_raw = -K_D * (Now - Pre_Now) / D_T;
        }
        else
        {
            tmp_d_raw = K_D * (Now - Pre_Now) / D_T;
        }
    }

    // D项滤波
    if (D_Filter_Alpha > 0.0f)
    {
        Filtered_D_Out = D_Filter_Alpha * tmp_d_raw + (1.0f - D_Filter_Alpha) * Filtered_D_Out;
        D_Out = Filtered_D_Out;
    }
    else
    {
        D_Out = tmp_d_raw;
    }

    // D项限幅
    if (D_Out_Max != 0.0f)
    {
        Constrain_Float(&D_Out, -D_Out_Max, D_Out_Max);
    }

    //计算前馈

    float tmp_target_delta = (Target - Pre_Target) / D_T;

    if (Direction == PID_REVERSE)
    {
        tmp_target_delta = -tmp_target_delta;
    }

    F_Out = K_F * tmp_target_delta;
    
    //计算总共的输出

    Out = P_Out + I_Out + D_Out + F_Out;
    //输出限幅
    if (Out_Max != 0.0f)
    {
        Constrain_Float(&Out, -Out_Max, Out_Max);
    }
    
    //善后工作
    Pre_Now = Now;
    Pre_Target = Target;
    Pre_Out = Out;
    Pre_Error = Error;
}

/*----------------------------------------------------------------------------*/

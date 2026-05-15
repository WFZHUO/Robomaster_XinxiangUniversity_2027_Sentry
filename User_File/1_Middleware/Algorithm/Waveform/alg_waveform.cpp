/**
 * @file alg_waveform.cpp
 * @author WangFonzhuo
 * @brief 常用测试波形发生器
 * @version 1.0
 * @date 2026-05-15 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "alg_waveform.h"
#include <math.h>

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 初始化波形发生器
 * @param __DT_s 控制周期, 单位s
 */
void Class_Waveform::Init(float __DT_s)
{
    Type = Waveform_Type_HOLD;
    Set_DT(__DT_s);

    Time_s = 0.0f;
    Output = 0.0f;
    Output_Min = -1.0e9f;
    Output_Max = 1.0e9f;

    Amplitude = 0.0f;
    Offset = 0.0f;
    Phase_01 = 0.0f;

    Step_Low = 0.0f;
    Step_High = 0.0f;
    Step_T_s = 0.0f;

    Square_Freq_Hz = 1.0f;
    Square_Duty = 0.5f;

    Sine_Freq_Hz = 1.0f;
    Wave_Freq_Hz = 1.0f;

    Ramp_Slope_Per_s = 0.0f;
    Ramp_Period_s = 0.0f;

    Pulse_Low = 0.0f;
    Pulse_High = 1.0f;
    Pulse_Period_s = 1.0f;
    Pulse_Width_s = 0.1f;

    Doublet_A = 1.0f;
    Doublet_T_Pos_s = 0.2f;
    Doublet_T_Zero_s = 0.2f;
    Doublet_T_Neg_s = 0.2f;
    Doublet_T_End_Zero_s = 0.0f;
    Doublet_Baseline = 0.0f;

    Chirp_A = 1.0f;
    Chirp_F0_Hz = 0.1f;
    Chirp_F1_Hz = 2.0f;
    Chirp_T_s = 5.0f;

    PRBS_A = 1.0f;
    PRBS_Bit_T_s = 0.05f;
    PRBS_Bit_Timer_s = 0.0f;
    PRBS_LFSR = 0xABCDEu;

    Noise_Amp = 0.1f;
    RNG_State = 0x12345678u;

    Relay_A = 1.0f;
    Relay_Hys = 0.0f;
    Relay_Bias = 0.0f;
    Relay_State = 1.0f;
}

/**
 * @brief 重置波形运行状态
 */
void Class_Waveform::Reset()
{
    Time_s = 0.0f;
    Phase_01 = 0.0f;
    PRBS_Bit_Timer_s = 0.0f;
    Relay_State = 1.0f;
}

/**
 * @brief 设置恒值波形
 *
 * @param __Value 保持的输出值
 */
void Class_Waveform::Hold(float __Value)
{
    Type = Waveform_Type_HOLD;
    Output = Basic_Math_Constrain(__Value, Output_Min, Output_Max);
}

/**
 * @brief 设置阶跃波形
 *
 * @param __Low 阶跃前输出值
 * @param __High 阶跃后输出值
 * @param __T_Step_s 阶跃时刻, 单位s
 */
void Class_Waveform::Step(float __Low, float __High, float __T_Step_s)
{
    Type = Waveform_Type_STEP;
    Time_s = 0.0f;
    Step_Low = __Low;
    Step_High = __High;
    Step_T_s = (__T_Step_s >= 0.0f) ? __T_Step_s : 0.0f;
}

/**
 * @brief 设置方波波形
 *
 * @param __Amplitude 幅值
 * @param __Frequency_Hz 频率, 单位Hz
 * @param __Offset 偏置
 * @param __Duty_0_1 占空比, 范围0~1
 * @param __Phase_01 初相位, 范围0~1
 */
void Class_Waveform::Square(float __Amplitude, float __Frequency_Hz, float __Offset, float __Duty_0_1, float __Phase_01)
{
    Type = Waveform_Type_SQUARE;
    Amplitude = Basic_Math_Abs(__Amplitude);
    Offset = __Offset;
    Square_Freq_Hz = (__Frequency_Hz > 0.0f) ? __Frequency_Hz : 0.0f;
    Square_Duty = Basic_Math_Constrain(__Duty_0_1, 0.0f, 1.0f);
    Phase_01 = Wrap_01(__Phase_01);
}

/**
 * @brief 设置正弦波波形
 *
 * @param __Amplitude 幅值
 * @param __Frequency_Hz 频率, 单位Hz
 * @param __Offset 偏置
 * @param __Phase_Deg 初相位, 单位度, 范围0~360
 */
void Class_Waveform::Sine(float __Amplitude, float __Frequency_Hz, float __Offset, float __Phase_Deg)
{
    Type = Waveform_Type_SINE;
    Amplitude = Basic_Math_Abs(__Amplitude);
    Offset = __Offset;
    Sine_Freq_Hz = (__Frequency_Hz > 0.0f) ? __Frequency_Hz : 0.0f;
    Phase_01 = Wrap_01(__Phase_Deg / 360.0f);
}

/**
 * @brief 设置三角波波形
 *
 * @param __Amplitude 幅值
 * @param __Frequency_Hz 频率, 单位Hz
 * @param __Offset 偏置
 * @param __Phase_01 初相位, 范围0~1
 */
void Class_Waveform::Triangle(float __Amplitude, float __Frequency_Hz, float __Offset, float __Phase_01)
{
    Type = Waveform_Type_TRIANGLE;
    Amplitude = Basic_Math_Abs(__Amplitude);
    Offset = __Offset;
    Wave_Freq_Hz = (__Frequency_Hz > 0.0f) ? __Frequency_Hz : 0.0f;
    Phase_01 = Wrap_01(__Phase_01);
}

/**
 * @brief 设置锯齿波波形
 *
 * @param __Amplitude 幅值
 * @param __Frequency_Hz 频率, 单位Hz
 * @param __Offset 偏置
 * @param __Phase_01 初相位, 范围0~1
 */
void Class_Waveform::Saw(float __Amplitude, float __Frequency_Hz, float __Offset, float __Phase_01)
{
    Type = Waveform_Type_SAW;
    Amplitude = Basic_Math_Abs(__Amplitude);
    Offset = __Offset;
    Wave_Freq_Hz = (__Frequency_Hz > 0.0f) ? __Frequency_Hz : 0.0f;
    Phase_01 = Wrap_01(__Phase_01);
}

/**
 * @brief 设置斜坡波形
 *
 * @param __Slope_Per_s 斜坡斜率, 单位每秒变化量
 * @param __Offset 偏置
 * @param __Period_s 周期, 单位s; 如果为0或负数则不循环
 */
void Class_Waveform::Ramp(float __Slope_Per_s, float __Offset, float __Period_s)
{
    Type = Waveform_Type_RAMP;
    Time_s = 0.0f;
    Ramp_Slope_Per_s = __Slope_Per_s;
    Offset = __Offset;
    Ramp_Period_s = (__Period_s > 0.0f) ? __Period_s : 0.0f;
}

/**
 * @brief 设置脉冲波形
 *
 * @param __Low 脉冲低电平
 * @param __High 脉冲高电平
 * @param __Period_s 周期, 单位s
 * @param __Width_s 脉冲宽度, 单位s
 */
void Class_Waveform::Pulse(float __Low, float __High, float __Period_s, float __Width_s)
{
    Type = Waveform_Type_PULSE;
    Time_s = 0.0f;
    Pulse_Low = __Low;
    Pulse_High = __High;
    Pulse_Period_s = (__Period_s > 0.0f) ? __Period_s : 1.0f;
    Pulse_Width_s = Basic_Math_Constrain(__Width_s, 0.0f, Pulse_Period_s);
}

/**
 * @brief 设置双脉冲波形
 *
 * @param __Amplitude 双脉冲幅值
 * @param __T_Pos_s 正脉冲持续时间, 单位s
 * @param __T_Zero_s 双脉冲中间的零输出持续时间, 单位s
 * @param __T_Neg_s 负脉冲持续时间, 单位s
 * @param __Baseline 基线值
 * @param __T_End_Zero_s 双脉冲结束后的零输出持续时间, 单位s; 0或负数表示不恢复到基线
 */
void Class_Waveform::Doublet(float __Amplitude, float __T_Pos_s, float __T_Zero_s, float __T_Neg_s, float __Baseline, float __T_End_Zero_s)
{
    Type = Waveform_Type_DOUBLET;
    Time_s = 0.0f;
    Doublet_A = Basic_Math_Abs(__Amplitude);
    Doublet_T_Pos_s = (__T_Pos_s > 0.0f) ? __T_Pos_s : 0.0f;
    Doublet_T_Zero_s = (__T_Zero_s > 0.0f) ? __T_Zero_s : 0.0f;
    Doublet_T_Neg_s = (__T_Neg_s > 0.0f) ? __T_Neg_s : 0.0f;
    Doublet_T_End_Zero_s = (__T_End_Zero_s > 0.0f) ? __T_End_Zero_s : 0.0f;
    Doublet_Baseline = __Baseline;
}

/**
 * @brief 设置线性扫频正弦波形
 *
 * @param __Amplitude 幅值
 * @param __F0_Hz 起始频率, 单位Hz
 * @param __F1_Hz 结束频率, 单位Hz
 * @param __Sweep_T_s 扫描时间, 单位s
 * @param __Offset 偏置
 */
void Class_Waveform::Chirp(float __Amplitude, float __F0_Hz, float __F1_Hz, float __Sweep_T_s, float __Offset)
{
    Type = Waveform_Type_CHIRP;
    Time_s = 0.0f;
    Phase_01 = 0.0f;
    Chirp_A = Basic_Math_Abs(__Amplitude);
    Chirp_F0_Hz = (__F0_Hz >= 0.0f) ? __F0_Hz : 0.0f;
    Chirp_F1_Hz = (__F1_Hz >= 0.0f) ? __F1_Hz : 0.0f;
    Chirp_T_s = (__Sweep_T_s > 0.0f) ? __Sweep_T_s : 1.0f;
    Offset = __Offset;
}

/**
 * @brief 设置伪随机二进制序列(PRBS)波形
 *
 * @param __Amplitude 幅值
 * @param __Bit_T_s 每个比特持续时间, 单位s
 * @param __Seed 伪随机数发生器种子, 非零值; 不同的种子会产生不同的PRBS序列
 * @param __Offset 偏置
 */
void Class_Waveform::PRBS(float __Amplitude, float __Bit_T_s, uint32_t __Seed, float __Offset)
{
    Type = Waveform_Type_PRBS;
    PRBS_A = Basic_Math_Abs(__Amplitude);
    PRBS_Bit_T_s = (__Bit_T_s > 0.0f) ? __Bit_T_s : 0.05f;
    PRBS_Bit_Timer_s = 0.0f;
    PRBS_LFSR = (__Seed != 0u) ? __Seed : 0xABCDEu;
    Offset = __Offset;
}

/**
 * @brief 设置白噪声波形
 *
 * @param __Noise_Amp 噪声幅值
 * @param __Seed 伪随机数发生器种子, 非零值; 不同的种子会产生不同的噪声序列
 * @param __Offset 偏置
 */
void Class_Waveform::Noise(float __Noise_Amp, uint32_t __Seed, float __Offset)
{
    Type = Waveform_Type_NOISE;
    Noise_Amp = Basic_Math_Abs(__Noise_Amp);
    RNG_State = (__Seed != 0u) ? __Seed : 0x12345678u;
    Offset = __Offset;
}

/**
 * @brief 设置继电器波形
 *
 * @param __Amplitude 输出幅值
 * @param __Hys 滞环宽度
 * @param __Bias 输出偏置
 */
void Class_Waveform::Relay(float __Amplitude, float __Hys, float __Bias)
{
    Type = Waveform_Type_RELAY;
    Relay_A = Basic_Math_Abs(__Amplitude);
    Relay_Hys = Basic_Math_Abs(__Hys);
    Relay_Bias = __Bias;
    Relay_State = 1.0f;
}

/**
 * @brief 更新波形输出
 * @param __Feedback 反馈值, 仅 Relay 波形需要使用
 * @return float 当前波形输出
 */
float Class_Waveform::Update(float __Feedback)
{
    float y = Output;

    switch (Type)
    {
        case Waveform_Type_HOLD:
        {
            y = Output;
        }
        break;

        case Waveform_Type_STEP:
        {
            y = (Time_s < Step_T_s) ? Step_Low : Step_High;
        }
        break;

        case Waveform_Type_SQUARE:
        {
            float tmp_high = (Phase_01 < Square_Duty) ? 1.0f : -1.0f;
            y = Offset + Amplitude * tmp_high;
            Phase_01 = Wrap_01(Phase_01 + Square_Freq_Hz * DT_s);
        }
        break;

        case Waveform_Type_SINE:
        {
            y = Offset + Amplitude * sinf(2.0f * WAVEFORM_PI * Phase_01);
            Phase_01 = Wrap_01(Phase_01 + Sine_Freq_Hz * DT_s);
        }
        break;

        case Waveform_Type_TRIANGLE:
        {
            float tmp_triangle = (Phase_01 < 0.5f) ? (4.0f * Phase_01 - 1.0f) : (3.0f - 4.0f * Phase_01);
            y = Offset + Amplitude * tmp_triangle;
            Phase_01 = Wrap_01(Phase_01 + Wave_Freq_Hz * DT_s);
        }
        break;

        case Waveform_Type_SAW:
        {
            y = Offset + Amplitude * (2.0f * Phase_01 - 1.0f);
            Phase_01 = Wrap_01(Phase_01 + Wave_Freq_Hz * DT_s);
        }
        break;

        case Waveform_Type_RAMP:
        {
            float tmp_time = (Ramp_Period_s > 0.0f) ? Positive_Mod(Time_s, Ramp_Period_s) : Time_s;
            y = Offset + Ramp_Slope_Per_s * tmp_time;
        }
        break;

        case Waveform_Type_PULSE:
        {
            float tmp_time = Positive_Mod(Time_s, Pulse_Period_s);
            y = (tmp_time < Pulse_Width_s) ? Pulse_High : Pulse_Low;
        }
        break;

        case Waveform_Type_DOUBLET:
        {
            float tmp_t1 = Doublet_T_Pos_s;
            float tmp_t2 = tmp_t1 + Doublet_T_Zero_s;
            float tmp_t3 = tmp_t2 + Doublet_T_Neg_s;
            float tmp_t4 = tmp_t3 + Doublet_T_End_Zero_s;
            float tmp_time = (tmp_t4 > 0.0f) ? Positive_Mod(Time_s, tmp_t4) : 0.0f;

            if (tmp_time < tmp_t1)
            {
                y = Doublet_Baseline + Doublet_A;
            }
            else if (tmp_time < tmp_t2)
            {
                y = Doublet_Baseline;
            }
            else if (tmp_time < tmp_t3)
            {
                y = Doublet_Baseline - Doublet_A;
            }
            else
            {
                y = Doublet_Baseline;
            }
        }
        break;

        case Waveform_Type_CHIRP:
        {
            float tmp_frequency = Chirp_F1_Hz;
            if (Time_s < Chirp_T_s)
            {
                float tmp_k = Time_s / Chirp_T_s;
                tmp_frequency = Chirp_F0_Hz + (Chirp_F1_Hz - Chirp_F0_Hz) * tmp_k;
            }

            y = Offset + Chirp_A * sinf(2.0f * WAVEFORM_PI * Phase_01);
            Phase_01 = Wrap_01(Phase_01 + tmp_frequency * DT_s);
        }
        break;

        case Waveform_Type_PRBS:
        {
            y = Offset + PRBS_A * ((PRBS_LFSR & 1u) ? 1.0f : -1.0f);

            PRBS_Bit_Timer_s += DT_s;
            while (PRBS_Bit_Timer_s >= PRBS_Bit_T_s)
            {
                PRBS_Bit_Timer_s -= PRBS_Bit_T_s;

                uint32_t tmp_lfsr = PRBS_LFSR;
                uint32_t tmp_lsb = tmp_lfsr & 1u;
                tmp_lfsr >>= 1;

                if (tmp_lsb != 0u)
                {
                    tmp_lfsr ^= 0x80200003u;
                }

                if (tmp_lfsr == 0u)
                {
                    tmp_lfsr = 0xABCDEu;
                }

                PRBS_LFSR = tmp_lfsr;
            }
        }
        break;

        case Waveform_Type_NOISE:
        {
            uint32_t tmp_random = XORSHIFT32(RNG_State);
            float tmp_uniform = static_cast<float>(tmp_random & 0xFFFFFFu) / static_cast<float>(0x1000000u);
            y = Offset + Noise_Amp * (2.0f * tmp_uniform - 1.0f);
        }
        break;

        case Waveform_Type_RELAY:
        {
            if (Relay_State > 0.0f)
            {
                if (__Feedback > Relay_Hys)
                {
                    Relay_State = -1.0f;
                }
            }
            else
            {
                if (__Feedback < -Relay_Hys)
                {
                    Relay_State = 1.0f;
                }
            }
            y = Relay_Bias + Relay_State * Relay_A;
        }
        break;

        default:
        {
            y = Output;
        }
        break;
    }

    Output = Basic_Math_Constrain(y, Output_Min, Output_Max);
    Time_s += DT_s;

    return Output;
}

/**
 * @brief 归一化相位到[0, 1)
 * @param __P 输入相位
 * @return float 归一化后的相位
 */
float Class_Waveform::Wrap_01(float __P)
{
    return Positive_Mod(__P, 1.0f);
}

/**
 * @brief 正取模
 * @param __X 输入值
 * @param __Modulus 模数
 * @return float 输出范围[0, __Modulus)
 */
float Class_Waveform::Positive_Mod(float __X, float __Modulus)
{
    if (__Modulus <= 0.0f)
    {
        return 0.0f;
    }

    float tmp = fmodf(__X, __Modulus);
    if (tmp < 0.0f)
    {
        tmp += __Modulus;
    }
    return tmp;
}

/**
 * @brief 32位XORSHIFT伪随机数发生器
 * @param __State 状态量
 * @return uint32_t 随机数
 */
uint32_t Class_Waveform::XORSHIFT32(uint32_t &__State)
{
    uint32_t tmp_x = __State;
    if (tmp_x == 0u)
    {
        tmp_x = 0x12345678u;
    }

    tmp_x ^= tmp_x << 13;
    tmp_x ^= tmp_x >> 17;
    tmp_x ^= tmp_x << 5;

    __State = tmp_x;
    return tmp_x;
}

/*----------------------------------------------------------------------------*/

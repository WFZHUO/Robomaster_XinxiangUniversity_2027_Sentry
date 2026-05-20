/**
 * @file alg_waveform.h
 * @author WangFonzhuo
 * @brief 常用测试波形发生器
 * @note  使用方法 ：
 *          0) 定义一个 Class_Waveform 类型的对象
 *          1) Init(dt)
 *          2) 选一个波形：Square/Sine/Step/...
 *          3) 每个周期调用 Update() 得到输出
 * @version 1.0
 * @date 2026-05-15 27赛季
 */

#ifndef ALG_WAVEFORM_H
#define ALG_WAVEFORM_H

/* Includes ------------------------------------------------------------------*/

#include "alg_basic.h"
#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

#ifndef WAVEFORM_PI
#define WAVEFORM_PI 3.14159265358979323846f
#endif

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 波形类型
 */
typedef enum
{
    Waveform_Type_HOLD = 0,   // 恒值
    Waveform_Type_STEP,       // 阶跃
    Waveform_Type_SQUARE,     // 方波
    Waveform_Type_SINE,       // 正弦
    Waveform_Type_TRIANGLE,   // 三角波
    Waveform_Type_SAW,        // 锯齿波
    Waveform_Type_RAMP,       // 斜坡
    Waveform_Type_PULSE,      // 脉冲
    Waveform_Type_DOUBLET,    // 双脉冲（+A -> 0 -> -A，可选末尾0）
    Waveform_Type_CHIRP,      // 扫频正弦
    Waveform_Type_PRBS,       // 伪随机二进制序列（±A）
    Waveform_Type_NOISE,      // 近似白噪声
    Waveform_Type_RELAY,      // 继电器激励（需要反馈输入）
} Enum_Waveform_Type;

/**
 * @brief 波形发生器类
 */
class Class_Waveform
{
public:
    /**
     * @brief 初始化波形发生器
     *
     * @param __DT_s 控制周期, 单位s
     */
    void Init(float __DT_s = 0.001f);

    /**
     * @brief 复位波形运行状态
     */
    void Reset();

    /**
     * @brief 获取当前波形类型
     */
    inline Enum_Waveform_Type Get_Type();

    /**
     * @brief 获取当前运行时间, 单位s
     */
    inline float Get_Time_s();

    /**
     * @brief 获取当前输出值
     */
    inline float Get_Output();

    /**
     * @brief 设置控制周期
     *
     * @param __DT_s 控制周期, 单位s
     */
    inline void Set_DT(float __DT_s);

    /**
     * @brief 设置输出限幅
     *
     * @param __Output_Min 输出下限
     * @param __Output_Max 输出上限
     */
    inline void Set_Limit(float __Output_Min, float __Output_Max);

    /**
     * @brief 设置恒值波形
     *
     * @param __Value 保持的输出值
     */
    void Hold(float __Value);

    /**
     * @brief 设置阶跃波形
     *
     * @param __Low 阶跃前输出值
     * @param __High 阶跃后输出值
     * @param __T_Step_s 阶跃时刻, 单位s
     */
    void Step(float __Low, float __High, float __T_Step_s);

    /**
     * @brief 设置方波波形
     *
     * @param __Amplitude 幅值
     * @param __Frequency_Hz 频率, 单位Hz
     * @param __Offset 偏置
     * @param __Duty_0_1 占空比, 范围0~1
     * @param __Phase_01 初相位, 范围0~1
     */
    void Square(float __Amplitude, float __Frequency_Hz, float __Offset = 0.0f, float __Duty_0_1 = 0.5f, float __Phase_01 = 0.0f);

    /**
     * @brief 设置正弦波形
     *
     * @param __Amplitude 幅值
     * @param __Frequency_Hz 频率, 单位Hz
     * @param __Offset 偏置
     * @param __Phase_Deg 初相位, 单位deg
     */
    void Sine(float __Amplitude, float __Frequency_Hz, float __Offset = 0.0f, float __Phase_Deg = 0.0f);

    /**
     * @brief 设置三角波波形
     *
     * @param __Amplitude 幅值
     * @param __Frequency_Hz 频率, 单位Hz
     * @param __Offset 偏置
     * @param __Phase_01 初相位, 范围0~1
     */
    void Triangle(float __Amplitude, float __Frequency_Hz, float __Offset = 0.0f, float __Phase_01 = 0.0f);

    /**
     * @brief 设置锯齿波波形
     *
     * @param __Amplitude 幅值
     * @param __Frequency_Hz 频率, 单位Hz
     * @param __Offset 偏置
     * @param __Phase_01 初相位, 范围0~1
     */
    void Saw(float __Amplitude, float __Frequency_Hz, float __Offset = 0.0f, float __Phase_01 = 0.0f);

    /**
     * @brief 设置斜坡波形
     *
     * @param __Slope_Per_s 斜率, 单位每秒
     * @param __Offset 偏置
     * @param __Period_s 周期, 单位s; 0为不循环
     */
    void Ramp(float __Slope_Per_s, float __Offset = 0.0f, float __Period_s = 0.0f);

    /**
     * @brief 设置脉冲波形
     *
     * @param __Low 低电平
     * @param __High 高电平
     * @param __Period_s 周期, 单位s
     * @param __Width_s 高电平宽度, 单位s
     */
    void Pulse(float __Low, float __High, float __Period_s, float __Width_s);

    /**
     * @brief 设置双脉冲波形
     *
     * @param __Amplitude 幅值
     * @param __T_Pos_s 正脉冲持续时间, 单位s
     * @param __T_Zero_s 中间零位持续时间, 单位s
     * @param __T_Neg_s 负脉冲持续时间, 单位s
     * @param __Baseline 基准值
     * @param __T_End_Zero_s 末尾零位持续时间, 单位s; 0为不额外保持零位
     */
    void Doublet(float __Amplitude, float __T_Pos_s, float __T_Zero_s, float __T_Neg_s, float __Baseline = 0.0f, float __T_End_Zero_s = 0.0f);

    /**
     * @brief 设置线性扫频正弦波形
     *
     * @param __Amplitude 幅值
     * @param __F0_Hz 起始频率, 单位Hz
     * @param __F1_Hz 终止频率, 单位Hz
     * @param __Sweep_T_s 扫频时间, 单位s
     * @param __Offset 偏置
     */
    void Chirp(float __Amplitude, float __F0_Hz, float __F1_Hz, float __Sweep_T_s, float __Offset = 0.0f);

    /**
     * @brief 设置伪随机二进制序列波形
     *
     * @param __Amplitude 幅值
     * @param __Bit_T_s 单个bit持续时间, 单位s
     * @param __Seed 随机种子, 不能为0
     * @param __Offset 偏置
     */
    void PRBS(float __Amplitude, float __Bit_T_s, uint32_t __Seed = 0xABCDEu, float __Offset = 0.0f);

    /**
     * @brief 设置均匀白噪声波形
     *
     * @param __Noise_Amp 噪声幅值
     * @param __Seed 随机种子, 不能为0
     * @param __Offset 偏置
     */
    void Noise(float __Noise_Amp, uint32_t __Seed = 0x12345678u, float __Offset = 0.0f);

    /**
     * @brief 设置继电器波形
     *
     * @param __Amplitude 输出幅值
     * @param __Hys 滞环宽度
     * @param __Bias 输出偏置
     */
    void Relay(float __Amplitude, float __Hys, float __Bias = 0.0f);

    /**
     * @brief 更新波形输出
     *
     * @param __Feedback 反馈值, 仅Relay波形使用
     * @return float 当前波形输出
     */
    float Update(float __Feedback = 0.0f);

protected:
    // 波形类型
    Enum_Waveform_Type Type = Waveform_Type_HOLD;

    // 波形时间参数
    float DT_s = 0.001f;
    float Time_s = 0.0f;

    // 波形输出量
    float Output = 0.0f;
    float Output_Min = -1.0e9f;
    float Output_Max = 1.0e9f;

    // 波形通用参数
    float Amplitude = 0.0f;
    float Offset = 0.0f;
    float Phase_01 = 0.0f;

    // 阶跃波参数
    float Step_Low = 0.0f;
    float Step_High = 0.0f;
    float Step_T_s = 0.0f;

    // 方波参数
    float Square_Freq_Hz = 1.0f;
    float Square_Duty = 0.5f;

    // 正弦波和普通周期波参数
    float Sine_Freq_Hz = 1.0f;
    float Wave_Freq_Hz = 1.0f;

    // 斜坡波参数
    float Ramp_Slope_Per_s = 0.0f;
    float Ramp_Period_s = 0.0f;

    // 脉冲波参数
    float Pulse_Low = 0.0f;
    float Pulse_High = 1.0f;
    float Pulse_Period_s = 1.0f;
    float Pulse_Width_s = 0.1f;

    // 双脉冲波参数
    float Doublet_A = 1.0f;
    float Doublet_T_Pos_s = 0.2f;
    float Doublet_T_Zero_s = 0.2f;
    float Doublet_T_Neg_s = 0.2f;
    float Doublet_T_End_Zero_s = 0.0f;
    float Doublet_Baseline = 0.0f;

    // 扫频波参数
    float Chirp_A = 1.0f;
    float Chirp_F0_Hz = 0.1f;
    float Chirp_F1_Hz = 2.0f;
    float Chirp_T_s = 5.0f;

    // PRBS参数
    float PRBS_A = 1.0f;
    float PRBS_Bit_T_s = 0.05f;
    float PRBS_Bit_Timer_s = 0.0f;
    uint32_t PRBS_LFSR = 0xABCDEu;

    // 噪声波参数
    float Noise_Amp = 0.1f;
    uint32_t RNG_State = 0x12345678u;

    // 继电器波参数
    float Relay_A = 1.0f;
    float Relay_Hys = 0.0f;
    float Relay_Bias = 0.0f;
    float Relay_State = 1.0f;

    // 归一化相位到[0, 1)
    float Wrap_01(float __P);

    // 正取模
    float Positive_Mod(float __X, float __Modulus);

    // 32位XORSHIFT伪随机数发生器
    uint32_t XORSHIFT32(uint32_t &__State);
};

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 获取当前波形类型
 */
inline Enum_Waveform_Type Class_Waveform::Get_Type()
{
    return Type;
}

/**
 * @brief 获取当前运行时间, 单位s
 */
inline float Class_Waveform::Get_Time_s()
{
    return Time_s;
}

/**
 * @brief 获取当前输出值
 */
inline float Class_Waveform::Get_Output()
{
    return Output;
}

/**
 * @brief 设置控制周期
 * @param __DT_s 控制周期, 单位s
 */
inline void Class_Waveform::Set_DT(float __DT_s)
{
    DT_s = (__DT_s > 0.0f) ? __DT_s : 0.001f;
}

/**
 * @brief 设置输出限幅
 * @param __Output_Min 输出下限
 * @param __Output_Max 输出上限
 */
inline void Class_Waveform::Set_Limit(float __Output_Min, float __Output_Max)
{
    if (__Output_Min <= __Output_Max)
    {
        Output_Min = __Output_Min;
        Output_Max = __Output_Max;
    }
    else
    {
        Output_Min = __Output_Max;
        Output_Max = __Output_Min;
    }
}

#endif /* ALG_WAVEFORM_H */

/*----------------------------------------------------------------------------*/

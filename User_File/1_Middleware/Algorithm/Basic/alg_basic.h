/**
 * @file alg_basic.h
 * @author yssickjgd
 * @author WangFonzhuo
 * @brief 基础数学库
 * @version 1.0
 * @date 2026-04-27 27赛季
 */

#ifndef ALG_BASIC_H
#define ALG_BASIC_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <float.h>

/* Exported macros -----------------------------------------------------------*/

/**
 * @brief rpm转rad/s的比例系数
 * @note rad/s = rpm * BASIC_MATH_RPM_TO_RADPS
 */
extern const float BASIC_MATH_RPM_TO_RADPS;

/**
 * @brief 角度转弧度的比例系数
 * @note rad = deg * BASIC_MATH_DEG_TO_RAD
 */
extern const float BASIC_MATH_DEG_TO_RAD;

/**
 * @brief 摄氏度转开氏度的偏移量
 * @note K = ℃ + BASIC_MATH_CELSIUS_TO_KELVIN
 */
extern const float BASIC_MATH_CELSIUS_TO_KELVIN;

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief 布尔值反转
 * @param Value 布尔值地址
 * @note 如果Value为空指针, 函数直接返回
 */
void Basic_Math_Boolean_Logical_Not(bool *Value);

/**
 * @brief 16位数据大小端原地转换
 * @param Address 数据地址, 指向至少2字节连续空间
 * @note 函数直接交换Address[0]和Address[1]
 */
void Basic_Math_Endian_Reverse_16(void *Address);

/**
 * @brief 16位数据大小端转换
 * @param Source 源数据地址, 指向至少2字节连续空间
 * @param Destination 目标存储地址, 可为nullptr; 不为空时写入反转后的2字节数据
 * @return uint16_t 按大端顺序组合后的16位数值
 * @note 如果Source为空指针, 返回0
 */
uint16_t Basic_Math_Endian_Reverse_16(void *Source, void *Destination = nullptr);

/**
 * @brief 32位数据大小端原地转换
 * @param Address 数据地址, 指向至少4字节连续空间
 * @note 函数直接交换Address[0]~Address[3]的字节顺序
 */
void Basic_Math_Endian_Reverse_32(void *Address);

/**
 * @brief 32位数据大小端转换
 * @param Source 源数据地址, 指向至少4字节连续空间
 * @param Destination 目标存储地址, 可为nullptr; 不为空时写入反转后的4字节数据
 * @return uint32_t 按大端顺序组合后的32位数值
 * @note 如果Source为空指针, 返回0
 */
uint32_t Basic_Math_Endian_Reverse_32(void *Source, void *Destination = nullptr);

/**
 * @brief 8位数组求和
 * @param Address 起始地址
 * @param Length 被加的数据数量, 注意不是字节数
 * @return uint8_t 累加和结果, 溢出时自然截断到8位
 * @note 如果Address为空指针, 返回0
 */
uint8_t Basic_Math_Sum_8(const uint8_t *Address, uint32_t Length);

/**
 * @brief 16位数组求和
 * @param Address 起始地址
 * @param Length 被加的数据数量, 注意不是字节数
 * @return uint16_t 累加和结果, 溢出时自然截断到16位
 * @note 如果Address为空指针, 返回0
 */
uint16_t Basic_Math_Sum_16(const uint16_t *Address, uint32_t Length);

/**
 * @brief 32位数组求和
 * @param Address 起始地址
 * @param Length 被加的数据数量, 注意不是字节数
 * @return uint32_t 累加和结果, 溢出时自然截断到32位
 * @note 如果Address为空指针, 返回0
 */
uint32_t Basic_Math_Sum_32(const uint32_t *Address, uint32_t Length);

/**
 * @brief sinc函数
 * @param x 输入, 单位为弧度
 * @return float sinc(x) = sin(x) / x; 当x接近0时返回1
 * @note 内部使用CMSIS-DSP的arm_sin_f32
 */
float Basic_Math_Sinc(float x);

/**
 * @brief 将浮点数线性映射到整型区间
 * @param x 输入浮点数
 * @param Float_1 输入浮点区间端点1
 * @param Float_2 输入浮点区间端点2
 * @param Int_1 输出整型区间端点1
 * @param Int_2 输出整型区间端点2
 * @return int32_t 映射后的整型数值
 * @note 如果Float_1与Float_2过近, 返回Int_1
 */
int32_t Basic_Math_Float_To_Int(float x, float Float_1, float Float_2, int32_t Int_1, int32_t Int_2);

/**
 * @brief 将整型数线性映射到浮点区间
 * @param x 输入整型数
 * @param Int_1 输入整型区间端点1
 * @param Int_2 输入整型区间端点2
 * @param Float_1 输出浮点区间端点1
 * @param Float_2 输出浮点区间端点2
 * @return float 映射后的浮点数值
 * @note 如果Int_1等于Int_2, 返回Float_1
 */
float Basic_Math_Int_To_Float(int32_t x, int32_t Int_1, int32_t Int_2, float Float_1, float Float_2);

/**
 * @brief 判断浮点数是否为无效浮点数
 * @param x 输入浮点数
 * @return true 无效浮点数
 * @return false 正常浮点数或正负零
 * @note 这里将非正规数、无穷大、NaN都视为无效浮点数
 */
bool Basic_Math_Is_Invalid_Float(float x);

/**
 * @brief 求取模归化
 * @param x 传入数据
 * @param modulus 模数
 * @return float 归化后的数值, 范围为[-modulus / 2, modulus / 2)
 * @note 常用于角度归一化, 例如将弧度角归一化到[-PI, PI)
 */
float Basic_Math_Modulus_Normalization(float x, float modulus);

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 限幅函数
 * @tparam Type 数据类型
 * @param x 传入数据
 * @param Min 最小值
 * @param Max 最大值
 * @return Type 限幅后的输出值
 * @note 该函数不会修改原变量
 */
template<typename Type>
Type Basic_Math_Constrain(Type x, Type Min, Type Max)
{
    if (x < Min)
    {
        x = Min;
    }
    else if (x > Max)
    {
        x = Max;
    }

    return x;
}

/**
 * @brief 限幅函数
 * @tparam Type 数据类型
 * @param x 传入数据地址
 * @param Min 最小值
 * @param Max 最大值
 * @return Type 限幅后的输出值
 * @note 该函数会直接修改x指向的变量; 如果x为空指针, 返回Min
 */
template<typename Type>
Type Basic_Math_Constrain(Type *x, Type Min, Type Max)
{
    if (x == nullptr)
    {
        return Min;
    }

    if (*x < Min)
    {
        *x = Min;
    }
    else if (*x > Max)
    {
        *x = Max;
    }

    return *x;
}

/**
 * @brief 求绝对值
 * @tparam Type 数据类型
 * @param x 传入数据
 * @return Type x的绝对值
 * @note 对无符号类型没有实际意义; 对有符号整型最小负数存在溢出风险
 */
template<typename Type>
Type Basic_Math_Abs(Type x)
{
    return ((x >= static_cast<Type>(0)) ? x : static_cast<Type>(-x));
}

#endif /* ALG_BASIC_H */

/*----------------------------------------------------------------------------*/

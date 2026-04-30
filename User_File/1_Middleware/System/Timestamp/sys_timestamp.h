/**
 * @file sys_timestamp.h
 * @author WangFonzhuo
 * @brief 时间计算相关支持库
 * @version 1.0
 * @date 2026-04-25 27赛季
 */

/**
 * 要求: 使能并绑定一个定时器, 开外部中断, PSC分频到1MHz, ARR为3600000000
 * 保证arr计数器1us增一次, 1h触发一次外部中断
 */

#ifndef SYS_TIMESTAMP_H
#define SYS_TIMESTAMP_H

/* Includes ------------------------------------------------------------------*/

#include "tim.h"
#include "stm32h7xx_hal.h"

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 系统时间戳
 *
 */
class Class_Timestamp
{
public:
    /**
     * @brief 初始化时间戳
     * @param htim 定时器句柄
     */
    void Init(TIM_HandleTypeDef *htim);

    /**
     * @brief 获取当前时间戳
     * @return 当前时间戳
     */
    inline uint64_t Get_Current_Timestamp() const;

    /**
     * @brief 获取当前秒数
     * @return 当前秒数
     */
    inline float Get_Now_Second() const;

    /**
     * @brief 获取当前毫秒数
     * @return 当前毫秒数
     */
    inline float Get_Now_Millisecond() const;

    /**
     * @brief 获取当前微秒数
     * @return 当前微秒数
     */
    inline uint64_t Get_Now_Microsecond() const;

    /**
     * @brief 定时器周期回调函数
     */
    void TIM_3600s_PeriodElapsedCallback();

protected:
    // 初始化相关常量
    TIM_HandleTypeDef *TIM_Handler = nullptr;

    // 定时器溢出计数, 一小时溢出一次
    volatile uint32_t TIM_Overflow_Count = 0;

    // 计算当前时间戳
    uint64_t Calculate_Timestamp() const;
};

/**
 * @brief Reusable, 时间对象, 可以为负数
 *
 */
class Class_Time
{
public:
    /**
     * @brief 构造函数
     */
    Class_Time()
    {
        Microsecond = 0;
    }

    /**
     * @brief 显式构造函数
     * @param __Microsecond 微秒数
     */
    explicit Class_Time(const int64_t &__Microsecond)
    {
        Microsecond = __Microsecond;
    }

    /**
     * @brief 构造函数
     * @param __Second 秒数
     * @param __Millisecond 毫秒数
     * @param __Microsecond 微秒数
     */
    Class_Time(const int32_t &__Second, const int16_t &__Millisecond, const int16_t &__Microsecond)
    {
        Microsecond = (int64_t)(__Second) * 1000000LL + (int64_t)(__Millisecond) * 1000LL + (int64_t)(__Microsecond);
    }

    /**
     * @brief 复制构造函数
     * @param Time 时间对象
     */
    Class_Time(const Class_Time &Time)
    {
        Microsecond = Time.Microsecond;
    }

    /**
     * @brief 移动构造函数
     * @param Time 时间对象
     */
    Class_Time(Class_Time &&Time) noexcept
    {
        Microsecond = Time.Microsecond;
    }

    /**
     * @brief 析构函数
     */
    ~Class_Time() = default;

    /**
     * @brief 复制赋值函数
     * @param Time 时间对象
     * @return 更新后的时间对象
     */
    Class_Time &operator=(const Class_Time &Time)
    {
        if (this != &Time)
        {
            Microsecond = Time.Microsecond;
        }
        return (*this);
    }

    /**
     * @brief 移动赋值函数
     * @param Time 时间对象
     * @return 更新后的时间对象
     */
    Class_Time &operator=(Class_Time &&Time) noexcept
    {
        Microsecond = Time.Microsecond;
        return (*this);
    }

    // 运算符重载
    inline Class_Time operator+(const Class_Time &Time) const
    {
        return (Class_Time(Microsecond + Time.Get_Microsecond()));
    }

    inline Class_Time operator-(const Class_Time &Time) const
    {
        return (Class_Time(Microsecond - Time.Get_Microsecond()));
    }

    inline Class_Time operator*(const float &value) const
    {
        return (Class_Time((int64_t)((float) (Microsecond) * value)));
    }

    inline friend Class_Time operator*(const float &value, const Class_Time &Time)
    {
        return (Class_Time((int64_t)((float) (Time.Get_Microsecond()) * value)));
    }

    inline Class_Time operator/(const float &value) const
    {
        return (Class_Time((int64_t)((float) (Microsecond) / value)));
    }

    inline Class_Time &operator+=(const Class_Time &Time)
    {
        Microsecond += Time.Get_Microsecond();
        return (*this);
    }

    inline Class_Time &operator-=(const Class_Time &Time)
    {
        Microsecond -= Time.Get_Microsecond();
        return (*this);
    }

    inline Class_Time &operator*=(const float &value)
    {
        Microsecond = (int64_t)((float) (Microsecond) * value);
        return (*this);
    }

    inline Class_Time &operator/=(const float &value)
    {
        Microsecond = (int64_t)((float) (Microsecond) / value);
        return (*this);
    }

    inline bool operator>(const Class_Time &Time) const
    {
        return (Microsecond > Time.Get_Microsecond());
    }

    inline bool operator<(const Class_Time &Time) const
    {
        return (Microsecond < Time.Get_Microsecond());
    }

    inline bool operator>=(const Class_Time &Time) const
    {
        return (Microsecond >= Time.Get_Microsecond());
    }

    inline bool operator<=(const Class_Time &Time) const
    {
        return (Microsecond <= Time.Get_Microsecond());
    }

    inline bool operator==(const Class_Time &Time) const
    {
        return (Microsecond == Time.Get_Microsecond());
    }

    inline bool operator!=(const Class_Time &Time) const
    {
        return (Microsecond != Time.Get_Microsecond());
    }

    /**
     * @brief 获取时间, 单位秒
     * @return float 时间, 单位秒
     */
    inline float Get_Second() const;

    /**
     * @brief 获取时间, 单位毫秒
     * @return float 时间, 单位毫秒
     */
    inline float Get_Millisecond() const;

    /**
     * @brief 获取时间, 单位微秒
     * @return int64_t 时间, 单位微秒
     */
    inline int64_t Get_Microsecond() const;

protected:
    // 微秒
    int64_t Microsecond = 0;
};

/* Exported variables --------------------------------------------------------*/

// 声明全局时间戳对象
extern Class_Timestamp SYS_Timestamp;

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief SYS_Timestamp相关的延迟函数, 以秒、毫秒、微秒为单位
 */
namespace Namespace_SYS_Timestamp
{
    void Delay_Second(uint32_t Second);

    void Delay_Millisecond(uint32_t Millisecond);

    void Delay_Microsecond(uint32_t Microsecond);
}

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 获取当前时间
 *
 * @return uint64_t 当前时间
 */
inline uint64_t Class_Timestamp::Get_Current_Timestamp() const
{
    return (Calculate_Timestamp());
}

/**
 * @brief 获取当前时间, 单位秒
 *
 * @return float 当前时间, 单位秒
 */
inline float Class_Timestamp::Get_Now_Second() const
{
    return ((float) (Calculate_Timestamp()) / 1000000.0f);
}

/**
 * @brief 获取当前时间, 单位毫秒
 *
 * @return float 当前时间, 单位毫秒
 */
inline float Class_Timestamp::Get_Now_Millisecond() const
{
    return ((float) (Calculate_Timestamp()) / 1000.0f);
}

/**
 * @brief 获取当前时间, 单位微秒
 *
 * @return uint64_t 当前时间, 单位微秒
 */
inline uint64_t Class_Timestamp::Get_Now_Microsecond() const
{
    return (Calculate_Timestamp());
}

/**
 * @brief 获取时间, 单位秒
 *
 * @return float 时间, 单位秒
 */
inline float Class_Time::Get_Second() const
{
    return ((float) Microsecond / 1000000.0f);
}

/**
 * @brief 获取时间, 单位毫秒
 *
 * @return float 时间, 单位毫秒
 */
inline float Class_Time::Get_Millisecond() const
{
    return ((float) Microsecond / 1000.0f);
}

/**
 * @brief 获取时间, 单位微秒
 *
 * @return int64_t 时间, 单位微秒
 */
inline int64_t Class_Time::Get_Microsecond() const
{
    return (Microsecond);
}

#endif /* SYS_TIMESTAMP_H */

/*----------------------------------------------------------------------------*/

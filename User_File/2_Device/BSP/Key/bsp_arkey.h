/**
 * @file bsp_arkey.h
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief Ark按键驱动. 模仿电脑鼠标按键逻辑
 * @version 1.0
 * @date 2026-01-24
 * @copyright Wang Fangzhuo (c) 2026
 *
 * 说明:
 * 1. 建议在定时器中以按键扫描周期调用:
 *      Class_ArkKey::TIM_Update_PeriodElapsedCallback();
 * 2. 逻辑函数读取完标志位后调用:
 *      Class_ArkKey::ClearAllFlags();
 */

#ifndef DVC_ARKEY_REFACTOR_H
#define DVC_ARKEY_REFACTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

/* Exported macros -----------------------------------------------------------*/

// 按键扫描周期(ms) - 需与你的定时器调用周期一致
#define UPDATE_KEY_STATE_MS     (15)

// 双击最大间隔(ms)
#define INTERVAL_THRESHOLD_MS   (150)

// 长按阈值(ms)
#define HOLD_THRESHOLD_MS       (300)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 按键状态机状态
 *
 */
typedef enum
{
    ArkKey_State_DOWN = 0,   // 按下瞬间(下降沿)
    ArkKey_State_UP,         // 松手瞬间(上升沿)
    ArkKey_State_PRESSED,    // 持续按下(低电平)
    ArkKey_State_RELEASED,   // 持续释放(高电平)
} Enum_ArkKey_State;

/**
 * @brief Ark按键对象
 *
 * 说明:
 * - 每个对象对应一个实体按键(一个GPIO+PIN)
 * - 内部使用静态链表统一管理所有对象, 以便批量扫描
 * - 外部使用 isPressed/isUp/isHold/isMultiClick/isReleased/clickState 等标志位
 *
 */
class Class_ArkKey
{
public:
    /* 用户接口-----------------------------------------------------------*/

    bool isPressed = false;     // 按下瞬间触发标志
    bool isHold = false;        // 长按事件标志
    bool isMultiClick = false;  // 连击事件标志
    bool isUp = false;          // 按键释放瞬间标志
    bool isReleased = false;    // 按键持续释放标志
    uint8_t clickState = 0;     // 点击状态(0=无操作 1=单击 2=双击...)
    
    void Init(GPIO_TypeDef *GPIOX, uint16_t GPIO_Pin_x);
    inline Enum_ArkKey_State GetArkKeyState();
    void ClearFlags();

    /* 批量处理接口----------------------------------------*/

    static void ClearAllFlags();
    static void ScanKeyState();
    static void UserApp();
    static void TIM_Update_PeriodElapsedCallback();

protected:
    /* 内部状态管理 ----------------------------------------------------------*/

    uint8_t value = 1;           // 当前GPIO电平缓存(0=按下 1=未按)
    uint8_t preValue = 1;        // 上一次扫描缓存
    Class_ArkKey *next = nullptr;// 链表指针(用于多按键管理)

    uint32_t holdTime = 0;       // 长按判定时间(ms)
    uint32_t intervalTime = 0;   // 连击间隔时间(ms)
    uint8_t clickCount = 0;      // 点击次数

    // 按键GPIO信息
    GPIO_TypeDef *gpio = nullptr;
    uint16_t pin = 0;

    // 按键状态机
    Enum_ArkKey_State state = ArkKey_State_RELEASED;

    //静态链表管理,note: head/tail 为类静态成员，全局仅一份，用于把所有按键对象串成链表
    static Class_ArkKey *head;
    static Class_ArkKey *tail;

    //按键去抖函数
    static void DebounceFilter();

};

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief 获取按键状态
 *
 * @return Enum_ArkKey_State 按键状态
 */
inline Enum_ArkKey_State Class_ArkKey::GetArkKeyState()
{
    return (state);
}

/* Exported function definitions ---------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* DVC_ARKEY_REFACTOR_H */

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO *********************/


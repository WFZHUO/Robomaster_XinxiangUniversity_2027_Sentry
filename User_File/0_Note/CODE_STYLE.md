# 27赛季设备库代码编写规范

> 本规范以 `dvc_motor_dji.h/.cpp` 当前风格为基准。后续新增 Device 层设备库、驱动适配库、控制型设备类，优先按本文档执行。  
> 目标不是写成“最复杂的规范”，而是让所有库看起来像同一个人、同一个赛季、同一个工程写出来的。

---

## 0. 总原则

1. **风格统一优先**：后续新增库优先模仿 `dvc_motor_dji` 的文件结构、命名、注释和函数组织方式。
2. **接口清楚，内部稳定**：`.h` 文件放对外类型、类声明、函数声明和简单 `inline`；`.cpp` 文件放具体实现。
3. **注释说明职责和意图**：注释用中文，解释“这个变量/函数承担什么职责”，不写大段学习笔记。
4. **Device 层只做设备层的事**：Device 层可以解析协议、维护设备状态、做离线检测和基础控制接口；不要写整车业务状态机。
5. **能防低级错误，但不无限防御**：空指针、非法外设、非法参数要保护；极低概率边界不要把库写复杂。

---

## 1. 文件组织规范

### 1.1 头文件 `.h` 结构

头文件统一使用以下结构：

```cpp
/**
 * @file dvc_xxx.h
 * @author WangFonzhuo
 * @brief XXX的配置与操作
 * @version 1.0
 * @date 2026-xx-xx 27赛季
 */

#ifndef DVC_XXX_H
#define DVC_XXX_H

/* Includes ------------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/* Exported function definitions ---------------------------------------------*/

#endif

/*----------------------------------------------------------------------------*/
```

规则：

1. 区块标题即使为空，也保留。
2. `Includes` 放对外接口必须依赖的头文件。
3. `Exported types` 放 `enum`、`struct`、`class`。
4. `Exported function prototypes` 放普通对外函数声明。
5. `Exported function definitions` 只放 `inline` getter/setter 或必须在头文件实现的小函数。
6. 不需要 C 调用的 C++ 头文件，不加 `extern "C"`。

---

### 1.2 源文件 `.cpp` 结构

源文件统一使用以下结构：

```cpp
/**
 * @file dvc_xxx.cpp
 * @author WangFonzhuo
 * @brief XXX的配置与操作
 * @version 1.0
 * @date 2026-xx-xx 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "dvc_xxx.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
```

规则：

1. `.cpp` 必须先 include 自己的 `.h`。
2. 只在内部实现中使用的头文件，放 `.cpp` 中。
3. 全局缓存、全局管理对象放在 `Variables` 区。
4. `static` 私有函数原型放在 `Function prototypes` 区。
5. 所有函数定义放在 `Function definitions` 区。

---

## 2. 文件命名规范

文件名按工程层级统一：

```text
drv_xxx.h / drv_xxx.cpp      Driver 层，外设通用封装
bsp_xxx.h / bsp_xxx.cpp      BSP 层，板级硬件支持
alg_xxx.h / alg_xxx.cpp      Algorithm 层，算法工具
sys_xxx.h / sys_xxx.cpp      System 层，系统级工具
dvc_xxx.h / dvc_xxx.cpp      Device 层，具体设备协议和状态维护
```

Device 层示例：

```text
dvc_motor_dji.h
dvc_motor_dji.cpp
dvc_referee.h
dvc_referee.cpp
dvc_dr16.h
dvc_dr16.cpp
```

规则：

1. 一个设备库使用一组 `.h/.cpp`。
2. 文件名使用小写字母和下划线。
3. 文件名表达设备类型，不表达临时测试目的。
4. 临时测试文件不要提交到正式库目录。

---

## 3. 类型命名规范

### 3.1 枚举

格式：

```cpp
enum Enum_模块名_用途
{
    模块名_用途_枚举项 = 0,
    模块名_用途_枚举项,
};
```

示例：

```cpp
/**
 * @brief 电机控制方式
 */
enum Enum_Motor_DJI_Control_Method
{
    Motor_DJI_Control_Method_CURRENT = 0,
    Motor_DJI_Control_Method_OMEGA,
    Motor_DJI_Control_Method_ANGLE,
};
```

规则：

1. 枚举类型以 `Enum_` 开头。
2. 枚举项必须带模块前缀，避免全局命名冲突。
3. 第一个枚举项显式赋值为 `0`。
4. 如果存在未初始化状态，使用 `UNDEFINED`。
5. 枚举项含义清楚时，不需要逐项注释。

---

### 3.2 结构体

格式：

```cpp
struct Struct_模块名_用途
{
    成员变量;
};
```

示例：

```cpp
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
```

规则：

1. 结构体以 `Struct_` 开头。
2. 原始协议帧结构体可以使用 `__attribute__((packed))`。
3. 处理后的业务数据结构体不随便加 `packed`。
4. 成员变量使用首字母大写的单词组合。
5. 当前值用 `Now_`，上一时刻用 `Pre_`，累计值用 `Total_`，标志位用 `_Flag`。

---

### 3.3 类

格式：

```cpp
/**
 * @brief XXX设备类
 */
class Class_模块名
{
public:

protected:

};
```

示例：

```cpp
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

protected:
    // 绑定的CAN管理对象
    Struct_CAN_Manage_Object *CAN_Manage_Object = nullptr;
};
```

规则：

1. 类名以 `Class_` 开头。
2. Device 类命名优先表达设备型号或协议对象。
3. 类内先写 `public`，再写 `protected`。
4. 没有必要时不强行写 `private`。
5. 对外可调的 PID 或配置对象可以放在 `public` 前部。
6. 内部状态、缓存指针、协议数据、私有函数放在 `protected`。

---

## 4. 函数命名规范

### 4.1 普通模块函数

格式：

```cpp
模块名_动作_补充说明()
```

示例：

```cpp
CAN_Transmit_Data();
Motor_DJI_CAN_Tx_PeriodElapsedCallback();
```

规则：

1. 函数名使用大驼峰单词 + 下划线分隔。
2. 模块名放前面。
3. 动作表达清楚，不为了短而牺牲可读性。
4. 周期任务函数可以使用 `PeriodElapsedCallback`。
5. 接收完成函数可以使用 `RxCpltCallback`。

---

### 4.2 类成员函数

Device 类成员函数优先使用以下命名：

```cpp
Init();
CAN_RxCpltCallback();
TIM_100ms_Alive_PeriodElapsedCallback();
TIM_Calculate_PeriodElapsedCallback();
Data_Process();
PID_Calculate();
Output();
Clean_Output();
```

规则：

1. 初始化函数统一叫 `Init()`。
2. CAN 接收完成处理统一叫 `CAN_RxCpltCallback()`。
3. 定时器周期函数统一使用 `TIM_xxx_PeriodElapsedCallback()`。
4. 数据解析过程叫 `Data_Process()`。
5. 控制计算叫 `PID_Calculate()` 或 `Control_Calculate()`。
6. 写入发送缓存叫 `Output()`。
7. 清空输出叫 `Clean_Output()`。

---

### 4.3 Getter / Setter

Getter：

```cpp
inline float Get_Now_Angle() const;
inline Enum_Motor_DJI_Status Get_Status() const;
```

Setter：

```cpp
inline void Set_Target_Angle(float __Target_Angle);
inline void Set_Control_Method(Enum_Motor_DJI_Control_Method __Control_Method);
```

规则：

1. Getter 使用 `Get_` 前缀。
2. Setter 使用 `Set_` 前缀。
3. Getter 必须加 `const`，除非函数确实会改变对象状态。
4. Getter/Setter 简单函数在头文件底部实现为 `inline`。
5. 参数使用 `__` 前缀，避免和成员变量重名。
6. Getter 返回值保持当前库风格：`return (Value);`。

示例：

```cpp
/**
 * @brief 获取当前角速度, rad/s
 *
 * @return float 当前角速度
 */
inline float Class_Motor_DJI_C620::Get_Now_Omega() const
{
    return (Rx_Data.Now_Omega);
}
```

---

## 5. 变量命名规范

### 5.1 全局变量

全局缓存按设备编号、帧 ID、用途命名：

```cpp
uint8_t CAN1_0x1fe_Tx_Data[8];
uint8_t CAN1_0x200_Tx_Data[8];
uint8_t CAN2_0x1ff_Tx_Data[8];
```

规则：

1. CAN 缓存格式：`CAN编号_帧ID_方向_Data`。
2. 接收用 `Rx_Data`，发送用 `Tx_Data`。
3. 全局变量只用于模块级共享状态或发送缓存。
4. 不要把任务层临时变量放到设备库全局变量区。

---

### 5.2 成员变量

常见成员变量命名：

```cpp
CAN_Manage_Object
CAN_Rx_ID
Tx_Data
Encoder_Offset
Motor_Status
Rx_Data
Control_Method
Target_Angle
Feedforward_Current
Now_External_Angle
External_Angle_Flag
```

规则：

1. 成员变量使用首字母大写的单词组合。
2. 指针变量不强制加 `p` 前缀，变量名表达对象即可。
3. 目标值使用 `Target_`。
4. 前馈值使用 `Feedforward_`。
5. 外部反馈值使用 `Now_External_`。
6. 标志位使用 `_Flag`。
7. 状态枚举变量使用 `Status` 或 `Motor_Status` 这类清楚名字。

---

### 5.3 常量成员

类内固定参数使用 `const` 成员：

```cpp
const uint16_t ENCODER_NUM_PER_ROUND = 8192U;
const float CURRENT_TO_OUT = 16384.0f / 20.0f;
const float THEORETICAL_OUTPUT_CURRENT_MAX = 16384.0f;
```

规则：

1. 固定转换系数使用全大写和下划线。
2. 物理单位写在注释里。
3. 浮点常量加 `.0f` 或 `f` 后缀。
4. 无符号整数加 `U` 后缀。
5. 不在函数里到处写魔法数，优先提成类内常量。

---

## 6. 注释规范

### 6.1 注释语言

1. 注释主体使用中文。
2. HAL、CAN、PID、Rx、Tx、Callback、ID 等专有词保持英文原名。
3. 单位写在 `@brief` 或 `@return` 里，例如 `rad/s`、`A`、`W`、`℃`。
4. 不写大段协议背景，协议说明放文档或手册笔记。
5. 不写情绪化注释或临时调试说明。

---

### 6.2 文件头注释

每个 `.h/.cpp` 顶部必须有文件头：

```cpp
/**
 * @file dvc_motor_dji.cpp
 * @author WangFonzhuo
 * @brief DJI电机的配置与操作
 * @version 1.0
 * @date 2026-05-22 27赛季
 */
```

规则：

1. `@file` 和文件名一致。
2. `@brief` 写该库职责。
3. `@date` 可以保留多行历史日期，但新文件至少写 27 赛季日期。
4. 文件头不写大段说明。

---

### 6.3 枚举、结构体、类注释

必须写 `@brief`：

```cpp
/**
 * @brief 电机状态
 */
enum Enum_Motor_DJI_Status
{
};
```

```cpp
/**
 * @brief 电机经过处理的数据
 */
struct Struct_Motor_DJI_Rx_Data
{
};
```

```cpp
/**
 * @brief C620无刷电调
 */
class Class_Motor_DJI_C620
{
};
```

规则：

1. 类型声明必须写 `@brief`。
2. 成员变量不用每个都写 Doxygen。
3. 成员变量分组用 `//` 注释。
4. 枚举项含义清楚时不逐项注释。

---

### 6.4 函数注释

普通函数定义使用：

```cpp
/**
 * @brief 初始化C620无刷电调
 *
 * @param hfdcan CAN编号
 * @param __CAN_Rx_ID 电机反馈报文ID枚举
 * @param __Control_Method 电机控制方式
 * @param __Encoder_Offset 编码器零点偏移
 * @param __Gearbox_Rate 减速箱减速比
 */
void Class_Motor_DJI_C620::Init(FDCAN_HandleTypeDef *hfdcan,
                                Enum_Motor_DJI_ID __CAN_Rx_ID,
                                Enum_Motor_DJI_Control_Method __Control_Method,
                                int32_t __Encoder_Offset,
                                float __Gearbox_Rate)
{
}
```

有返回值的函数使用：

```cpp
/**
 * @brief 估计功率值
 *
 * @param Power_Model 功率模型
 * @param Current 电流值
 * @param Omega 角速度值
 * @return float 估计功率值
 */
static float power_calculate(const Struct_Motor_DJI_Power_Model *Power_Model, float Current, float Omega)
{
}
```

规则：

1. 函数声明和函数定义都可以写注释，定义处必须写完整。
2. 有参数写 `@param`。
3. 有返回值写 `@return`。
4. 简单回调如果只有一个参数，可以保持紧凑格式。
5. 不要在 `@brief` 里写太长，复杂解释放函数内部关键位置。

---

### 6.5 函数内部注释

推荐写这种注释：

```cpp
// 判断该时间段内是否接收过电机数据
if (Flag == Pre_Flag)
{
}
```

```cpp
// 第一帧只建立编码器基准, 防止Pre_Encoder默认0导致误判跨圈
if (Rx_Data.Encoder_Init_Flag == false)
{
}
```

```cpp
// 处理大小端
Basic_Math_Endian_Reverse_16(...);
```

不推荐写这种注释：

```cpp
// 如果hfdcan为空则返回
if (hfdcan == nullptr)
{
    return;
}
```

规则：

1. 关键状态分支要写注释。
2. 协议解析、跨圈判断、掉线判断、缓冲区分配要写注释。
3. 空指针判断、普通赋值、简单 return 不写废话注释。
4. 注释不要比代码还长。

---

## 7. Include 规范

### 7.1 头文件 include

头文件只包含对外类型必须依赖的头文件：

```cpp
#include "alg_pid.h"
#include "drv_can.h"
```

规则：

1. 类成员里用到了某个类型，头文件必须 include 对应头文件。
2. 只在 `.cpp` 使用的工具函数，不放进 `.h`。
3. 不依赖间接 include，自己用到什么就 include 什么。
4. 不为了省事把大量 HAL 头文件塞进 Device 头文件。

---

### 7.2 源文件 include

源文件先 include 自己的头文件，再 include 内部实现需要的头文件：

```cpp
#include "dvc_motor_dji.h"
#include "alg_basic.h"
```

规则：

1. 自己的 `.h` 永远放第一位。
2. 内部数学、协议工具、HAL 辅助放在后面。
3. 不使用的 include 要删掉。

---

## 8. 类接口组织顺序

Device 类推荐按以下顺序组织：

```cpp
class Class_Device
{
public:
    // 对外可调对象，例如PID

    // Init

    // Getter

    // Setter

    // 通信接收回调

    // 定时器周期回调

protected:
    // 绑定对象和通信配置

    // 固定常量和模型参数

    // 运行状态和接收数据

    // 控制目标和前馈

    // 外部反馈

    // 内部处理函数
};
```

规则：

1. `Init()` 放在 public 函数最前面。
2. Getter 集中放在 Setter 前面。
3. 回调函数放在 Getter/Setter 后面。
4. `protected` 里先放绑定对象和 ID，再放常量，再放运行状态。
5. 内部函数声明放在 `protected` 最后。

---

## 9. inline 规范

### 9.1 什么函数可以 inline

可以 inline：

```text
1. Getter
2. Setter
3. 简单状态读取
4. 简单参数设置
```

不建议 inline：

```text
1. Init
2. CAN_RxCpltCallback
3. TIM_Calculate_PeriodElapsedCallback
4. Data_Process
5. PID_Calculate
6. Output
7. Clean_Output
```

规则：

1. `inline` 函数只在头文件底部 `Exported function definitions` 区实现。
2. 类内只写声明，不在类声明中直接展开实现。
3. Getter 必须 `const`。
4. inline 定义处写完整 Doxygen 注释。

---

## 10. 代码格式规范

### 10.1 缩进与大括号

使用 Allman 风格：

```cpp
if (hfdcan == nullptr)
{
    return;
}
else
{
    return;
}
```

规则：

1. 缩进使用 4 个空格。
2. 左大括号单独一行。
3. `if/else/switch/case/for/while` 即使只有一行，也写大括号。
4. 函数之间空一行。
5. 逻辑分组之间空一行。

---

### 10.2 switch-case

格式：

```cpp
switch (Control_Method)
{
    case (Motor_DJI_Control_Method_CURRENT):
    {
        break;
    }
    case (Motor_DJI_Control_Method_OMEGA):
    {
        break;
    }
    default:
    {
        Target_Current = 0.0f;

        break;
    }
}
```

规则：

1. `case` 后的枚举值加括号，保持当前库风格。
2. 每个 `case` 使用大括号包住。
3. 每个 `case` 末尾显式 `break`。
4. 推荐写 `default`，即使正常不会进入。

---

### 10.3 返回值与类型转换

返回值格式保持：

```cpp
return (Motor_Status);
return (nullptr);
```

字节拆分格式保持：

```cpp
tmp_out = (int16_t)Out;

Tx_Data[0] = (uint8_t)((uint16_t)tmp_out >> 8U);
Tx_Data[1] = (uint8_t)((uint16_t)tmp_out);
```

规则：

1. 简单返回值使用 `return (Value);`。
2. 底层字节处理允许使用当前库的 C 风格强转。
3. 同一个库中不要混用多种强转风格。
4. 对有符号输出拆字节时，先转成 `uint16_t` 再移位。

---

## 11. 防御性代码规范

### 11.1 空指针保护

外部传入指针必须保护：

```cpp
if (hfdcan == nullptr)
{
    return;
}
```

```cpp
if (Rx_Data_Buffer == nullptr)
{
    return;
}
```

规则：

1. `Init()`、回调、数据处理函数要检查外部指针。
2. 指针非法时直接 `return`。
3. 不在每个内部成员变量读取前都过度检查。

---

### 11.2 外设实例保护

外设实例按当前库风格处理：

```cpp
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
```

规则：

1. 不同芯片可能没有 FDCAN2/FDCAN3，使用 `#ifdef` 包住。
2. 未匹配到合法外设时直接 `return`。
3. 不要让对象半初始化后继续运行。

---

### 11.3 参数边界保护

容易导致除 0 或越界的参数要保护：

```cpp
if (__Gearbox_Rate > 1.0e-6f)
{
    Gearbox_Rate = __Gearbox_Rate;
}
```

输出值要限幅：

```cpp
Basic_Math_Constrain(&Out, -THEORETICAL_OUTPUT_CURRENT_MAX, THEORETICAL_OUTPUT_CURRENT_MAX);
```

规则：

1. 减速比、比例系数、长度、ID 等参数要做基本保护。
2. 电机输出必须限幅。
3. 保护逻辑保持简单，不写复杂异常系统。

---

## 12. Device 层通信与回调规范

### 12.1 接收路径

推荐路径：

```text
Driver层HAL中断
    -> Driver层用户回调
        -> Task层按ID分发
            -> Device对象.CAN_RxCpltCallback(Buffer)
                -> Device对象.Data_Process(Buffer)
```

规则：

1. Device 层不直接写 HAL 中断函数。
2. Task 层负责按 CAN ID 分发给对应对象。
3. Device 对象只处理属于自己的反馈数据。
4. 回调函数里尽量短，只增加 Flag 并调用数据处理。

示例：

```cpp
void Class_Motor_DJI_C620::CAN_RxCpltCallback(uint8_t *Rx_Data)
{
    Flag += 1U;
    Data_Process(Rx_Data);
}
```

---

### 12.2 周期计算路径

推荐路径：

```text
Task 1ms
    -> Device.TIM_Calculate_PeriodElapsedCallback()
    -> Module_CAN_Tx_PeriodElapsedCallback()
```

规则：

1. 控制计算和 CAN 发送由任务层周期调用。
2. 同一个发送缓存不要被多个地方同时写。
3. 发送函数统一发送该模块所有控制帧。
4. 不要在 Device 层自己启动定时器。

---

### 12.3 离线检测

离线检测推荐使用 `Flag / Pre_Flag`：

```cpp
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
```

规则：

1. 接收回调中更新 `Flag`。
2. 低频定时器中比较 `Flag` 和 `Pre_Flag`。
3. 掉线时控制计算函数必须清输出。
4. 需要更高安全性时，可以在离线检测处直接调用 `Clean_Output()`。

---

## 13. 数据处理规范

### 13.1 原始协议数据

原始协议帧使用结构体描述：

```cpp
struct Struct_Motor_DJI_CAN_Rx_Data
{
    uint16_t Encoder;
    int16_t Omega;
    int16_t Current;
    uint8_t Temperature;
    uint8_t Reserved;
} __attribute__((packed));
```

规则：

1. 原始协议帧结构体只描述协议布局。
2. 原始协议帧字段保持协议名，不强行改成应用层名字。
3. 大小端处理放在 `Data_Process()`。
4. 处理后的数据写入 `Rx_Data`。

---

### 13.2 处理后数据

处理后的数据统一放入 `Rx_Data`：

```cpp
Rx_Data.Now_Angle = ...;
Rx_Data.Now_Omega = ...;
Rx_Data.Now_Current = ...;
Rx_Data.Now_Temperature = ...;
Rx_Data.Now_Power = ...;
```

规则：

1. 对外 getter 只从 `Rx_Data` 或成员变量返回。
2. 角度统一用 `rad`。
3. 角速度统一用 `rad/s`。
4. 电流统一用 `A`。
5. 温度统一用 `℃`。
6. 功率统一用 `W`。

---

### 13.3 编码器累计

编码器跨圈处理按当前库风格：

```cpp
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
}
```

规则：

1. 第一帧只建立基准，不直接判断跨圈。
2. 跨圈阈值使用半圈编码器值。
3. 累计编码器使用 `int64_t`。
4. 编码器偏移使用 `Encoder_Offset`。

---

## 14. 输出规范

### 14.1 控制输出

控制输出流程：

```cpp
PID_Calculate();

Out = (Target_Current + Feedforward_Current) * CURRENT_TO_OUT;
Basic_Math_Constrain(&Out, -THEORETICAL_OUTPUT_CURRENT_MAX, THEORETICAL_OUTPUT_CURRENT_MAX);

Output();
```

规则：

1. 先算控制量，再换算协议输出值。
2. 输出值统一放在 `Out`。
3. 写入 CAN 缓冲区统一由 `Output()` 完成。
4. `Output()` 只负责写缓冲区，不负责真正发送 CAN。

---

### 14.2 清输出

清输出函数格式：

```cpp
void Class_Device::Clean_Output()
{
    Rx_Data.Encoder_Init_Flag = false;

    PID_Angle.Set_Integral_Error(0.0f);
    PID_Omega.Set_Integral_Error(0.0f);

    Out = 0.0f;

    if (Tx_Data != nullptr)
    {
        Tx_Data[0] = 0U;
        Tx_Data[1] = 0U;
    }
}
```

规则：

1. 清输出必须清 `Out`。
2. 清输出必须清发送缓存对应字节。
3. 掉线后可以重置编码器初始化标志。
4. PID 积分要清，防止重连后积分冲击。
5. 是否清目标值由具体设备策略决定，不强制。

---

## 15. 分层边界规范

### 15.1 Driver 层

Driver 层只做通用外设能力：

```text
CAN_Init
CAN_Transmit_Data
UART_Init
UART_Transmit_Data
```

Driver 层不要做：

```text
设备协议解析
电机控制算法
整车状态机
具体设备掉线策略
```

---

### 15.2 Device 层

Device 层可以做：

```text
协议解析
状态维护
离线检测
目标值接口
基础闭环控制封装
发送缓存管理
```

Device 层不要做：

```text
比赛策略
遥控器模式切换
整车任务调度
多个设备之间的复杂协同
```

---

### 15.3 Task 层

Task 层负责：

```text
初始化对象
注册回调
按ID分发数据
周期调用设备对象
组合多个设备完成业务逻辑
```

Task 层不要把设备协议解析代码复制出来。

---

## 16. 提交前检查清单

新增设备库提交前检查：

```text
[ ] 文件头注释完整
[ ] h/cpp 区块标题完整
[ ] 文件名符合层级命名
[ ] enum / struct / class 命名统一
[ ] public 接口顺序统一
[ ] protected 成员顺序统一
[ ] Getter 全部 const
[ ] Setter 参数使用 __ 前缀
[ ] inline 函数集中放在 h 文件底部
[ ] cpp 中函数定义有 Doxygen 注释
[ ] 外部指针有 nullptr 保护
[ ] 外设实例有非法分支 return
[ ] 输出值有限幅
[ ] 掉线后能清输出
[ ] 单位注释正确
[ ] 没有临时测试代码
[ ] 没有无意义 TODO
```

---

## 17. 新设备库最小模板

### 17.1 `dvc_xxx.h`

```cpp
/**
 * @file dvc_xxx.h
 * @author WangFonzhuo
 * @brief XXX的配置与操作
 * @version 1.0
 * @date 2026-xx-xx 27赛季
 */

#ifndef DVC_XXX_H
#define DVC_XXX_H

/* Includes ------------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief XXX状态
 */
enum Enum_XXX_Status
{
    XXX_Status_DISABLE = 0,
    XXX_Status_ENABLE,
};

/**
 * @brief XXX设备类
 */
class Class_XXX
{
public:
    /**
     * @brief 初始化XXX
     */
    void Init();

    /**
     * @brief 获取设备状态
     */
    inline Enum_XXX_Status Get_Status() const;

    /**
     * @brief CAN通信接收回调函数
     * @param Rx_Data 接收数据缓冲区
     */
    void CAN_RxCpltCallback(uint8_t *Rx_Data);

    /**
     * @brief TIM定时器中断计算回调函数
     */
    void TIM_Calculate_PeriodElapsedCallback();

protected:
    // 设备状态
    Enum_XXX_Status Status = XXX_Status_DISABLE;

    // 数据处理过程
    void Data_Process(uint8_t *Rx_Data_Buffer);
};

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 获取设备状态
 *
 * @return Enum_XXX_Status 设备状态
 */
inline Enum_XXX_Status Class_XXX::Get_Status() const
{
    return (Status);
}

#endif

/*----------------------------------------------------------------------------*/
```

---

### 17.2 `dvc_xxx.cpp`

```cpp
/**
 * @file dvc_xxx.cpp
 * @author WangFonzhuo
 * @brief XXX的配置与操作
 * @version 1.0
 * @date 2026-xx-xx 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "dvc_xxx.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 初始化XXX
 */
void Class_XXX::Init()
{
}

/**
 * @brief CAN通信接收回调函数
 * @param Rx_Data 接收数据缓冲区
 */
void Class_XXX::CAN_RxCpltCallback(uint8_t *Rx_Data)
{
    Data_Process(Rx_Data);
}

/**
 * @brief TIM定时器中断计算回调函数
 */
void Class_XXX::TIM_Calculate_PeriodElapsedCallback()
{
}

/**
 * @brief 数据处理过程
 * @param Rx_Data_Buffer 接收数据缓冲区
 */
void Class_XXX::Data_Process(uint8_t *Rx_Data_Buffer)
{
    if (Rx_Data_Buffer == nullptr)
    {
        return;
    }
}

/*----------------------------------------------------------------------------*/
```

---

## 18. 最终执行标准

后续新增设备库时，以这句话为准：

> 先让新库在文件结构、命名、注释、接口顺序、成员变量顺序上看起来像 `dvc_motor_dji`，再考虑具体设备的特殊性。

如果某个设备确实需要打破本规范，必须满足：

```text
1. 能说明为什么不能照搬当前规范
2. 只局部打破，不整体换风格
3. 同一个文件内保持一致
```

/*----------------------------------------------------------------------------*/

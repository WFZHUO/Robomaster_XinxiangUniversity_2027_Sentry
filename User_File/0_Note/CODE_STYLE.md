# 27赛季库代码风格规范

> 本文档用于统一 27 赛季哨兵工程后续库文件的**注释、命名、声明、定义、文件组织和分层边界**。  

---

## 0. 总原则

1. **统一优先于完美**：已有模块已经形成的风格，除非明显错误，否则后续尽量保持一致。
2. **接口清楚，内部简洁**：`h` 文件说明“怎么调用”，`cpp/c` 文件说明“怎么实现”。
3. **注释解释意图，不翻译代码**：不要写代码本身已经能看懂的废话注释。
4. **底层库只做底层的事**：Driver 不解析协议，BSP 不写任务逻辑，Device 不直接处理 HAL 中断。高内聚，低耦合

---

## 1. 文件组织规范

### 1.1 头文件 `.h` 基本结构

```cpp
/**
 * @file xxx_xxx.h
 * @author WangFonzhuo
 * @brief XXX
 * @version 1.0
 * @date 2026-xx-xx 27赛季
 */

#ifndef XXX_XXX_H
#define XXX_XXX_H

/*
根据需求加
#ifdef __cplusplus
extern "C" {
#endif
*/

/* Includes ------------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/* Exported function definitions ---------------------------------------------*/

/*
根据需求加
#ifdef __cplusplus
}
#endif
*/

#endif /* XXX_XXX_H */

/*----------------------------------------------------------------------------*/
```

规则：

1. 即使某些区块暂时不用，也保留区块标题。
2. `Exported function definitions` 只放简单 `inline` 函数、模板函数或必须放在头文件中的实现。
3. C++ 工程中只有需要被 C 文件调用的头文件，才加 `extern "C"`。
4. 不要在头文件中包含不必要的外部库，避免外部文件被迫引入过多依赖。

---

### 1.2 源文件 `.cpp/.c` 基本结构

```cpp
/**
 * @file xxx_xxx.cpp
 * @author WangFonzhuo
 * @brief XXX
 * @version 1.0
 * @date 2026-xx-xx 27赛季
 */

/* Includes ------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
```

规则：

1. 即使某些区块暂时不用，也保留区块标题。
2. `static` 私有函数原型放在 `Function prototypes` 区。
3. 私有宏、私有类型、私有变量优先放在源文件，不污染头文件。
4. 某个库如果不是对外接口必须依赖，只在源文件中 `#include`。

---

## 2. 命名风格规范

### 2.1 文件命名

```text
drv_xxx.h / drv_xxx.cpp      Driver 层，HAL 外设封装
bsp_xxx.h / bsp_xxx.cpp      BSP 层，板级硬件支持
alg_xxx.h / alg_xxx.cpp      Algorithm 层，算法库
dvc_xxx.h / dvc_xxx.cpp      Device 层，具体设备协议
sys_xxx.h / sys_xxx.cpp      System 层，系统级工具
...
```

示例：

```text
drv_uart.h
drv_can.h
bsp_buzzer.h
alg_waveform.h
dvc_serialplot.h
sys_timestamp.h
```

---

### 2.2 类型命名

```cpp
Struct_UART_Manage_Object
Class_ArkKey
Enum_Buzzer_Status
Namespace_SYS_Timestamp
```

规则：

```text
结构体：Struct_模块名_用途
类：Class_模块名
枚举：Enum_模块名_用途
命名空间：Namespace_模块名
```

---

### 2.3 函数命名

```cpp
UART_Init()
UART_Reinit()
UART_Transmit_Data()

CAN_Init()
CAN_Transmit_Data()

TIM_Init()
```

规则：

1. 使用“模块名前缀 + 动作”。
2. 模块名与动作之间用下划线连接。
3. 单词首字母大写。
4. 不使用过短或含义不明确的函数名。

---

### 2.4 变量命名

```cpp
UART1_Manage_Object
Rx_Buffer_Active
Rx_Buffer_Ready
Rx_Time_Stamp
Callback_Function
```

规则：

1. 全局管理对象：`模块号 + _Manage_Object`。
2. 回调函数指针：`Callback_Function`。
3. 接收缓冲区统一用 `Rx_Buffer`。
4. 发送缓冲区统一用 `Tx_Buffer`。
5. 时间戳统一用 `Time_Stamp` 或 `Timestamp`，同一文件内必须统一。
6. 变量名优先表达用途，不为了缩短而牺牲可读性。

---

## 3. Include 与 extern 规范

### 3.1 Include 放置原则

1. 对外接口必须使用的类型，对应头文件可以放在 `.h` 中。
2. 只在内部实现中使用的库，放在 `.cpp/.c` 中。
3. 不要因为某个源文件需要，就把 include 提前放进公共头文件。
4. 避免头文件层层包含导致编译依赖臃肿。
5. 用到哪个库就直接include那个库，不要间接通过别的库省事引入那个库，特殊情况除外。

示例：

```cpp
// h 文件中确实需要 UART_HandleTypeDef 时才包含
#include "usart.h"
```

```cpp
// 只在源文件内部使用时，放在 cpp/c 文件中
#include "sys_timestamp.h"
```

---

### 3.2 extern 变量规范

可以放在头文件中的 `extern`：

```cpp
// 声明UART管理对象
extern struct Struct_UART_Manage_Object UART1_Manage_Object;
```

适用情况：

1. 该变量是模块允许外部访问的管理对象。
2. 外部文件确实需要读取或传递该对象。
3. 该变量属于模块公开接口的一部分。

不建议放在头文件中的 `extern`：

```cpp
extern bool init_finished;
```

规则：

1. 只是源文件内部借用的变量，原则上优先放在 `.cpp/.c` 中。
2. 如果多个 Driver 已经采用同一风格，可以暂时保持一致。
3. 后续要重构时，再集中迁移，不要单独改一个模块造成风格割裂。

---

## 4. 注释总规则

### 4.1 推荐注释

推荐解释“为什么这么做”或“这段代码承担什么职责”。

```cpp
// 初始化未完成时也要清空FIFO, 防止FIFO满
```

```cpp
// 双缓冲适配的缓冲区 以及 当前激活的缓冲区
```

```cpp
// 接收时间戳
```

这些注释说明了工程意图，是有价值的。

---

### 4.2 不推荐注释

不推荐翻译代码本身。

```cpp
// 判断huart是不是USART1
if (huart->Instance == USART1)
```

```cpp
// 返回空指针
return nullptr;
```

这种代码本身已经很明显，不需要注释。

---

### 4.3 注释语言

1. 注释主体使用中文。
2. HAL 名称、寄存器名、类型名、函数名保持英文原名。
3. 不要中英文混乱解释同一个概念。
4. 专有名词第一次出现可以写“中文 + English”，后续直接用英文名。
5. 注释尽量短，长篇学习笔记不要塞进代码。

---

## 5. Doxygen 注释规范

### 5.1 文件头注释

所有 `.h`、`.cpp`、`.c` 文件顶部保留文件头注释。

```cpp
/**
 * @file drv_uart.cpp
 * @author WangFonzhuo
 * @brief UART通用驱动
 * @version 1.0
 * @date 2026-xx-xx 27赛季
 */
```

规则：

1. `@brief` 写清楚该文件职责。
2. 不在文件头写大段协议背景。
3. 重要硬件限制可以用 `@note` 简短说明。

---

### 5.2 宏定义注释

简单宏：

```cpp
// 缓冲区字节长度
#define UART_BUFFER_SIZE 512
```

```cpp
#define CAN_RX_BUFFER_SIZE 8
```

规则：

1. 宏名足够清楚时，可以不写注释。
2. 简单宏最多写一行说明。
3. 不要为了形式给每个宏写废话。

复杂宏：

```cpp
// CAN错误中断集合
#define CAN_ERROR_INTERRUPTS (FDCAN_IT_ERROR_WARNING      | \
                              FDCAN_IT_ERROR_PASSIVE      | \
                              FDCAN_IT_BUS_OFF)
```

规则：

1. 复杂宏必须说明用途。
2. 基础库里不要提前加入暂时不用的复杂宏。
3. 如果宏代表一组硬件标志位，命名要体现集合含义。

---

### 5.3 typedef 回调函数注释

参数简单时：

```cpp
/**
 * @brief UART通信接收回调函数数据类型
 */
typedef void (*UART_Callback)(uint8_t *Buffer, uint16_t Length);
```

参数容易混淆时：

```cpp
/**
 * @brief CAN通信接收回调函数数据类型
 *
 * @param Header 接收帧头
 * @param Buffer 接收数据缓冲区
 */
typedef void (*CAN_Callback)(FDCAN_RxHeaderTypeDef &Header, uint8_t *Buffer);
```

规则：

1. 回调函数类型必须写 `@brief`。
2. 参数简单时可以不展开 `@param`。
3. 参数含义容易混淆时必须写 `@param`。
4. 回调函数类型不要设计得过重，基础库只传必要数据。

---

### 5.4 结构体注释

```cpp
/**
 * @brief UART通信处理结构体
 */
struct Struct_UART_Manage_Object
{
    UART_HandleTypeDef *UART_Handler;
    UART_Callback Callback_Function;

    // 双缓冲适配的缓冲区 以及 当前激活的缓冲区
    uint8_t *Rx_Buffer_0;
    uint8_t *Rx_Buffer_1;

    // 正在接收的缓冲区
    uint8_t *Rx_Buffer_Active;

    // 接收完毕的缓冲区
    uint8_t *Rx_Buffer_Ready;

    // 接收时间戳
    uint64_t Rx_Time_Stamp;
};
```

规则：

1. 结构体必须有 `@brief`。
2. 成员变量不用每一项都写注释。
3. 分组成员建议用 `//` 简短说明。
4. 结构体里不要提前塞暂时不用的状态量。

---

### 5.5 枚举注释

普通枚举：

```cpp
/**
 * @brief 波形类型
 */
enum Enum_Waveform_Type
{
    Waveform_Type_Sine = 0,
    Waveform_Type_Square,
    Waveform_Type_Triangle,
    Waveform_Type_Sawtooth,
};
```

带特殊含义的枚举：

```cpp
/**
 * @brief CAN发送状态
 */
enum Enum_CAN_Tx_Status
{
    CAN_Tx_Status_OK = 0,       // 发送入队成功
    CAN_Tx_Status_BUSY,         // Tx FIFO/Queue已满
    CAN_Tx_Status_ERROR,        // 参数错误或HAL执行失败
};
```

规则：

1. `enum` 必须有 `@brief`。
2. 枚举项名字清楚时，不需要每项都加注释。
3. 枚举项含义特殊时，才写行内注释。
4. 不要为了形式给每一项写废话。

---

### 5.6 类声明注释

```cpp
/**
 * @brief 按键检测类
 */
class Class_ArkKey
{
public:
    void Init();

    void Update();

    Enum_ArkKey_Status Get_Status();

protected:

private:

};
```

规则：

1. `class` 必须有 `@brief`。
2. `public/protected/private` 分区保留。
3. 类内私有函数如果只是声明，可以用 `//` 简短注释。
4. 复杂函数的详细说明放到 `.cpp` 定义处。

---

### 5.7 函数声明注释

```cpp
/**
 * @brief 初始化UART
 *
 * @param huart UART编号
 * @param Callback_Function 回调函数
 */
void UART_Init(UART_HandleTypeDef *huart, UART_Callback Callback_Function);
```

规则：

1. `h` 文件中的普通函数声明必须写完整 Doxygen 注释。
2. 至少包含 `@brief`。
3. 有参数就写 `@param`。
4. 有返回值就写 `@return`。

---

### 5.8 函数定义注释

```cpp
/**
 * @brief UART发送数据
 *
 * @param huart UART编号
 * @param Data 被发送的数据指针
 * @param Length 数据长度
 * @return uint8_t HAL执行状态
 */
uint8_t UART_Transmit_Data(UART_HandleTypeDef *huart, uint8_t *Data, uint16_t Length)
{
    return HAL_UART_Transmit_DMA(huart, Data, Length);
}
```

规则：

1. `.cpp/.c` 定义处也要写完整注释。
2. 不要因为 `h` 文件写过就省略定义处注释。
3. 简短函数也保留注释，方便从源文件直接阅读。
4. 必要时对算法进行一定解释

---

### 5.9 static 私有函数注释

```cpp
static Struct_CAN_Manage_Object *CAN_Get_Manage_Object(FDCAN_HandleTypeDef *hfdcan);
```

规则：

1. `static` 函数原型放在 `Function prototypes` 区。
2. `static` 函数声明处可以不写 Doxygen 注释。
3. `static` 函数定义处写完整注释。

定义处示例：

```cpp
/**
 * @brief 获取CAN管理对象
 *
 * @param hfdcan CAN编号
 * @return Struct_CAN_Manage_Object* CAN管理对象
 */
static Struct_CAN_Manage_Object *CAN_Get_Manage_Object(FDCAN_HandleTypeDef *hfdcan)
{
}
```

---

### 5.10 HAL 回调函数注释

```cpp
/**
 * @brief HAL库UART接收DMA空闲中断
 *
 * @param huart UART编号
 * @param Size 长度
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
}
```

```cpp
/**
 * @brief HAL库CAN接收FIFO0中断
 *
 * @param hfdcan CAN编号
 * @param RxFifo0ITs Rx FIFO0中断标志
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
}
```

规则：

1. HAL 回调函数必须写完整注释。
2. `@brief` 格式统一写“HAL库XXX中断”。
3. 参数名按 HAL 原型，不随意改。
4. 回调里不要写大量业务逻辑，只做分发、缓存切换、时间戳、调用用户回调。

---

## 6. 类内函数与 inline 规范

### 6.1 适合 inline 的函数

适合放在头文件中的函数：

```text
1. 简单 getter
2. 简单 setter
3. 小型数学函数
4. 小型状态判断函数
5. 模板函数
```

示例：

```cpp
/**
 * @brief 获取当前输出值
 *
 * @return float 当前输出值
 */
float Get_Output()
{
    return Output;
}
```

规则：

1. 简单 getter / setter / 小工具函数可以直接写在 `h` 文件类内。
2. 如果这个函数是对外高频接口，写完整简洁注释。

---

### 6.2 不适合 inline 的函数

不适合放在头文件中的函数：

```text
1. 调用 HAL 的复杂初始化函数
2. 中断回调函数
3. 大型状态机函数
4. 含较多分支的解析函数
5. 需要隐藏实现细节的函数
```

这些函数应放到 `.cpp/.c` 文件中实现。

---

### 6.3 类内声明与类外定义的注释分工

头文件中：

```cpp
class Class_Waveform
{
public:
    /**
     * @brief 初始化波形参数
     *
     * @param __Frequency 波形频率
     * @param __Amplitude 波形幅值
     */
    void Init(float __Frequency, float __Amplitude);

    /**
     * @brief 更新函数
     *
     * @param __Delta_Time 时间差
     */
    void Update(float __Delta_Time);

    /**
     * @brief 输出
     *
     * @return float 当前输出值
     */
    float Get_Output();

protected:

private:
    // 归一化相位到[0, 1)
    float Wrap_01(float __P);

    // 正取模
    float Positive_Mod(float __X, float __Modulus);

    // 32位XORSHIFT伪随机数发生器
    uint32_t XORSHIFT32(uint32_t &__State);
};
```

源文件中：

```cpp
/**
 * @brief 初始化波形参数
 *
 * @param __Frequency 波形频率
 * @param __Amplitude 波形幅值
 */
void Class_Waveform::Init(float __Frequency, float __Amplitude)
{
}
```

规则：

1. 类内声明可以相对简短。
2. `.cpp` 定义必须详细。
3. 参数含义在 `.cpp` 定义处写完整。
4. `h` 文件不要重复写一大堆和 `.cpp` 完全一样的注释，除非该函数只在 `h` 中实现。
5. 私有函数在类内可以用 `//` 简短说明，详细注释放在源文件定义处。

---

## 7. 函数内部注释规范

### 7.1 推荐写注释的位置

函数内部只在这些位置写注释：

```text
1. 状态机关键分支
2. 缓冲区切换
3. 中断重启接收
4. 硬件限制相关操作
5. 防止隐藏 bug 的特殊处理
6. 初始化未完成时的保护逻辑
```

示例：

```cpp
// 判断程序初始化完成
if (!init_finished)
{
    // 重启接收
    HAL_UARTEx_ReceiveToIdle_DMA(...);
    return;
}
```

```cpp
// 初始化未完成时也要清空FIFO, 防止FIFO满
while (HAL_FDCAN_GetRxMessage(...) == HAL_OK)
{
}
```

---

### 7.2 不推荐写注释的位置

不写这种注释：

```cpp
// 如果为空指针则返回
if (ptr == nullptr)
{
    return;
}
```

```cpp
// 调用HAL函数发送数据
HAL_UART_Transmit_DMA(...);
```

除非这里有特殊原因，否则不要解释代码表面动作。

---

## 8. @note / @return / TODO 规范

### 8.1 @note 使用规范

只有这些情况才用 `@note`：

```text
1. 文件级说明
2. 函数有重要使用前提
3. 函数有硬件限制
4. 函数有时序要求
5. 需要提醒后续维护者不要误改
```

示例：

```cpp
/**
 * @brief 初始化CAN总线
 *
 * @note
 * 1. 调用前需要先完成 MX_FDCANx_Init().
 * 2. 当前版本默认只配置标准帧过滤器.
 *
 * @param hfdcan CAN编号
 * @param Callback_Function 处理回调函数
 */
void CAN_Init(FDCAN_HandleTypeDef *hfdcan, CAN_Callback Callback_Function);
```

不推荐把普通流程都写成 `@note`。

---

### 8.2 @return 写法规范

如果返回 HAL 状态：

```cpp
@return uint8_t HAL执行状态
```

如果返回对象指针：

```cpp
@return Struct_CAN_Manage_Object* CAN管理对象
```

如果返回当前值：

```cpp
@return float 当前输出值
```

规则：

1. `@return` 后面先写返回类型，再写含义。
2. 不写“返回值”这种无意义说明。
3. 返回状态时说明状态来源，比如 HAL 状态、自定义状态、错误码。

---

### 8.3 TODO / FIXME 规范

```cpp
// TODO: 后续增加Bus-Off恢复策略
```

```cpp
// FIXME: 当前滤波器配置仅适用于标准帧
```

规则：

1. `TODO` 表示计划增强。
2. `FIXME` 表示当前存在问题。
3. 不要写模糊 TODO。
4. TODO 后面必须能看懂要做什么。
5. 能立刻修的小问题不要写 TODO，直接修掉。

---

## 9. init_finished 使用规范

```cpp
// 判断程序初始化完成
if (!init_finished)
{
    // 初始化未完成时只做必要清理或重启接收
    return;
}
```

规则：

1. `init_finished` 只用于防止系统初始化阶段误触发回调。
2. 初始化未完成时不要进入用户回调。
3. 初始化未完成时可以做“清 FIFO / 重启接收”这种保护动作。
4. 不要把 `init_finished` 当作某个驱动自己的 `Init_Finish`。
5. 如果多个 Driver 已经统一使用该变量，后续保持一致；若要重构，集中处理。

---

## 10. 分层设计规范

### 10.1 Driver 层规范

Driver 层只做：

```text
1. HAL 外设封装
2. 中断接收
3. 缓冲区管理
4. 时间戳记录
5. 回调分发
6. 必要的参数检查
```

Driver 层不要做：

```text
1. 电机协议解析
2. 裁判系统协议解析
3. 控制算法
4. 业务状态机
5. 复杂错误恢复策略
6. 大量应用层缓存
```

原则：

1. Driver 层只提供“通用收发能力”。
2. 具体协议解析放到 Device 层。
3. 复杂控制逻辑放到 Task 层或 Control 层。
4. Driver 不主动依赖上层模块。

---

### 10.2 BSP 层规范

BSP 更贴近板级硬件。

```cpp
/**
 * @brief 蜂鸣器控制类
 */
class Class_Buzzer
{
public:
    // 初始化蜂鸣器
    void Init();

    // 设置频率
    void Set_Frequency(uint32_t Frequency);

    // 开始播放
    void Start();

    // 停止播放
    void Stop();
};
```

规则：

1. BSP 类注释写清楚硬件对象。
2. 不在 BSP 类里写任务层逻辑。
3. 类内简单操作可以 inline。
4. 涉及 HAL 的复杂操作放到 `.cpp`。
5. BSP 不负责整车策略。

---

### 10.3 Algorithm 层规范

算法类一般用 `Class_XXX`。

```cpp
/**
 * @brief 波形发生器类
 */
class Class_Waveform
{
public:
    // 初始化
    void Init();

    // 更新输出
    void Update();

    // 获取输出值
    float Get_Output();

private:
    float Output;
};
```

cpp 定义处：

```cpp
/**
 * @brief 更新输出
 *
 * @note
 * 根据当前波形类型和时间计算输出值.
 */
void Class_Waveform::Update()
{
}
```

规则：

1. 算法层不直接依赖具体硬件。
2. 算法层不直接调用 HAL。
3. 算法层只关注输入、输出和状态更新。
4. 算法推导不要写进代码注释，长推导放笔记文档。

---

### 10.4 Device 层规范

Device 层用于具体设备协议。

```cpp
/**
 * @brief 电机设备类
 */
class Class_Motor
{
public:
    // 初始化电机
    void Init();

    // CAN反馈数据处理
    void CAN_RxCallback(uint8_t *Data);

    // 设置目标值
    void Set_Target(float Target);
};
```

规则：

1. Device 层可以解析协议。
2. Device 层不要直接处理底层中断。
3. Device 层通过 Driver 层回调获得数据。
4. Device 层可以维护设备状态、离线检测、协议数据。
5. Device 层不写整车业务状态机。



---

## 12. 空函数与预留函数规范

如果必须保留空函数，写清楚用途：

```cpp
/**
 * @brief CAN的TIM定时器中断发送回调函数
 */
void TIM_1ms_CAN_PeriodElapsedCallback()
{
}
```

规则：

1. 空函数可以保留，但必须有明确用途。
2. 不要加入一堆没有计划使用的空函数。
3. 空函数不写 TODO，除非马上要补。
4. 如果只是临时测试函数，提交前删除。

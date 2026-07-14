/**
 * @file bsp_w25q64jv.h
 * @author WangFonzhuo
 * @brief W25Q64JV串行Flash的配置与操作
 * @version 1.0
 * @date 2026-07-14 27赛季
 * @note 当前版本仅完成人工上板功能测试并通过, 尚未进行人工逐行代码审查.
 */

#ifndef BSP_W25Q64JV_H
#define BSP_W25Q64JV_H

/* Includes ------------------------------------------------------------------*/

#include "bsp_w25q64jv_register.h"
#include "drv_ospi.h"

#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

// W25Q64JV总容量, 8MB
#define W25Q64JV_SIZE          (8U * 1024U * 1024U)

// W25Q64JV单页大小, 256B
#define W25Q64JV_PAGE_SIZE     (256U)

// W25Q64JV单扇区大小, 4KB
#define W25Q64JV_SECTOR_SIZE   (4U * 1024U)

// W25Q64JV JEDEC ID
#define W25Q64JV_JEDEC_ID      (0x00EF4017U)

// 状态寄存器1 BUSY位
#define W25Q64JV_STATUS_1_BUSY (0x01U)

// 状态寄存器1 WEL位
#define W25Q64JV_STATUS_1_WEL  (0x02U)

// 状态寄存器2 QE位
#define W25Q64JV_STATUS_2_QE   (0x02U)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief W25Q64JV串行Flash
 *
 * @note 本库使用OSPI中断和MDMA异步执行.
 * @note HAL_OK仅表示异步操作成功启动, 操作完成需通过Get_Busy_Flag()判断.
 * @note 自动轮询当前未配置软件超时, 异常情况下可能长期保持Busy, 但不会阻塞CPU运行.
 * @note 当前版本仅完成人工上板功能测试并通过, 尚未进行人工逐行代码审查、长期稳定性测试和异常注入测试.
 */
class Class_W25Q64JV
{
public:
    /**
     * @brief 异步初始化W25Q64JV, 自动读取JEDEC ID并检查或开启QE
     *
     * @note 返回HAL_OK仅表示初始化流程成功启动, 完成状态通过Get_Init_Finished_Flag()判断.
     * @param __hospi 绑定的OSPI
     * @return uint8_t HAL执行状态
     */
    uint8_t Init(OSPI_HandleTypeDef *__hospi);

    /**
     * @brief 异步四线读取数据
     *
     * @note 单次读取长度不能超过OSPI_BUFFER_SIZE.
     * @note 操作完成后通过Get_Rx_Buffer()和Get_Rx_Length()获取数据.
     * @note 接收缓冲区由OSPI底层管理, 下一次OSPI接收可能覆盖原数据.
     * @param __Address 读取起始地址
     * @param __Length 读取长度, Byte
     * @return uint8_t HAL执行状态
     */
    uint8_t Get_Buffer(uint32_t __Address,
                       uint32_t __Length);

    /**
     * @brief 异步四线页编程, 内部自动写使能并等待Flash编程完成
     *
     * @note 单次写入长度不能超过256B且不能跨页.
     * @note NOR Flash写入只能将Bit由1改为0, 需要将0恢复为1时必须先擦除所在扇区.
     * @note 数据会先复制到类内部缓冲区, 函数返回后调用者可以修改原缓冲区.
     * @param __Buffer 待写入数据
     * @param __Address 写入起始地址
     * @param __Length 写入长度, Byte
     * @return uint8_t HAL执行状态
     */
    uint8_t Set_Buffer(const uint8_t *__Buffer,
                       uint32_t __Address,
                       uint32_t __Length);

    /**
     * @brief 异步擦除4KB扇区, 内部自动写使能并等待Flash擦除完成
     *
     * @note 地址必须按照4KB扇区边界对齐.
     * @param __Address 扇区起始地址
     * @return uint8_t HAL执行状态
     */
    uint8_t Set_Sector_Erased(uint32_t __Address);

    /**
     * @brief OSPI底层完成或错误回调, 直接推进W25Q64JV异步操作
     *
     * @note 该函数由drv_ospi回调转发, 应用层不直接调用.
     * @param __Operation OSPI完成或发生错误的操作类型
     * @param __Buffer OSPI接收或发送缓冲区
     * @param __Length 有效数据长度
     * @param __Error_Code HAL OSPI错误码
     */
    void OSPI_Callback(enum Enum_OSPI_Operation __Operation,
                       uint8_t *__Buffer,
                       uint32_t __Length,
                       uint32_t __Error_Code);

    /**
     * @brief 获取W25Q64JV初始化完成标志
     *
     * @return bool 初始化完成标志
     */
    inline bool Get_Init_Finished_Flag() const;

    /**
     * @brief 获取W25Q64JV忙标志
     *
     * @return bool 当前存在初始化、读取、写入或擦除操作时为true
     */
    inline bool Get_Busy_Flag() const;

    /**
     * @brief 获取W25Q64JV JEDEC ID
     *
     * @return uint32_t JEDEC ID
     */
    inline uint32_t Get_JEDEC_ID() const;

    /**
     * @brief 获取最近一次OSPI接收缓冲区
     *
     * @return const uint8_t* OSPI底层接收缓冲区
     */
    inline const uint8_t *Get_Rx_Buffer() const;

    /**
     * @brief 获取最近一次OSPI接收数据长度
     *
     * @return uint32_t 接收数据长度, Byte
     */
    inline uint32_t Get_Rx_Length() const;

protected:
    /**
     * @brief W25Q64JV当前完整操作类型
     */
    enum Enum_Operation : uint8_t
    {
        Operation_NONE = 0,
        Operation_INIT,
        Operation_READ,
        Operation_PAGE_PROGRAM,
        Operation_SECTOR_ERASE,
    };

    // 绑定的OSPI
    OSPI_HandleTypeDef *hospi = nullptr;

    // 当前完整操作及当前Flash指令
    volatile enum Enum_Operation Operation = Operation_NONE;
    volatile enum Enum_W25Q64JV_Command Current_Instruction = W25Q64JV_Command_JEDEC_ID;

    // 初始化状态及Flash信息
    volatile bool Init_Finished_Flag = false;
    volatile uint32_t JEDEC_ID = 0U;
    uint8_t Status_Register_2 = 0U;

    // 最近一次OSPI接收结果, 指向drv_ospi内部接收缓冲区
    const uint8_t *volatile Rx_Buffer = nullptr;
    volatile uint32_t Rx_Length = 0U;

    // 页编程内部发送缓冲区
    uint8_t Tx_Buffer[W25Q64JV_PAGE_SIZE] = {};

    // 等待执行的地址和数据长度
    volatile uint32_t Pending_Address = 0U;
    volatile uint32_t Pending_Length = 0U;

    // 内部指令构造与异步流程函数
    OSPI_RegularCmdTypeDef Build_Command(enum Enum_W25Q64JV_Command __Instruction) const;

    // 启动JEDEC ID异步读取
    uint8_t Start_Read_ID();

    // 启动状态寄存器2异步读取
    uint8_t Start_Read_Status_Register_2();

    // 启动写使能指令
    uint8_t Start_Write_Enable();

    // 启动状态寄存器2异步写入, 开启QE四线模式
    uint8_t Start_Write_Status_Register_2();

    // 启动四线页编程
    uint8_t Start_Page_Program();

    // 启动4KB扇区擦除
    uint8_t Start_Sector_Erase();

    // 启动状态寄存器1硬件自动轮询
    uint8_t Start_Auto_Polling(uint32_t __Match, uint32_t __Mask);

    // 完成当前W25Q64JV操作并恢复空闲状态
    void Finish_Operation();

    // 终止当前W25Q64JV操作并恢复空闲状态
    void Fail_Operation();
};

/* Exported variables --------------------------------------------------------*/

// 全局W25Q64JV对象
extern Class_W25Q64JV BSP_W25Q64JV;

/* Exported function prototypes ----------------------------------------------*/

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 获取W25Q64JV初始化完成标志
 *
 * @return bool 初始化完成标志
 */
inline bool Class_W25Q64JV::Get_Init_Finished_Flag() const
{
    return (Init_Finished_Flag);
}

/**
 * @brief 获取W25Q64JV忙标志
 *
 * @return bool 当前存在初始化、读取、写入或擦除操作时为true
 */
inline bool Class_W25Q64JV::Get_Busy_Flag() const
{
    return (Operation != Operation_NONE);
}

/**
 * @brief 获取W25Q64JV JEDEC ID
 *
 * @return uint32_t JEDEC ID
 */
inline uint32_t Class_W25Q64JV::Get_JEDEC_ID() const
{
    return (JEDEC_ID);
}

/**
 * @brief 获取最近一次OSPI接收缓冲区
 *
 * @return const uint8_t* OSPI底层接收缓冲区
 */
inline const uint8_t *Class_W25Q64JV::Get_Rx_Buffer() const
{
    return (Rx_Buffer);
}

/**
 * @brief 获取最近一次OSPI接收数据长度
 *
 * @return uint32_t 接收数据长度, Byte
 */
inline uint32_t Class_W25Q64JV::Get_Rx_Length() const
{
    return (Rx_Length);
}

#endif /* BSP_W25Q64JV_H */

/*----------------------------------------------------------------------------*/
